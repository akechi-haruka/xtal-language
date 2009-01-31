#include "xtal.h"
#include "xtal_macro.h"

namespace xtal{

class ArrayIter : public Base{
	ArrayPtr array_;
	int_t index_;
	bool reverse_;

	virtual void visit_members(Visitor& m){
		Base::visit_members(m);
		m & array_;
	}

public:

	ArrayIter(const ArrayPtr& a, bool reverse = false)
		:array_(a), index_(-1), reverse_(reverse){
	}
			
	void block_next(const VMachinePtr& vm){
		++index_;
		if(index_<(int_t)array_->size()){
			vm->return_result(from_this(this), array_->at(reverse_ ? array_->size()-1-index_ : index_));
		}
		else{
			vm->return_result(null, null);
		}
	}
};

void Array::visit_members(Visitor& m){
	Base::visit_members(m);
	for(uint_t i=0; i<size_; ++i){
		m & values_[i];
	}
}

Array::Array(uint_t size){
	capa_ = size + 3;
	size_ = size;
	values_ = (AnyPtr*)user_malloc(sizeof(AnyPtr)*capa_);
	std::memset(values_, 0, sizeof(AnyPtr)*size_);
}

Array::Array(const AnyPtr* first, const AnyPtr* end){
	int_t size = end-first;

	capa_ = size;
	size_ = size;
	values_ = (AnyPtr*)user_malloc(sizeof(AnyPtr)*capa_);

	for(int_t i=0; i<size; ++i){
		copy_innocence(values_[i], first[i]);
		inc_ref_count_force(values_[i]);
	}
}

Array::Array(const Array& v){
	size_ = capa_ = ((Array&)v).size();
	values_ = (AnyPtr*)user_malloc(sizeof(AnyPtr)*capa_);
	
	for(uint_t i=0; i<size_; ++i){
		copy_innocence(values_[i], v.values_[i]);
		inc_ref_count_force(values_[i]);
	}
}

Array& Array::operator =(const Array& v){
	Array temp(v);
	std::swap(values_, temp.values_);
	std::swap(size_, temp.size_);
	std::swap(capa_, temp.capa_);
	return *this;
}

Array::~Array(){
	for(uint_t i=0; i<size_; ++i){
		dec_ref_count_force(values_[i]);
	}
	user_free(values_);
}

void Array::clear(){
	for(uint_t i=0; i<size_; ++i){
		dec_ref_count_force(values_[i]);
	}
	size_ = 0;
}

void Array::resize(uint_t sz){
	if(sz<0) sz = 0;

	if(sz<size_){
		for(uint_t i=sz; i<size_; ++i){
			dec_ref_count_force(values_[i]);
		}
		size_ = sz;
	}
	else if(sz>size_){
		if(sz>capa_){
			uint_t newcapa = sz+capa_;
			AnyPtr* newp = (AnyPtr*)user_malloc(sizeof(AnyPtr)*newcapa);
			std::memcpy(newp, values_, sizeof(AnyPtr)*size_);
			std::memset(&newp[size_], 0, sizeof(AnyPtr)*(sz-size_));
			user_free(values_);
			values_ = newp;
			size_ = sz;
			capa_ = newcapa;
		}
		else{
			std::memset(&values_[size_], 0, sizeof(AnyPtr)*(sz-size_));
			size_ = sz;
		}
	}
}

const AnyPtr& Array::op_at(int_t i){
	return values_[calc_offset(i)];

}

void Array::op_set_at(int_t i, const AnyPtr& v){
	values_[calc_offset(i)] = v;
}

void Array::erase(int_t start, int_t n){
	int pos = calc_offset(start);
	if(n<0 || (uint_t)(n + pos)>size_){
		throw_index_error();
		return;
	}

	for(int_t i=0; i<n; ++i){
		dec_ref_count_force(values_[pos + i]);
	}

	size_ -= n;
	if(size_!=0){
		std::memmove(&values_[pos], &values_[pos+n], sizeof(AnyPtr)*(size_-pos));
	}
}

void Array::insert(int_t i, const AnyPtr& v){
	if(capa_==size_){
		resize(size_ + 1);
	}
	else{
		size_++;
	}
	int_t pos = calc_offset(i);
	std::memmove(&values_[pos+1], &values_[pos], sizeof(AnyPtr)*(size_-1-pos));
	copy_innocence(values_[pos], v);
	inc_ref_count_force(values_[pos]);
}

void Array::push_back(const AnyPtr& v){
	if(capa_==size_){
		resize(size_ + 1);
	}
	else{
		size_++;
	}
	copy_innocence(values_[size_-1], v);
	inc_ref_count_force(values_[size_-1]);
}

void Array::pop_back(){
	if(!empty()){
		dec_ref_count_force(values_[size_-1]);
		size_--;
	}
}

ArrayPtr Array::slice(int_t start, int_t n){
	if(n==0){
		return xnew<Array>(0);
	}

	int_t pos = calc_offset(start);
	if(n<0 || (uint_t)(n + pos)>size_){
		throw_index_error();
		return null;
	}

	return xnew<Array>(&values_[pos], &values_[pos+n]);
}

ArrayPtr Array::splice(int_t start, int_t n){
	ArrayPtr ret = slice(start, n);
	erase(start, n);
	return ret;
}

ArrayPtr Array::clone(){
	return xnew<Array>(*this);
}

ArrayPtr Array::cat(const ArrayPtr& a){
	ArrayPtr ret(clone());
	ret->cat_assign(a);
	return ret;
}

ArrayPtr Array::cat_assign(const ArrayPtr& a){
	for(uint_t i = 0, size = a->size(); i<size; ++i){
		push_back(a->at(i));
	}
	return from_this(this);
}
	
StringPtr Array::join(const StringPtr& sep){
	return ptr_cast<String>(send(Xid(join), sep));
}

bool Array::op_eq(const ArrayPtr& other){
	if(size()!=other->size())
		return false;

	const VMachinePtr& vm = vmachine();
	for(uint_t i=0, size=other->size(); i<size; ++i){
		if(rawne(at(i), other->at(i))){
			vm->setup_call(1, other->at(i));
			at(i)->rawsend(vm, Xid(op_eq));
			if(!vm->processed() || !vm->result()){
				vm->cleanup_call();
				return false;
			}
			vm->cleanup_call();
		}
	}
	return true;
}

AnyPtr Array::each(){
	return xnew<ArrayIter>(from_this(this));
}

AnyPtr Array::reverse(){
	return xnew<ArrayIter>(from_this(this), true);
}

void Array::assign(const AnyPtr& iterator){
	clear();

	uint_t i = 0;
	Xfor(v, iterator){
		if(i>=size_){
			resize(i+1);
		}

		set_at(i, v);
		++i;
	}
}

void Array::concat(const AnyPtr& iterator){
	Xfor(v, iterator){
		push_back(v);
	}
}

int_t Array::calc_offset(int_t i){
	if(i<0){
		i = size_ + i;
		if(i<0){
			throw_index_error();
			return 0;
		}
	}
	else{
		if((uint_t)i >= size_){
			throw_index_error();
			return 0;
		}
	}
	return i;
}

void Array::throw_index_error(){
	XTAL_THROW(RuntimeError()->call(Xt("Xtal Runtime Error 1020")), return);
}

MultiValuePtr MultiValue::clone(){
	return xnew<MultiValue>(*this);
}

MultiValuePtr MultiValue::flatten_mv(){
	if(empty()){ return xnew<MultiValue>(); }

	MultiValuePtr ret = xnew<MultiValue>();
	ret->concat(from_this(this));
	
	for(;;){
		if(const MultiValuePtr& mv = ptr_as<MultiValue>(ret->back())){
			ret->pop_back();
			ret->concat(mv);
		}
		else{
			if(type(ret->back())==TYPE_UNDEFINED){
				ret->pop_back();
			}
			break;
		}
	}
	return ret;
}

MultiValuePtr MultiValue::flatten_all_mv(){
	if(empty()){ return xnew<MultiValue>(); }

	MultiValuePtr ret = xnew<MultiValue>();
	MultiValuePtr temp(from_this(this));
	
	for(uint_t i=0; i<temp->size(); ++i){
		if(const MultiValuePtr& mv = ptr_as<MultiValue>(temp->at(i))){
			ret->concat(mv->flatten_all_mv());
		}
		else{
			if(type(ret->back())!=TYPE_UNDEFINED){
				ret->push_back(temp->at(i));
			}
		}
	}
	return ret;
}

//////////////////////////////////////////////

void initialize_array(){
	
	{
		ClassPtr p = new_cpp_class<ArrayIter>(Xid(ArrayIter));
		p->inherit(Iterator());
		p->method(Xid(block_next), &ArrayIter::block_next);
	}

	{
		ClassPtr p = new_cpp_class<Array>(Xid(Array));
		p->inherit(Iterable());

		p->def(Xid(new), ctor<Array, int_t>()->params(Xid(size), 0));
		
		p->method_static<uint_t (Array::*)(), &Array::size>(Xid(size));
		p->method_static<uint_t (Array::*)(), &Array::size>(Xid(length));
		p->method_static<void (Array::*)(uint_t), &Array::resize>(Xid(resize));
		p->method_static<bool (Array::*)(), &Array::empty>(Xid(empty));
		p->method_static<ArrayPtr (Array::*)(int_t, int_t), &Array::slice>(Xid(slice))->params(Xid(i), null, Xid(n), 1);
		p->method_static<ArrayPtr (Array::*)(int_t, int_t), &Array::splice>(Xid(splice))->params(Xid(i), null, Xid(n), 1);
		p->method_static<void (Array::*)(), &Array::pop_back>(Xid(pop_back));
		p->method_static<void (Array::*)(), &Array::pop_front>(Xid(pop_front));
		p->method_static<void (Array::*)(const AnyPtr&), &Array::push_back>(Xid(push_back));
		p->method_static<void (Array::*)(const AnyPtr&), &Array::push_front>(Xid(push_front));

		p->method(Xid(erase), &Array::erase)->params(Xid(i), null, Xid(n), 1);
		p->method(Xid(insert), &Array::insert);
		p->method(Xid(to_a), &Array::to_a);
		p->method(Xid(each), &Array::each);
		p->method(Xid(clone), &Array::clone);
		p->method(Xid(front), &Array::front);
		p->method(Xid(back), &Array::back);
		p->method(Xid(clear), &Array::clear);
		p->method(Xid(reverse), &Array::reverse);
		p->method(Xid(assign), &Array::assign);
		p->method(Xid(concat), &Array::concat);

		p->method(Xid(op_cat), &Array::cat, get_cpp_class<Array>());
		p->method(Xid(op_cat_assign), &Array::cat_assign, get_cpp_class<Array>());
		p->method(Xid(op_at), &Array::op_at, get_cpp_class<Int>());
		p->method(Xid(op_set_at), &Array::op_set_at, get_cpp_class<Int>());
		p->method(Xid(op_eq), &Array::op_eq, get_cpp_class<Array>());
	}

	{
		ClassPtr p = new_cpp_class<MultiValue>(Xid(MultiValue));
		p->inherit(get_cpp_class<Array>());

		p->def(Xid(new), ctor<MultiValue, int_t>()->params(Xid(size), 0));
		p->method(Xid(clone), &MultiValue::clone);
		p->method(Xid(to_a), &Array::clone);
		p->method(Xid(to_mv), &Any::self);
		p->method(Xid(flatten_mv), &MultiValue::flatten_mv);
		p->method(Xid(flatten_all_mv), &MultiValue::flatten_all_mv);
	}

	{
		ClassPtr p = new_cpp_class<Expr>(Xid(Expr));
		p->inherit(get_cpp_class<Array>());		
	}

	builtin()->def(Xid(Array), get_cpp_class<Array>());
	builtin()->def(Xid(MultiValue), get_cpp_class<MultiValue>());
}

void initialize_array_script(){
Xemb((
	Array::block_first: method this.each.block_first;

Array::join: method(sep: ""){
	sep = sep.to_s;
	ms: MemoryStream();
	if(sep===""){
		this.each{
			ms.put_s(it.to_s);
		}
	}
	else{
		this.each{
			if(!first_step){
				ms.put_s(sep);
			}
			ms.put_s(it.to_s);
		}
	}
	return ms.to_s;
}

Array::to_s: method{
	return %f([%s])(this.join(", "));
}
MultiValue::to_s: method{
	return %f((%s))(this.join(", "));
}

),
""
)->call();

}

}
