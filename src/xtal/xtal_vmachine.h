/** \file src/xtal/xtal_vmachine.h
* \brief src/xtal/xtal_vmachine.h
*/

#ifndef XTAL_VMACHINE_H_INCLUDE_GUARD
#define XTAL_VMACHINE_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/**
* \brief �֐��Ăяo���ŁA���O�t�������Ƃ��ēn�����߂̃N���X
*
*/
struct Named{
	const IDPtr& name;
	const AnyPtr& value;

	/**
	* \brief ���O�ƒl���w�肵�č\�z����B
	*/
	Named(const IDPtr& name, const AnyPtr& value)
		:name(name), value(value){}

	/**
	* \brief ��ȏ�ԂŐ�������
	*/
	Named()
		:name(nul<ID>()), value(undefined){}

private:
	void operator=(const Named&);
};

struct NamedParam{
	NamedParam()
		:value(undefined){}

	IDPtr name;
	AnyPtr value;
};

template<>
struct FastStackDefaultValue<AnyPtr>{
	static const AnyPtr& get(){ return null; }
};

#if defined(XTAL_NO_THREAD) || !defined(XTAL_USE_THREAD_MODEL2)

#define XTAL_VM_LOCK
#define XTAL_VM_UNLOCK
#define XTAL_VM_INC(v) XTAL_detail_inc_ref_count(v)
#define XTAL_VM_DEC(v) XTAL_detail_dec_ref_count(v)

#else

#define XTAL_detail_inc_ref_count_locked(v) (void)(XTAL_detail_is_rcpvalue(v) && (XTAL_detail_rcpvalue(v)->atomic_inc_ref_count(), 1))
#define XTAL_detail_dec_ref_count_locked(v) (void)(XTAL_detail_is_rcpvalue(v) && (XTAL_LOCK_DIRECT, XTAL_detail_rcpvalue(v)->object_destroy(), XTAL_UNLOCK_DIRECT, 1))

#define XTAL_VM_LOCK XTAL_LOCK
#define XTAL_VM_UNLOCK XTAL_UNLOCK
#define XTAL_VM_INC(v) (void)(XTAL_detail_is_rcpvalue(v) && (XTAL_detail_rcpvalue(v)->atomic_inc_ref_count(), 1))
#define XTAL_VM_DEC(v) (void)(XTAL_detail_is_rcpvalue(v) && !XTAL_detail_rcpvalue(v)->atomic_dec_ref_count() && (XTAL_LOCK_DIRECT, XTAL_detail_rcpvalue(v)->object_destroy(), XTAL_UNLOCK_DIRECT, 1))

#endif

#define XTAL_VM_variables_top() (variables_top_ - variables_.data())
#define XTAL_VM_set_variables_top(top) (variables_top_ = variables_.data() + top)
#define XTAL_VM_local_variable(pos) (*(variables_top_ + pos))
#define XTAL_VM_set_local_variable(pos, value) (XTAL_VM_INC(value), XTAL_VM_DEC(XTAL_VM_local_variable(pos)), XTAL_detail_copy(XTAL_VM_local_variable(pos), value))

#define XTAL_VM_ff() (**fun_frames_.current_)
#define XTAL_VM_prev_ff() (**(fun_frames_.current_-1))
#define XTAL_VM_identifier(n) (XTAL_VM_ff().identifier_[n])

// XTAL���z�}�V��
class VMachine : public Base{
public:

	VMachine();

	~VMachine();

public:

	// �֐��Ăяo�������g�����߂̊֐��Q

	/**
	* \brief ������1�ςށB
	*
	*/
	void push_arg(const AnyPtr& value);
	
	/**
	* \brief ���O�t��������1�ςށB
	*
	*/
	void push_arg(const IDPtr& name, const AnyPtr& value);
	
	/**
	* \brief ���O�t��������1�ςށB
	*
	*/
	void push_arg(const Named& p){ push_arg(p.name, p.value); }

	/**
	* \brief ������z��̗v�f���ςށB
	*
	*/
	void push_ordered_args(const ArrayPtr& p);

