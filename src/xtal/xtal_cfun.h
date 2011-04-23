/** \file src/xtal/xtal_cfun.h
* \brief src/xtal/xtal_cfun.h
*/

#ifndef XTAL_CFUN_H_INCLUDE_GUARD
#define XTAL_CFUN_H_INCLUDE_GUARD

#pragma once

namespace xtal{

class NativeMethod;	

////////////////////////////////////////////

struct FunctorParam;

typedef void (*native_functon_t)(FunctorParam&);

struct param_types_holder_n{
	native_functon_t fun; // �֐�
	CppClassSymbolData* const * param_types; // this�ƈ����̌^��\���N���X�V���{���ւ̃|�C���^
	u8 size; // �֐��̃T�C�Y
	u8 param_n; // �����̐�
	u8 extendable; // �ϒ����ǂ���
	u8 vm; // ���z�}�V���𓧉߂��邩�ǂ���
};

template<class TFun>
struct fun_param_holder{
	typedef typename TFun::fun_param_holder fph;
	typedef typename TFun::fun_type fun_type;

	static const param_types_holder_n value;
};

template<class TFun>
const param_types_holder_n fun_param_holder<TFun>::value = {
	&TFun::call,
	&fph::values[0],
	sizeof(fun_type),
	TFun::arity,
	TFun::extendable,
	TFun::vm,
};

//////////////////////////////////////////////

template<class C, class T>
struct getter_functor;

template<class C, class T>
struct setter_functor;

template<class TFun>
struct functor{};

template<class TFun, TFun fun>
struct static_functor{};

///////////////////////////////////////////////

template<int Arity, class TFun, int Method, class R>
struct nfun_base{};

template<class TFun, int Method>
struct nfun
	: public nfun_base<TFun::arity, TFun, Method, typename TFun::result_type>{};

template<class TFun>
struct dfun
	: public nfun<TFun, 0>{};

template<class TFun>
struct dmemfun
	: public nfun<TFun, 1>{};

template<class C, class T>
struct getter_functor;

template<class C, class T>
struct setter_functor;


template<class T, 
class A0=void, class A1=void, class A2=void, class A3=void, class A4=void, 
class A5=void, class A6=void, class A7=void, class A8=void, class A9=void,
class A10=void, class A11=void, class A12=void, class A13=void, class A14=void>
struct ctor_functor{};

//////////////////////////////////////////////////////////////

struct NamedParam;

class StatelessNativeMethod : public Any{
public:
	StatelessNativeMethod(const param_types_holder_n& pth);
public:
	void on_rawcall(const VMachinePtr& vm) const;
};

class NativeMethod : public RefCountingBase{
public:
	enum{ TYPE = TYPE_NATIVE_METHOD };

	NativeMethod(const param_types_holder_n& pth, const void* val = 0);
	
	~NativeMethod();

public:

	const NativeFunPtr& param(int_t i, const IDPtr& key, const AnyPtr& value);

	const NativeFunPtr& add_param(const IDPtr& key, const AnyPtr& value);

public:

	const NamedParam* params();

	void on_visit_members(Visitor& m);

	void on_rawcall(const VMachinePtr& vm);

protected:
	const param_types_holder_n* pth_;
	void* data_;

	u8 min_param_count_;
	u8 max_param_count_;
	u8 val_size_;
};

class NativeFun : public NativeMethod{
public:
	enum{ TYPE = TYPE_NATIVE_FUN };

	NativeFun(const param_types_holder_n& pth, const void* val, const AnyPtr& this_);

public:

	void on_visit_members(Visitor& m);

	void on_rawcall(const VMachinePtr& vm);

private:
	AnyPtr this_;
};

NativeFunPtr new_native_fun(const param_types_holder_n& pth, const void* val);
NativeFunPtr new_native_fun(const param_types_holder_n& pth, const void* val, const AnyPtr& this_);

//////////////////////////////////////////////////////////////

/*
* \brief C++�̃R���X�g���N�^��Xtal����Ăяo����I�u�W�F�N�g�ɕϊ����邽�߂̋[���֐�
*
*/
template<class T, 
class A0=void, class A1=void, class A2=void, class A3=void, class A4=void, 
class A5=void, class A6=void, class A7=void, class A8=void, class A9=void,
class A10=void, class A11=void, class A12=void, class A13=void, class A14=void>
struct ctor : public NativeFunPtr{
	typedef dfun<ctor_functor<T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14> > fun_t;
	ctor(){
		NativeFunPtr::operator =(new_native_fun(fun_param_holder<fun_t>::value, 0));
	}
};
	
/**
* @brief 2�d�f�B�X�p�b�`���\�b�h
*/
class DoubleDispatchMethod : public Base{
public:

	DoubleDispatchMethod(const IDPtr& primary_key);

	void on_rawcall(const VMachinePtr& vm);

private:
	IDPtr primary_key_;
};

/**
* @brief 2�d�f�B�X�p�b�`�֐�
*/
class DoubleDispatchFun : public Base{
public:

	DoubleDispatchFun(const ClassPtr& klass, const IDPtr& primary_key);

	void on_rawcall(const VMachinePtr& vm);

	void on_visit_members(Visitor& m);

private:
	AnyPtr klass_;
	IDPtr primary_key_;
};

/// \name �l�C�e�B�u�֐���Xtal�ŌĂяo����I�u�W�F�N�g�ɕϊ����邽�߂̊֐��Q
//@{

/**
* \brief C++�̊֐���Xtal����Ăяo����I�u�W�F�N�g�ɕϊ����邽�߂̊֐�
*
*/
template<class Fun>
inline NativeFunPtr fun(const Fun& f){
	return new_native_fun(fun_param_holder<dfun<Fun> >::value, &f);
}

/**
* \brief C++�̃����o�֐���Xtal����Ăяo����I�u�W�F�N�g�ɕϊ����邽�߂̊֐�
*
* ���ʂ̊֐������\�b�h�Ƃ��ĕϊ��������ꍇ�A�����������̌^�ɂ��邱�ƁB
*/
template<class Fun>
inline NativeFunPtr method(const Fun& f){
	return new_native_fun(fun_param_holder<dmemfun<Fun> >::value, &f);
}

/**
* \brief �����o�ϐ��ւ̃|�C���^����Q�b�^�[�֐��𐶐�����
*
*/
template<class T, class C>
inline NativeFunPtr getter(T C::* f){
	return new_native_fun(fun_param_holder<getter_functor<C, T> >::value, &f);
}
	
/**
* \brief �����o�ϐ��ւ̃|�C���^����Z�b�^�[�֐��𐶐�����
*
*/
template<class T, class C>
inline NativeFunPtr setter(T C::* f){
	return new_native_fun(fun_param_holder<getter_functor<C, T> >::value(), &f);
}

/**
* \brief 2�d�f�B�X�p�b�`���\�b�h�I�u�W�F�N�g�𐶐�����
*
*/
DoubleDispatchMethodPtr double_dispatch_method(const IDPtr& primary_key);

/**
* \brief 2�d�f�B�X�p�b�`���\�b�h�I�u�W�F�N�g�𐶐�����
*
*/
DoubleDispatchFunPtr double_dispatch_fun(const ClassPtr& klass, const IDPtr& primary_key);

//@}

}

#endif // XTAL_CFUN_H_INCLUDE_GUARD
