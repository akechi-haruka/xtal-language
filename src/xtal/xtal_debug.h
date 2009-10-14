/** \file src/xtal/xtal_debug.h
* \brief src/xtal/xtal_debug.h
*/

#ifndef XTAL_DEBUG_H_INCLUDE_GUARD
#define XTAL_DEBUG_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/**
* \xbind lib::builtin
* \brief デバッグ
*/
namespace debug{

class CallerInfo : public Base{
public:

	/**
	* \xbind
	* \brief 呼び出し場所の行数を返す
	*/
	int_t line(){ return line_; }


	const FunPtr& fun(){ return fun_; }

	/**
	* \xbind
	* \brief 呼び出し場所のファイル名を返す
	*/
	StringPtr file_name(){ return fun_ ? fun_->code()->source_file_name() : "C++ Function"; }

	/**
	* \xbind
	* \brief 呼び出し場所の関数名を返す
	*/
	StringPtr fun_name(){ return fun_ ? fun_->object_name() : ""; }

	/**
	* \xbind
	* \brief 呼び出し場所の変数フレームオブジェクトを返す
	*/
	const FramePtr& variables_frame(){ return variables_frame_; }

	void set_line(int_t v){ line_ = v; }
	void set_fun(const FunPtr& v){ fun_ = v; }
	void set_variables_frame(const FramePtr& v){ variables_frame_ = v; }

	virtual void visit_members(Visitor& m);

private:
	int_t line_;
	FunPtr fun_;
	FramePtr variables_frame_;
};

typedef SmartPtr<CallerInfo> CallerInfoPtr;

/**
* \xbind lib::builtin::debug
* \xinherit lib::builtin::Any
* \brief デバッグフック関数で渡されるオブジェクト
*/
class HookInfo : public Base{
public:
	
	/**
	* \xbind
	* \brief フックの種類を返す
	*/
	int_t kind(){ return kind_; } 

	/**
	* \xbind
	* \brief フックされた場所の行数を返す
	*/
	int_t line(){ return line_; }

	/**
	* \xbind
	* \brief フックされた場所のファイル名を返す
	*/
	const StringPtr& file_name(){ return file_name_; }

	/**
	* \xbind
	* \brief フックされた場所の関数名を返す
	*/
	const StringPtr& fun_name(){ return fun_name_; }

	/**
	* \xbind
	* \brief assertionのメッセージを返す
	*/
	const StringPtr& assertion_message(){ return assertion_message_; }

	/**
	* \xbind
	* \brief 例外オブジェクトを返す
	*/
	const AnyPtr& exception(){ return exception_; }

	/**
	* \xbind
	* \brief フックされた場所の変数フレームオブジェクトを返す
	*/
	const FramePtr& variables_frame(){ return variables_frame_; }

	/**
	* \brief 呼び出し元の情報を返す
	*/
	CallerInfoPtr caller(uint_t n);

	int_t call_stack_size();

	void set_kind(int_t v){ kind_ = v; }
	void set_line(int_t v){ line_ = v; }
	void set_file_name(const StringPtr& v){ file_name_ = v; }
	void set_fun_name(const StringPtr& v){ fun_name_ = v; }
	void set_assertion_message(const StringPtr& v){ assertion_message_ = v; }
	void set_exception(const AnyPtr& e){ exception_ = e; }
	void set_variables_frame(const FramePtr& v){ variables_frame_ = v; }

	/**
	* \xbind
	* \brief クローンを作る
	* DebugInfoオブジェクトは使いまわされるため、情報を保持したい場合、クローンを作る必要がある。
	*/
	SmartPtr<HookInfo> clone(){
		return xnew<HookInfo>(*this);
	}

	void set_vm(const VMachinePtr& v){ vm_ = v; }

	const VMachinePtr& vm(){ return (VMachinePtr&)ap(vm_); }

private:

	virtual void visit_members(Visitor& m);

	int_t kind_;
	int_t line_;
	StringPtr file_name_;
	StringPtr fun_name_;
	StringPtr assertion_message_;
	AnyPtr exception_;
	FramePtr variables_frame_;
	Any vm_;

	friend class VMachine;
};

typedef SmartPtr<HookInfo> HookInfoPtr;

class Debug;

/**
* \xbind lib::builtin::debug
* \brief デバッグ機能を有効にする
* デバッグ機能はデフォルトでは無効になっている。
*/
void enable();

/**
* \xbind lib::builtin::debug
* \brief デバッグ機能を無効にする
*/
void disable();

/**
* \xbind lib::builtin::debug
* \brief デバッグ機能を強制的に有効にする
* デバッグ機能はデフォルトでは無効になっている。
*/
void enable_force(int_t count);

/**
* \xbind lib::builtin::debug
* \brief デバッグ機能を強制的に無効にする
*/
int_t disable_force();

/**
* \xbind lib::builtin::debug
* \brief デバッグ機能が有効かどうか
*/
bool is_enabled();

uint_t hook_setting_bit();

void set_hook(int_t hooktype, const AnyPtr& hook);

const AnyPtr& hook(int_t hooktype);

/**
* \xbind lib::builtin::debug
* \brief 行が変わるたびに呼び出されるフック関数を登録する
* \param hook 登録するフック関数
*/
void set_line_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief 関数呼び出しされる度に呼び出されるフック関数を登録する
* \param hook 登録するフック関数
*/
void set_call_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief 関数からreturnされる度に呼び出されるフック関数を登録する
* \param hook 登録するフック関数
*/
void set_return_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief 例外かthrowされる度に呼び出されるフック関数を登録する
* \param hook 登録するフック関数
*/
void set_throw_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief アサートが失敗される度に呼び出されるフック関数を登録する
* \param hook 登録するフック関数
*/
void set_assert_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief set_line_hook関数で登録した関数を取得する
*/
const AnyPtr& line_hook();

/**
* \xbind lib::builtin::debug
* \brief set_call_hook関数で登録した関数を取得する
*/
const AnyPtr& call_hook();

/**
* \xbind lib::builtin::debug
* \brief set_return_hook関数で登録した関数を取得する
*/
const AnyPtr& return_hook();

/**
* \xbind lib::builtin::debug
* \brief set_throw_hook関数で登録した関数を取得する
*/
const AnyPtr& throw_hook();

/**
* \xbind lib::builtin::debug
* \brief set_assert_hook関数で登録した関数を取得する
*/
const AnyPtr& assert_hook();


//////////////////////////////

void add_breakpoint(const AnyPtr& sorce, int_t line);
void remove_breakpoint(const AnyPtr& sorce, int_t line);
void clear_breakpoint();

enum{
	GO,
	STEP,
	STEP_IN,
	STEP_OUT
};

void set_breakpoint_hook(const AnyPtr& fun);

}

}

#endif // XTAL_DEBUG_H_INCLUDE_GUARD