	/**
	* \brief ���O��������A�z�z��̗v�f���ςށB
	*
	*/
	void push_named_args(const MapPtr& p);

	/**
	* \brief pos�Ԗڂ̖߂�l�𓾂�B
	*
	*/
	const AnyPtr& result(int_t pos = 0);

	/**
	* \brief �Ăяo���̌�n��������B
	*
	*/
	void cleanup_call();	

	/**
	* \brief pos�Ԗڂ̖߂�l��Ԃ��A�Ăяo���̌�n��������B
	*
	*/
	const AnyPtr& result_and_cleanup_call(int_t pos = 0);
		
	/**
	* \brief this�������ւ���B
	*
	*/	
	void set_arg_this(const AnyPtr& self){ 
		XTAL_VM_ff().self = self;
	}

	void insert_arg(int_t index, const AnyPtr& value);

// 
	/// \brief �֐����Ăяo���p�ӂ�����
	void setup_call(int_t need_result_count = 1);

public:
	template<class T>
	void push_arg(const T* p){
		push_arg(AnyPtr(p));
	}

	void push_arg(const char_t* s);
	void push_arg(const LongLivedString& s);

	void push_arg(char v);
	void push_arg(signed char v);
	void push_arg(unsigned char v);
	void push_arg(short v);
	void push_arg(unsigned short v);
	void push_arg(int v);
	void push_arg(unsigned int v);
	void push_arg(long v);
	void push_arg(unsigned long v);
	void push_arg(long long v);
	void push_arg(unsigned long long v);
	void push_arg(float v);
	void push_arg(double v);
	void push_arg(long double v);
	void push_arg(bool v);

	void push_arg(const ArgumentsPtr& args);

public:

	// �֐��Ăяo���ꑤ���g�����߂̊֐��Q

	/**
	* \brief pos�Ԗڂ̈����𓾂�B
	*
	*/
	const AnyPtr& arg(int_t pos);

	/**
	* \brief name�ɑΉ���������𓾂�B
	*
	*/
	const AnyPtr& arg(const IDPtr& name){
		return arg_default(name, undefined);
	}

	/**
	* \brief pos�Ԗڂ̈����𓾂�B����pos�Ԗڂ̈������Ȃ����name�ɑΉ���������𓾂�B
	*
	*/
	const AnyPtr& arg(int_t pos, const IDPtr& name);
	
	/**
	* \brief pos�Ԗڂ̈����𓾂�B
	*
	* ����pos�Ԗڂ̈������������def�̒l��Ԃ��B
	*/
	const AnyPtr& arg_default(int_t pos, const AnyPtr& def);

	/**
	* \brief name�ɑΉ���������𓾂�B
	*
	* ����name�ɑΉ�����������������def�̒l��Ԃ��B
	*/
	const AnyPtr& arg_default(const IDPtr& name, const AnyPtr& def);

	/**
	* \brief pos�Ԗڂ̈����𓾂�B����pos�Ԗڂ̈������Ȃ����name�ɑΉ���������𓾂�B
	*
	* ����name�ɑΉ�����������������def�̒l��Ԃ��B
	*/
	const AnyPtr& arg_default(int_t pos, const IDPtr& name, const AnyPtr& def);

	/**
	* \brief pos�Ԗڂ̖��O�w������̖��O���擾�B
	*
	* \param pos 0����named_arg_count()-1�܂�
	*/
	const IDPtr& arg_name(int_t pos){
		return unchecked_ptr_cast<ID>(XTAL_VM_local_variable(ordered_arg_count()+pos*2));
	}

	/**
	* \brief this���擾�B
	*
	*/
	const AnyPtr& arg_this(){ 
		return XTAL_VM_ff().self; 
	}

	/**
	* \brief �ʒu�w������̐����擾�B
	*
	*/
	int_t ordered_arg_count(){ 
		return XTAL_VM_ff().ordered_arg_count; 
	}
	
	/**
	* \brief ���O�w������̐����擾
	*
	*/
	int_t named_arg_count(){ 
		return XTAL_VM_ff().named_arg_count; 
	}
	
