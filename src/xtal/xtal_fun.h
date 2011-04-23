/** \file src/xtal/xtal_fun.h
* \brief src/xtal/xtal_fun.h
*/

#ifndef XTAL_FUN_H_INCLUDE_GUARD
#define XTAL_FUN_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/**
* \xbind lib::builtin
* \xinherit lib::builtin::Any
* \brief �����I�u�W�F�N�g
*/
class Arguments : public Base{
public:

	/**
	* \xbind
	* \brief ���Ԏw������A���O�t��������n���Ĉ����I�u�W�F�N�g���\�z����
	*/
	Arguments(const AnyPtr& ordered = undefined, const AnyPtr& named = undefined);

	const AnyPtr& op_at_int(uint_t index);

	const AnyPtr& op_at_string(const StringPtr& key);

	/**
	* \xbind
	* \brief key�ɑΉ����������Ԃ�
	* key�������ł���΁A���Ԏw������A������ł���Ζ��O�t��������Ԃ�
	*/
	const AnyPtr& op_at(const AnyPtr& key){
		if(XTAL_detail_is_ivalue(key)){
			return op_at_int(XTAL_detail_ivalue(key));
		}
		return op_at_string(key->to_s());
	}

	/**
	* \xbind
	* \brief ���Ԏw������̐���Ԃ�
	*/
	uint_t length();
	
	/**
	* \xbind
	* \brief ���Ԏw�������񋓂��邽�߂̃C�e���[�^��Ԃ�
	*/
	AnyPtr ordered_arguments();
	
	/**
	* \xbind
	* \brief ���O�t��������񋓂��邽�߂̃C�e���[�^��Ԃ�
	*/
	AnyPtr named_arguments();

	AnyPtr each();

	StringPtr to_s();

public:

	void add_ordered(const AnyPtr& v);

	void add_named(const AnyPtr& k, const AnyPtr& v);

	void add_named(const VMachinePtr& vm);

	uint_t ordered_size();

	uint_t named_size();

	void on_visit_members(Visitor& m);

private:

	ArrayPtr ordered_;
	MapPtr named_;
};

class ArgumentsIter : public Base{
public:

	ArgumentsIter(const ArgumentsPtr& a);
			
	void block_next(const VMachinePtr& vm);

	void on_visit_members(Visitor& m);

private:
	SmartPtr<ArrayIter> ait_;
	SmartPtr<MapIter> mit_;
	int_t index_;
};

class InstanceVariableGetter : public Any{
public:

	InstanceVariableGetter(int_t number, ClassInfo* info){
		value_.init_instance_variable_getter(number, info);
	}

public:

	int_t number() const{
		return value_.immediate_first_value();
	}

	ClassInfo* class_info() const{
		return (ClassInfo*)value_.immediate_second_vpvalue();
	}

	void on_rawcall(const VMachinePtr& vm) const;

public:

	operator const AnyPtr&() const{
		return ap(*this);
	}
};

class InstanceVariableSetter : public Any{
public:

	InstanceVariableSetter(int_t number, ClassInfo* info){
		value_.init_instance_variable_setter(number, info);
	}

public:

	int_t number() const{
		return value_.immediate_first_value();
	}

	ClassInfo* class_info() const{
		return (ClassInfo*)value_.immediate_second_vpvalue();
	}

	void on_rawcall(const VMachinePtr& vm) const;

public:

	operator const AnyPtr&() const{
		return ap(*this);
	}
};
/**
* \xbind lib::builtin
* \xinherit lib::builtin::Any
* \brief ���\�b�h�I�u�W�F�N�g
*/
class Method : public HaveParentRefCountingBase{
public:
	enum{ TYPE = TYPE_METHOD };

	Method(const FramePtr& outer, const CodePtr& code, FunInfo* info);

	const FramePtr& outer(){ return outer_; }

	const CodePtr& code(){ return code_; }

	int_t pc(){ return info_->pc; }

	const inst_t* source();

	const IDPtr& param_name_at(size_t i);

	int_t param_size();

	bool extendable_param();

	FunInfo* info(){ return info_; }

	void set_info(FunInfo* fc){ info_ = fc; }

	bool check_arg(const VMachinePtr& vm);

	const IDPtr& object_temporary_name();

public:
		
	void on_rawcall(const VMachinePtr& vm);
	
	void on_visit_members(Visitor& m){
		HaveParentRefCountingBase::on_visit_members(m);
		m & outer_ & code_;
	}

protected:
	BasePtr<Frame> outer_;
	BasePtr<Code> code_;
	FunInfo* info_;

	friend class VMachine;
};

/**
* \xbind lib::builtin
* \xinherit lib::builtin::Method
* \brief �֐��I�u�W�F�N�g
*/
class Fun : public Method{
public:
	enum{ TYPE = TYPE_FUN };

	Fun(const FramePtr& outer, const AnyPtr& athis, const CodePtr& code, FunInfo* info);

public:
	
	void on_rawcall(const VMachinePtr& vm);

	void on_visit_members(Visitor& m){
		Method::on_visit_members(m);
		m & this_;
	}

	const AnyPtr& self(){
		return this_;
	}

protected:

	AnyPtr this_;
};

/**
* \xbind lib::builtin
* \xinherit lib::builtin::Fun
* \brief �����_�I�u�W�F�N�g
*/
class Lambda : public Fun{
public:
	enum{ TYPE = TYPE_LAMBDA };

	Lambda(const FramePtr& outer, const AnyPtr& th, const CodePtr& code, FunInfo* info);

public:
	
	void on_rawcall(const VMachinePtr& vm);
};

/**
* \xbind lib::builtin
* \xinherit lib::builtin::Fun
* \brief �t�@�C�o�[�I�u�W�F�N�g
*/
class Fiber : public Fun{
public:
	enum{ TYPE = TYPE_FIBER };

	Fiber(const FramePtr& outer, const AnyPtr& th, const CodePtr& code, FunInfo* info);

	void on_finalize();
			
public:

	void block_next(const VMachinePtr& vm){
		call_helper(vm, true);
	}

	/**
	* \brief ���s��������~����
	*/
	void halt();

	void on_rawcall(const VMachinePtr& vm){
		call_helper(vm, false);
	}

	/**
	* \brief �t�@�C�o�[�I�u�W�F�N�g�����s�����ǂ���
	*/
	bool is_alive(){
		return alive_;
	}

	/**
	* \brief �t�@�C�o�[��������~���A������Ԃɖ߂�
	*/
	const FiberPtr& reset();

	void call_helper(const VMachinePtr& vm, bool add_succ_or_fail_result);

	void on_visit_members(Visitor& m){
		Fun::on_visit_members(m);
		m & vm_;
	}

private:
	VMachinePtr vm_;
	const inst_t* resume_pc_;
	bool alive_;
};

class BindedThis : public Base{
public:

	BindedThis(const AnyPtr& fun, const AnyPtr& athis)
		:fun_(fun), this_(athis){
	}

public:
	
	void on_rawcall(const VMachinePtr& vm);

	void on_visit_members(Visitor& m){
		Base::on_visit_members(m);
		m & fun_ & this_;
	}

protected:
	AnyPtr fun_;
	AnyPtr this_;
};

inline AnyPtr bind_this(const AnyPtr& method, const AnyPtr& athis){
	return xnew<BindedThis>(method, athis);
}

}


#endif // XTAL_FUN_H_INCLUDE_GUARD
