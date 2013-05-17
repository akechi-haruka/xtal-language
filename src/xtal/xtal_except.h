/** \file src/xtal/xtal_except.h
* \brief src/xtal/xtal_except.h
*/

#ifndef XTAL_EXCEPT_H_INCLUDE_GUARD
#define XTAL_EXCEPT_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/**
* \xbind lib::builtin
* \brief ��O
*/
class Exception : public Base{
public:

	Exception(const AnyPtr& message = empty_string);

	/**
	* \xbind
	* \brief ����������
	*/
	void initialize(const AnyPtr& message = empty_string);

	/**
	* \xbind
	* \brief �o�b�N�g���[�X��ǉ�����
	*/
	void append_backtrace(const AnyPtr& file, int_t line, const AnyPtr& function_name = empty_string);

	/**
	* \xbind
	* \brief �����񉻂���
	*/
	StringPtr to_s();

	/**
	* \xbind
	* \brief ���b�Z�[�W��Ԃ�
	*/
	const StringPtr& message(){
		return message_;
	}

	/**
	* \xbind
	* \brief �o�b�N�g���[�X�̏���v�f�Ƃ���Iterator��Ԃ�
	*/
	AnyPtr backtrace(){
		return backtrace_->each();
	}

	void on_visit_members(Visitor& m){
		Base::on_visit_members(m);
		m & message_ & backtrace_;
	}

private:
	StringPtr message_;
	ArrayPtr backtrace_;
};

class StandardError{};
class RuntimeError{};
class IOError{};
class EOSError{};
class LogicError{};
class CastError{};
class ArgumentError{};
class YieldError{};
class InstanceVariableError{};
class UnsupportedError{};
class RedefinedError{};
class AccessibilityError{};
class AssertionFailed{};
class CompileError{};

AnyPtr unsupported_error(const AnyPtr& target, const IDPtr& primary_key, const AnyPtr& secondary_key);
AnyPtr filelocal_unsupported_error(const CodePtr& code, const IDPtr& primary_key);

void set_unsupported_error(const AnyPtr& target, const IDPtr& primary_key, const AnyPtr& secondary_key, const VMachinePtr& vm = vmachine());
void set_runtime_error(const AnyPtr& arg, const VMachinePtr& vm = vmachine());
void set_argument_type_error(const AnyPtr& object, int_t no, const ClassPtr& required, const ClassPtr& type, const VMachinePtr& vm = vmachine());
void set_argument_num_error(const AnyPtr& funtion_name, int_t n, int_t min_count, int_t max_count, const VMachinePtr& vm = vmachine());

/**
* \brief ��O��ݒ肷��
* \hideinitializer
*/
#define XTAL_SET_EXCEPT(e) ::xtal::vmachine()->set_except(e)

/**
* \brief ��O�����o����Ă���Ȃ���o��
* \hideinitializer
*/
#define XTAL_CATCH_EXCEPT(e) if(const ::xtal::ExceptionPtr& e = (::xtal::unchecked_ptr_cast< ::xtal::Exception>(::xtal::vmachine()->catch_except())))

/**
* \brief ��O�����o����Ă��邩���ׂ�
* XTAL_CATCH_EXCEPT�ƈႢ�A��O�͐ݒ肳�ꂽ�܂܁B
* \hideinitializer
*/
#define XTAL_CHECK_EXCEPT(e) if(const ::xtal::ExceptionPtr& e = (::xtal::unchecked_ptr_cast< ::xtal::Exception>(::xtal::vmachine()->except())))

}

#endif // XTAL_EXCEPT_H_INCLUDE_GUARD