	/**
	* \brief �Ăяo�������K�v�Ƃ��Ă���߂�l�̐��B
	*
	*/
	int_t need_result_count(){ 
		return XTAL_VM_ff().need_result_count; 
	}
	
	/**
	* \brief �Ăяo�������߂�l��K�v�Ƃ��Ă��邩�ǂ����B
	*
	*/
	bool need_result(){ 
		return XTAL_VM_ff().need_result_count!=0; 
	}
	
	/*
	* Arguments�I�u�W�F�N�g�𐶐�����B
	* return_result()���Ă񂾌�͐���Ȓl�͓����Ȃ��B
	*/
	ArgumentsPtr make_arguments(int_t lower = 0);
	bool has_arguments();
	
	/**
	* \brief �Ăяo�����Ɉ����̐������̖߂�l��Ԃ��B
	*
	* need_result_count�̕������������ꍇ�͂��̕�null���Ԃ����B
	* need_result_count�̕����������Ȃ��ꍇ�͕Ԃ����l�͎̂Ă���B
	*/
	void return_result();
	void return_result(const AnyPtr& value1);
	void return_result(const AnyPtr& value1, const AnyPtr& value2);
	void return_result(const AnyPtr& value1, const AnyPtr& value2, const AnyPtr& value3);
	void return_result(const AnyPtr& value1, const AnyPtr& value2, const AnyPtr& value3, const AnyPtr& value4);

	/**
	* \brief ���l��Ԃ��B
	*/
	void return_result_mv(const ValuesPtr& values);

	/**
	* \brief return_result��carry_over�����ɌĂяo�����Ȃ�true���A�����łȂ��Ȃ�false��Ԃ��B
	*
	*/
	int_t is_executed(){
		return XTAL_VM_ff().is_executed; 
	}
	
public:

	void adjust_args(const NamedParam* params, int_t num);

	void adjust_args(Method* params);

	const AnyPtr& arg_unchecked(int_t pos){
		return XTAL_VM_local_variable(pos);
	}

	template<class T>
	void return_result(const T* p){
		return_result(AnyPtr(p));
	}

	void return_result(const char_t* s);
	void return_result(const LongLivedString& s);
	void return_result(const IDPtr& s);
	void return_result(char v);
	void return_result(signed char v);
	void return_result(unsigned char v);
	void return_result(short v);
	void return_result(unsigned short v);
	void return_result(int v);
	void return_result(unsigned int v);
	void return_result(long v);
	void return_result(unsigned long v);
	void return_result(long long v);
	void return_result(unsigned long long v);
	void return_result(float v);
	void return_result(double v);
	void return_result(long double v);
	void return_result(bool v);

public:

	FramePtr current_context(){
		return make_outer_outer();
	}

	FramePtr current_context2(){
		return make_outer_outer(0, 1);
	}

public:

	const AnyPtr& catch_except();

	const AnyPtr& except(){
		return except_[0];
	}

	void set_except(const AnyPtr& e);

	void set_except_x(const AnyPtr& e);

	void set_except_0(const AnyPtr& e);

	void execute_inner(const inst_t* start, int_t eval_n = 0);

	struct ExceptFrame;
	void execute_inner2(const inst_t* start, int_t eval_n, ExceptFrame& cur);

	void execute(Method* fun, const inst_t* start_pc);

	void carry_over(Method* fun, bool adjust_arguments = false);

	const inst_t* resume_pc(){
		return resume_pc_;
	}

	void present_for_vm(Fiber* fun, VMachine* vm, bool add_succ_or_fail_result);

	const inst_t* start_fiber(Fiber* fun, VMachine* vm, bool add_succ_or_fail_result);

	const inst_t* resume_fiber(Fiber* fun, const inst_t* pc, VMachine* vm, bool add_succ_or_fail_result);
	
	void exit_fiber();
	
	void reset();

public:

	void adjust_values2(int_t stack_base, int_t n, int_t need_result_count);
	void adjust_values3(AnyPtr* values, int_t src_count, int_t dest_count);

public:

