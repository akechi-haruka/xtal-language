/** \file src/xtal/xtal_debug.h
* \brief src/xtal/xtal_debug.h
*/

#ifndef XTAL_DEBUG_H_INCLUDE_GUARD
#define XTAL_DEBUG_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/**
* \xbind lib::builtin
* \brief �f�o�b�O
*/
namespace debug{

class CallerInfo : public Base{
public:

	/**
	* \xbind
	* \brief �Ăяo���ꏊ�̍s����Ԃ�
	*/
	int_t lineno(){ return line_; }

	const MethodPtr& fun(){ return fun_; }

	/**
	* \xbind
	* \brief �Ăяo���ꏊ�̃t�@�C������Ԃ�
	*/
	StringPtr file_name();

	/**
	* \xbind
	* \brief �Ăяo���ꏊ�̊֐�����Ԃ�
	*/
	StringPtr fun_name();

	/**
	* \xbind
	* \brief �Ăяo���ꏊ�̕ϐ��t���[���I�u�W�F�N�g��Ԃ�
	*/
	const FramePtr& variables_frame(){ return variables_frame_; }

	void set_line(int_t v){ line_ = v; }
	void set_fun(const MethodPtr& v){ fun_ = v; }
	void set_variables_frame(const FramePtr& v){ variables_frame_ = v; }

	void on_visit_members(Visitor& m);

private:
	int_t line_;
	MethodPtr fun_;
	FramePtr variables_frame_;
};

typedef SmartPtr<CallerInfo> CallerInfoPtr;

/**
* \xbind lib::builtin::debug
* \xinherit lib::builtin::Any
* \brief �f�o�b�O�t�b�N�֐��œn�����I�u�W�F�N�g
*/
class HookInfo : public Base{
public:
	
	/**
	* \xbind
	* \brief �t�b�N�̎�ނ�Ԃ�
	*/
	int_t kind(){ return kind_; } 

	/**
	* \xbind
	* \brief �t�b�N���ꂽ�ꏊ�̍s����Ԃ�
	*/
	int_t lineno(){ return line_; }

	/**
	* \xbind
	* \brief �t�b�N���ꂽ�ꏊ��Code�I�u�W�F�N�g��Ԃ�
	*/
	const CodePtr& code(){ return code_; }

	/**
	* \xbind
	* \brief �t�b�N���ꂽ�ꏊ�̃t�@�C������Ԃ�
	*/
	const StringPtr& file_name(){ return file_name_; }

	/**
	* \xbind
	* \brief �t�b�N���ꂽ�ꏊ�̊֐�����Ԃ�
	*/
	const StringPtr& fun_name(){ return fun_name_; }

	/**
	* \xbind
	* \brief ��O�I�u�W�F�N�g��Ԃ�
	*/
	const AnyPtr& exception(){ return exception_; }

	/**
	* \xbind
	* \brief �t�b�N���ꂽ�ꏊ�̕ϐ��t���[���I�u�W�F�N�g��Ԃ�
	*/
	const FramePtr& variables_frame(){ return variables_frame_; }

	/**
	* \brief �Ăяo�����̏���Ԃ�
	*/
	CallerInfoPtr caller(uint_t n);

	int_t call_stack_size();

	void set_kind(int_t v){ kind_ = v; }
	void set_line(int_t v){ line_ = v; }
	void set_code(const CodePtr& v){ code_ = v; }
	void set_file_name(const StringPtr& v){ file_name_ = v; }
	void set_fun_name(const StringPtr& v){ fun_name_ = v; }
	void set_exception(const AnyPtr& e){ exception_ = e; }
	void set_variables_frame(const FramePtr& v){ variables_frame_ = v; }

	/**
	* \xbind
	* \brief �N���[�������
	* HookInfo�I�u�W�F�N�g�͎g���܂킳��邽�߁A����ێ��������ꍇ�A�N���[�������K�v������B
	*/
	SmartPtr<HookInfo> clone();

	void set_vm(const VMachinePtr& v);

	const VMachinePtr& vm();

	void on_visit_members(Visitor& m);

private:

	int_t kind_;
	int_t line_;
	CodePtr code_;
	StringPtr file_name_;
	StringPtr fun_name_;
	AnyPtr exception_;
	FramePtr variables_frame_;
	VMachine* vm_;

	friend class VMachine;
};

typedef SmartPtr<HookInfo> HookInfoPtr;

class Debug{};

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�@�\��L���ɂ���
* �f�o�b�O�@�\�̓f�t�H���g�ł͖����ɂȂ��Ă���B
*/
void enable();

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�@�\�𖳌��ɂ���
*/
void disable();

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�@�\�������I�ɗL���ɂ���
* �f�o�b�O�@�\�̓f�t�H���g�ł͖����ɂȂ��Ă���B
*/
void enable_force(uint_t count);

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�@�\�������I�ɖ����ɂ���
*/
uint_t disable_force();

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�@�\���L�����ǂ���
*/
bool is_enabled();

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�R���p�C�����L�����ǂ���
*/
bool is_debug_compile_enabled();

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�R���p�C����L���ɂ���
*/
void enable_debug_compile();

/**
* \xbind lib::builtin::debug
* \brief �f�o�b�O�R���p�C���𖳌��ɂ���
*/
void disable_debug_compile();

uint_t hook_setting_bit();

uint_t* hook_setting_bit_ptr();

void set_hook(int_t hooktype, const AnyPtr& hook);

const AnyPtr& hook(int_t hooktype);

/**
* \xbind lib::builtin::debug
* \brief �u���[�N�|�C���g�ɒB���邽�тɌĂяo�����t�b�N�֐���o�^����
* \param hook �o�^����t�b�N�֐�
*/
void set_breakpoint_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief �֐��Ăяo�������x�ɌĂяo�����t�b�N�֐���o�^����
* \param hook �o�^����t�b�N�֐�
*/
void set_call_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief �֐�����return�����x�ɌĂяo�����t�b�N�֐���o�^����
* \param hook �o�^����t�b�N�֐�
*/
void set_return_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief ��O��throw�����x�ɌĂяo�����t�b�N�֐���o�^����
* \param hook �o�^����t�b�N�֐�
*/
void set_throw_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief �A�T�[�g�����s�����x�ɌĂяo�����t�b�N�֐���o�^����
* \param hook �o�^����t�b�N�֐�
*/
void set_assert_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin::debug
* \brief set_breakpoint_hook�֐��œo�^�����֐����擾����
*/
const AnyPtr& breakpoint_hook();

/**
* \xbind lib::builtin::debug
* \brief set_call_hook�֐��œo�^�����֐����擾����
*/
const AnyPtr& call_hook();

/**
* \xbind lib::builtin::debug
* \brief set_return_hook�֐��œo�^�����֐����擾����
*/
const AnyPtr& return_hook();

/**
* \xbind lib::builtin::debug
* \brief set_throw_hook�֐��œo�^�����֐����擾����
*/
const AnyPtr& throw_hook();

/**
* \xbind lib::builtin::debug
* \brief set_assert_hook�֐��œo�^�����֐����擾����
*/
const AnyPtr& assert_hook();

/**
* \xbind lib::builtin::debug
* \brief �Ē�`���L�����ǂ���
*/
bool is_redefine_enabled();

/**
* \xbind lib::builtin::debug
* \brief �Ē�`��L���ɂ���
*/
void enable_redefine();

/**
* \xbind lib::builtin::debug
* \brief �Ē�`�𖳌��ɂ���
*/
void disable_redefine();

//////////////////////////////

enum{
	RUN,
	STEP_OVER,
	STEP_INTO,
	STEP_OUT,
	REDO
};

void call_breakpoint_hook(int_t kind, HookInfoPtr info);

////////////////////////////////////////////////////////////////////////////////////

/**
* \brief �f�o�b�K�̎�M��
*/
class CommandReceiver{
public:

	~CommandReceiver();

	/**
	* \brief �f�o�b�K���X�^�[�g����
	* \param stream �f�o�b�K�ƒʐM���邽�߂̃X�g���[��
	*/
	bool start(const StreamPtr& stream);

	/**
	* �f�o�b�K���X�V����
	* 
	*/
	void update();

private:
	ArrayPtr recv_command();

	CodePtr require_source_hook(const StringPtr& name);

	void exec_command(const ArrayPtr& cmd);

	ArrayPtr make_debug_object(const AnyPtr& v, int depth = 3);

	ArrayPtr make_call_stack_info(debug::HookInfoPtr info);

	MapPtr make_eval_expr_info(debug::HookInfoPtr info, int level);

	void send_break(debug::HookInfoPtr info, int level);

	int breakpointhook(debug::HookInfoPtr info);
	int breakpointhook2(debug::HookInfoPtr info);

	void check();

private:
	StreamPtr stream_;
	MapPtr eval_exprs_;
	MapPtr code_map_;
	AnyPtr require_hook_;
	bool pause_;
};

typedef SmartPtr<CommandReceiver> CommandReceiverPtr;

////////////////////////////////////////////////////////////////////////////////////

/**
* \brief �f�o�b�K�̑��M��
*/
class CommandSender{
public:

	CommandSender();

	/**
	* \brief �f�o�b�K���X�^�[�g����
	* \param stream �f�o�b�K�ƒʐM���邽�߂̃X�g���[��
	*/
	void start(const StreamPtr& stream);

	/**
	* �f�o�b�K���X�V����
	* 
	*/
	void update();

public:
	// �]�����Ɋւ���ݒ�Ǝ擾

	void add_eval_expr(const StringPtr& expr);

	void remove_eval_expr(const StringPtr& expr);

	ArrayPtr eval_expr_result(const StringPtr& expr);

public:
	// �R�[���X�^�b�N�Ɋւ���ݒ�Ǝ擾

	int call_stack_size();

	StringPtr call_stack_fun_name(int n);
	StringPtr call_stack_fun_name();

	StringPtr call_stack_file_name(int n);
	StringPtr call_stack_file_name();

	int call_stack_lineno(int n);
	int call_stack_lineno();

	void move_call_stack(int n);

public:
	// �v�����ꂽ�X�N���v�g�t�@�C���Ɋւ���ݒ�Ǝ擾

	StringPtr required_file();

	void required_source(const CodePtr& code);

	int level(){ return level_;	}

public:
	// �u���[�N�|�C���g�Ɋւ���ݒ�Ǝ擾

	void add_breakpoint(const StringPtr& path, int n, const StringPtr& cond);
	void remove_breakpoint(const StringPtr& path, int n);

public:
	void pause();

public:
	void update_source(const CodePtr& code){}

public:
	void nostep();
	void start();

	void run();
	void step_over();
	void step_into();
	void step_out();
	void redo();

protected:

	// �u���[�N�����ۂ̃V�O�i��
	virtual void on_breaked(){}

	// �u���[�N�����ۂ̃V�O�i��
	virtual void on_required(){}

	// �R���p�C���G���[���N�������ۂ̃V�O�i��
	virtual void on_compile_error(){}

	// �L���b�`����Ȃ���O���N�������ۂ̃V�O�i��
	virtual void on_uncatched_exception(){}

protected:
	void send_command(const IDPtr& id);
	void erase_breakpoint(const StringPtr& path, int n);
	void add_breakpoint_inner(const StringPtr& path, int n, const StringPtr& cond);
	void remove_breakpoint_inner(const StringPtr& path, int n);
	void add_eval_expr_inner(const StringPtr& expr);
	void remove_eval_expr_inner(const StringPtr& expr);

private:
	StreamPtr stream_;

	struct CallInfo{
		StringPtr fun_name;
		StringPtr file_name;
		int lineno;
	};

	TArray<CallInfo> call_stack_;

	MapPtr exprs_;
	ArrayPtr breakpoints_;
	StringPtr required_file_;
	int level_;

	IDPtr prev_command_;
};

typedef SmartPtr<CommandSender> CommandSenderPtr;

}

}

#endif // XTAL_DEBUG_H_INCLUDE_GUARD