	void move_variables(VMachine* src, int_t size){
		for(int_t i=0; i<size; ++i){
			set_local_variable(i, src->local_variable(i));
		}
	}

	void replace_result(const AnyPtr& result);

public: // eval�n
	debug::CallerInfoPtr caller(uint_t n);
	int_t call_stack_size();

	AnyPtr eval(const CodePtr& code, uint_t n);

	AnyPtr eval_local_variable(const IDPtr& var, uint_t call_n);
	bool eval_set_local_variable(const IDPtr& var, const AnyPtr& value, uint_t call_n);

	AnyPtr eval_instance_variable(const AnyPtr& self, const IDPtr& key);
	bool eval_set_instance_variable(const AnyPtr& self, const IDPtr& key, const AnyPtr& value);

public:

	struct FunFrame{

		// pop_ff�����Ƃ��͂���pc������s����
		const inst_t* poped_pc;

		// call�����Ƃ��͂���pc������s����
		const inst_t* next_pc;

		// �֐����Ă΂ꂽ�Ƃ��̏��Ԏw������̐�
		int_t ordered_arg_count;
		
		// �֐����Ă΂ꂽ�Ƃ��̖��O�w������̐�
		int_t named_arg_count;

		// �֐��Ăяo�������K�v�Ƃ���߂�l�̐�
		int_t need_result_count;

		// yield���\���t���O�B���̃t���O�͌Ăяo�����ׂ��œ`�d����B
		int_t yieldable;

		// ���̊֐����g���Ă���X�R�[�v�̉���
		uint_t scope_lower;

		// ���s�����t���O
		int_t is_executed;

		// �߂�l��Ԃ��ʒu
		int_t result;

		// ���ꂪ�l�܂��O�̃X�^�b�N�y�[�X
		uint_t prev_stack_base;

		// �Ăяo���ꂽ�֐��I�u�W�F�N�g
		BasePtr<Method> fun; 

		const IDPtr* identifiers;
		const AnyPtr* values;
		Code* code;

		// �֐��̊O���̃t���[���I�u�W�F�N�g
		BasePtr<Frame> outer;

		// �֐����Ă΂ꂽ�Ƃ���this�I�u�W�F�N�g
		AnyPtr self;
	};

	friend void visit_members(Visitor& m, FunFrame& v);
	
	// ��O���������邽�߂̃t���[��
	struct ExceptFrame{
		ExceptInfo* info;
		uint_t stack_size;
		uint_t fun_frame_size;
		uint_t scope_size;
		uint_t variables_top;
	};

private:

	void push_args(const ArgumentsPtr& args, int_t stack_base, int_t ordered_arg_count, int_t named_arg_count);

	struct CallState{
		Any cls;
		Any target;
		Any primary;
		Any secondary;
		Any self;
		Any member;

		CallState()
			:cls(null), target(null), primary(null), secondary(null), self(null), member(null){} 

		const inst_t* pc;
		const inst_t* next_pc;
		int_t result;
		int_t need_result_count;
		int_t stack_base;
		int_t ordered;
		int_t named;
		int_t flags;

		void set(const inst_t* pc, const inst_t* next_pc,
			int_t result, int_t need_result_count, 
			int_t stack_base, int_t ordered, int_t named, 
			int_t flags){

			this->pc = pc;
			this->next_pc = next_pc; 
			this->result = result; 
			this->need_result_count = need_result_count;
			this->stack_base = stack_base;
			this->ordered = ordered;
			this->named = named;
			this->flags = flags;
		}
	};

	const inst_t* check_accessibility(CallState& call_state, int_t accessibility);

	void push_scope(ScopeInfo* info = &empty_scope_info);
	void pop_scope();

	FunFrame* reserve_ff();
	void push_ff(CallState& call_state);
	const inst_t* pop_ff(int_t base, int_t result_count);
	void pop_ff_non();

	void execute();

	void upsize_variables(uint_t upsize){
		if(XTAL_UNLIKELY(variables_.size()<XTAL_VM_variables_top()+upsize+129)){
			upsize_variables_detail(upsize);
		}
		variables_top_ += upsize;
	}

	void upsize_variables_detail(uint_t upsize);

public:
	ArgumentsPtr inner_make_arguments(Method* fun);
	ArgumentsPtr inner_make_arguments(const NamedParam* params, int_t num);

private:
	AnyPtr append_backtrace(const inst_t* pc, const AnyPtr& ep);

	void ready();

public: // ���[�J���ϐ��n
	void set_local_variable_out_of_fun(uint_t pos, uint_t depth, const AnyPtr& value);
	AnyPtr& local_variable_out_of_fun(uint_t pos, uint_t depth);

	void set_local_variable(int_t pos, const AnyPtr& value){ /*XTAL_ASSERT(pos<variables_.size());*/ XTAL_VM_set_local_variable(pos, value); }
	AnyPtr& local_variable(int_t pos){ return XTAL_VM_local_variable(pos); }

private: // ��O�n
	const inst_t* push_except(const inst_t* pc);
	const inst_t* push_except(const inst_t* pc, const AnyPtr& e);

	const inst_t* catch_body(const inst_t* pc, const ExceptFrame& cur);

private: // �u���[�N�|�C���g�n
	void breakpoint_hook(const inst_t* pc, const MethodPtr& fun, int_t kind);

	void check_breakpoint_hook(const inst_t* pc, const MethodPtr& fun, int_t kind){
		if((*hook_setting_bit_&(1<<kind))==0){ return; }
		breakpoint_hook(pc, fun, kind);
	}
	
	void check_breakpoint_hook(const inst_t* pc, int_t kind){
		if((*hook_setting_bit_&(1<<kind))==0){ return; }
		breakpoint_hook(pc, XTAL_VM_ff().fun, kind);
	}

public:

//{DECLS{{
	const inst_t* FunInstLine(const inst_t* pc);
	const inst_t* FunInstLoadValue(const inst_t* pc);
	const inst_t* FunInstLoadConstant(const inst_t* pc);
	const inst_t* FunInstLoadInt1Byte(const inst_t* pc);
	const inst_t* FunInstLoadFloat1Byte(const inst_t* pc);
	const inst_t* FunInstLoadCallee(const inst_t* pc);
	const inst_t* FunInstLoadThis(const inst_t* pc);
	const inst_t* FunInstCopy(const inst_t* pc);
	const inst_t* FunInstMember(const inst_t* pc);
	const inst_t* FunInstUna(const inst_t* pc);
	const inst_t* FunInstArith(const inst_t* pc);
	const inst_t* FunInstBitwise(const inst_t* pc);
	const inst_t* FunInstAt(const inst_t* pc);
	const inst_t* FunInstSetAt(const inst_t* pc);
	const inst_t* FunInstGoto(const inst_t* pc);
	const inst_t* FunInstNot(const inst_t* pc);
	const inst_t* FunInstIf(const inst_t* pc);
	const inst_t* FunInstIfComp(const inst_t* pc);
	const inst_t* FunInstIfRawEq(const inst_t* pc);
	const inst_t* FunInstIfIs(const inst_t* pc);
	const inst_t* FunInstIfUndefined(const inst_t* pc);
	const inst_t* FunInstIfDebug(const inst_t* pc);
	const inst_t* FunInstPush(const inst_t* pc);
	const inst_t* FunInstPop(const inst_t* pc);
	const inst_t* FunInstAdjustValues(const inst_t* pc);
	const inst_t* FunInstLocalVariable(const inst_t* pc);
	const inst_t* FunInstSetLocalVariable(const inst_t* pc);
	const inst_t* FunInstInstanceVariable(const inst_t* pc);
	const inst_t* FunInstSetInstanceVariable(const inst_t* pc);
	const inst_t* FunInstInstanceVariableByName(const inst_t* pc);
	const inst_t* FunInstSetInstanceVariableByname(const inst_t* pc);
	const inst_t* FunInstFilelocalVariable(const inst_t* pc);
	const inst_t* FunInstSetFilelocalVariable(const inst_t* pc);
	const inst_t* FunInstFilelocalVariableByName(const inst_t* pc);
	const inst_t* FunInstSetFilelocalVariableByName(const inst_t* pc);
	const inst_t* FunInstCall(const inst_t* pc);
	const inst_t* FunInstSend(const inst_t* pc);
	const inst_t* FunInstProperty(const inst_t* pc);
	const inst_t* FunInstSetProperty(const inst_t* pc);
	const inst_t* FunInstScopeBegin(const inst_t* pc);
	const inst_t* FunInstScopeEnd(const inst_t* pc);
	const inst_t* FunInstReturn(const inst_t* pc);
	const inst_t* FunInstYield(const inst_t* pc);
	const inst_t* FunInstExit(const inst_t* pc);
	const inst_t* FunInstRange(const inst_t* pc);
	const inst_t* FunInstOnce(const inst_t* pc);
	const inst_t* FunInstSetOnce(const inst_t* pc);
	const inst_t* FunInstMakeArray(const inst_t* pc);
	const inst_t* FunInstArrayAppend(const inst_t* pc);
	const inst_t* FunInstMakeMap(const inst_t* pc);
	const inst_t* FunInstMapInsert(const inst_t* pc);
	const inst_t* FunInstMapSetDefault(const inst_t* pc);
	const inst_t* FunInstClassBegin(const inst_t* pc);
	const inst_t* FunInstClassEnd(const inst_t* pc);
	const inst_t* FunInstDefineClassMember(const inst_t* pc);
	const inst_t* FunInstDefineMember(const inst_t* pc);
	const inst_t* FunInstMakeFun(const inst_t* pc);
	const inst_t* FunInstMakeInstanceVariableAccessor(const inst_t* pc);
	const inst_t* FunInstTryBegin(const inst_t* pc);
	const inst_t* FunInstTryEnd(const inst_t* pc);
	const inst_t* FunInstPushGoto(const inst_t* pc);
	const inst_t* FunInstPopGoto(const inst_t* pc);
	const inst_t* FunInstThrow(const inst_t* pc);
	const inst_t* FunInstAssert(const inst_t* pc);
	const inst_t* FunInstBreakPoint(const inst_t* pc);
	const inst_t* FunInstMAX(const inst_t* pc);
//}}DECLS}

private:

	const FramePtr& make_outer(ScopeInfo* scope);
	const FramePtr& make_outer_outer(uint_t i = 0, uint_t call_n = 0, bool force = false);

private:
	inst_t end_code_;
	inst_t throw_code_;

	int_t result_base_;

	const inst_t* resume_pc_;
	
	int_t yield_base_;
	int_t yield_result_;
	int_t yield_need_result_count_;
	int_t yield_result_count_;

	const inst_t* throw_pc_;

	// �l�ێ��p�X�^�b�N
	FastStack<AnyPtr> stack_;

	// �֐��Ăяo���̓x�ɐς܂��t���[��
	FastStack<FunFrame*> fun_frames_;

	struct Scope{
		BasePtr<Frame> frame;
		uint_t pos;
		uint_t size;
		uint_t flags;

		enum{
			NONE = 0,
			FRAME = 1,
			CLASS = 2
		};
	};

	// �X�R�[�v���
	FastStack<Scope> scopes_;

	AnyPtr* variables_top_;
	xarray variables_;

	// try�̓x�ɐς܂��t���[��
	FastStack<ExceptFrame> except_frames_;
	
	AnyPtr except_[3];

	debug::HookInfoPtr debug_info_;
	uint_t* hook_setting_bit_;

	int_t thread_yield_count_;

	VMachine* parent_vm_;

public:

	void on_shrink_to_fit(){
		/*int_t top = variables_top();
		variables_.shrink_to_fit();
		variables_top_ = (Any*)variables_.data()+top; */
	}

	void on_visit_members(Visitor& m);

	int_t fun_frame_size(){
		return fun_frames_.size();
	}

protected:

	void add_ref_count_members(int_t i);

public:

	void print_info();

private:

	XTAL_DISALLOW_COPY_AND_ASSIGN(VMachine);

};

}

#endif // XTAL_VMACHINE_H_INCLUDE_GUARD
