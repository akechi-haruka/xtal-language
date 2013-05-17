/** \file src/xtal/xtal_environment.h
* \brief src/xtal/xtal_environment.h
*/

#ifndef XTAL_ENVIRONMENT_H_INCLUDE_GUARD
#define XTAL_ENVIRONMENT_H_INCLUDE_GUARD

#pragma once

namespace xtal{

/// \name Xtal���̏������A�j������
//@{

/**
* \brief �A���P�[�^���C�u����
*/
class AllocatorLib{
public:
	virtual ~AllocatorLib(){}

	virtual void* malloc(std::size_t size){ return std::malloc(size); }
	virtual void free(void* p, std::size_t /*size*/){ std::free(p); }

	virtual void* malloc_align(std::size_t size, std::size_t alignment);
	virtual void free_align(void* p, std::size_t size, std::size_t alignment);

	virtual void out_of_memory(){}
};

/**
* \brief �����R�[�h���C�u����
*/
class ChCodeLib{
public:
	virtual ~ChCodeLib(){}

	virtual int_t ch_len(char_t /*lead*/){ return 1; }
	virtual int_t ch_len2(const char_t* str){ return ch_len(*str); }
	virtual int_t ch_inc(const char_t* data, int_t data_size, char_t* dest, int_t dest_size);
	virtual int_t ch_cmp(const char_t* a, int_t asize, const char_t* b, int_t bsize);
};

/**
* \brief �X���b�h���C�u����
*/
class ThreadLib{
public:
	virtual ~ThreadLib(){}

	virtual void* new_thread(){ return 0; }
	virtual void delete_thread(void* /*thread_object*/){}
	virtual void start_thread(void* /*thread_object*/, void (*callback)(void*), void* data){ callback(data); }
	virtual void join_thread(void* /*thread_object*/){}

	virtual void* new_mutex(){ return 0; }
	virtual void delete_mutex(void* /*mutex_object*/){}
	virtual void lock_mutex(void* /*mutex_object*/){}
	virtual void unlock_mutex(void* /*mutex_object*/){}

	virtual void* new_event(){ return 0; }
	virtual void delete_event(void* /*event_object*/){}
	virtual void wait_event(void* /*event_object*/){}
	virtual void signal_event(void* /*event_object*/){}

	virtual void yield(){}
	virtual void sleep(float_t /*sec*/){}
};

/**
* \brief �W�����o�̓��C�u����
*/
class StdStreamLib{
public:
	virtual ~StdStreamLib(){}

	virtual void* new_stdin_stream(){ return 0; }
	virtual void delete_stdin_stream(void* /*stdin_stream_object*/){}
	virtual uint_t read_stdin_stream(void* /*stdin_stream_object*/, void* /*dest*/, uint_t /*size*/){ return 0; }

	virtual void* new_stdout_stream(){ return 0; }
	virtual void delete_stdout_stream(void* /*stdout_stream_object*/){}
	virtual uint_t write_stdout_stream(void* /*stdout_stream_object*/, const void* /*src*/, uint_t /*size*/){ return 0; }

	virtual void* new_stderr_stream(){ return 0; }
	virtual void delete_stderr_stream(void* /*stderr_stream_object*/){}
	virtual uint_t write_stderr_stream(void* /*stderr_stream_object*/, const void* /*src*/, uint_t /*size*/){ return 0; }
};

/**
* \brief �t�@�C���V�X�e�����C�u����
*/
class FilesystemLib{
public:
	virtual ~FilesystemLib(){}

	virtual bool is_directory(const char_t* /*path*/){ return false; }
	virtual uint_t mtime(const char_t*){ return 0; }

	virtual void* new_file_stream(const char_t* /*path*/, const char_t* /*flags*/){ return 0; }
	virtual void delete_file_stream(void* /*file_stream_object*/){}
	virtual uint_t read_file_stream(void* /*file_stream_object*/, void* /*dest*/, uint_t /*size*/){ return 0; }
	virtual uint_t write_file_stream(void* /*file_stream_object*/, const void* /*src*/, uint_t /*size*/){ return 0; }
	virtual void seek_file_stream(void* /*file_stream_object*/, uint_t /*pos*/){}
	virtual uint_t tell_file_stream(void* /*file_stream_object*/){ return 0; }
	virtual bool end_file_stream(void* /*file_stream_object*/){ return true; }
	virtual uint_t size_file_stream(void* /*file_stream_object*/){ return 0; }
	virtual void flush_file_stream(void* /*file_stream_object*/){}

	virtual void* new_entries(const char_t* /*path*/){ return 0; }
	virtual void delete_entries(void* /*entries_object*/){}
	virtual const char_t* next_entries(void* /*entries_object*/){ return 0; }
	virtual void break_entries(void* /*entries_object*/){}
};

/**
* \brief �g�p���C�u�����̎w��̂��߂̍\����
*/
struct Setting{
	AllocatorLib* allocator_lib;
	ChCodeLib* ch_code_lib;
	ThreadLib* thread_lib;
	StdStreamLib* std_stream_lib;
	FilesystemLib* filesystem_lib;

	/**
	* \brief �قƂ�ǉ������Ȃ������ݒ肷��B
	*/
	Setting();
};

/**
* \brief Xtal���s�����쐬���A���������A�J�����g�ɐݒ肷��B
*/
void initialize(const Setting& setting);

/**
* \brief �J�����g��Xtal���s����j������B
*/
void uninitialize();

/**
* \brief ���݂̃J�����g��Xtal���s�����擾����B
*/
Environment* environment();

/**
* \brief �J�����g��Xtal���s����ݒ肷��B
*/
void set_environment(Environment* e);

//@}

ThreadLib* thread_lib();

StdStreamLib* std_stream_lib();

FilesystemLib* filesystem_lib();

/////////////////////////////////////////////////////

/// \name ����������
//@{

/**
* \brief ���[�U�[���o�^�����������A���P�[�g�֐����g���ă������m�ۂ���B
*/
void* xmalloc(size_t size);

/**
* \brief ���[�U�[���o�^�����������f�A���P�[�g�֐����g���ă������������B
*/
void xfree(void* p, size_t size);

/**
* \brief �������m�ۂ��X�R�[�v�ɕ����߂邽�߂̃��[�e�B���e�B�N���X
*/
struct XMallocGuard{
	/**
	* \brief �m�ۂ����ɍ\�z�����R���X�g���N�^
	*/
	XMallocGuard():p(0){}
	
	/**
	* \brief size���̃������m�ۂ���R���X�g���N�^
	*/
	explicit XMallocGuard(uint_t size):p(xmalloc(size)), sz(size){}

	/**
	* \brief ���������������
	*/
	~XMallocGuard(){ if(p)xfree(p, sz); }
public:	

	/**
	* \brief ���������m�ۂ���
	* ����܂łɊm�ۂ���Ă����������͉�������
	*/
	void malloc(size_t size){ if(p)xfree(p, sz); p = xmalloc(size); sz = size; }

	/**
	* \brief �������̐擪�A�h���X��Ԃ�
	*/
	void* get(){ return p; }

	/**
	* \brief �������̐擪�A�h���X��Ԃ�
	*/
	void* release(){
		void* temp = p;
		p = 0;
		return temp; 
	}

	/**
	* \brief �m�ۂ���Ă��郁�����̃T�C�Y��Ԃ�
	*/
	uint_t size(){ return sz; }

private:
	void* p;
	uint_t sz;

	XTAL_DISALLOW_COPY_AND_ASSIGN(XMallocGuard);
};

#define XTAL_PROTECT if(const ::xtal::Protect& xtalprotect = setjmp(::xtal::protect().buf)!=0)

#define XTAL_OUT_OF_MEMORY else 

//@}

/**
* \internal
* \brief jmp_buf������߂�\����
*/
struct JmpBuf{
	jmp_buf buf;
};

JmpBuf& protect();

void reset_protect();

bool ignore_memory_assert();

struct Protect{
	bool pass;
	Protect(bool pass):pass(pass){}
	~Protect(){ reset_protect(); }
	operator bool() const{ return !pass; }
};

/////////////////////////////////////////////////////

/**
* \xbind lib::builtin
* \brief �K�[�x�W�R���N�V���������s����
*
* ���قǎ��Ԃ͂�����Ȃ����A���S�ɃS�~������ł��Ȃ��K�[�x�W�R���N�g�֐�
* �Ⴆ�Ώz�Q�Ƃ͂���ł͌��m�ł��Ȃ��B
*/
void gc();

/**
* \xbind lib::builtin
* \brief �z�I�u�W�F�N�g���������t���K�[�x�W�R���N�V���������s����
*
* ���Ԃ͂����邪�A���݃S�~�ƂȂ��Ă�����̂͂Ȃ�ׂ��S�ĉ������K�[�x�W�R���N�g�֐�
* �z�Q�Ƃ����m�ł���B
*/
void full_gc();

/**
* \xbind lib::builtin
* \brief �K�[�x�W�R���N�V�����𖳌�������
*
* gc��full_gc�̌Ăяo���𖳌�������֐��B
* �����ł��ꂪ����Ăяo���ꂽ���L������Ă���A�Ăяo������enable_gc���ĂтȂ��ƃK�[�x�W�R���N�V�����͗L���ɂȂ�Ȃ�
*/
void disable_gc();

/**
* \xbind lib::builtin
* \brief �K�[�x�W�R���N�V������L��������

* disable_gc���Ă΂ꂽ�񐔂Ɠ��������Ăяo���ƃK�[�x�W�R���N�V�������L���ɂȂ�
*/
void enable_gc();

void set_gc_stress(bool b);

uint_t alive_object_count();

AnyPtr alive_object(uint_t i);

/////////////////////////////////////////////////////

/**
* \internal
* \brief key�ɑΉ�����C++�̃N���X�̃N���X�I�u�W�F�N�g��Ԃ��B
*/
const ClassPtr& cpp_class(CppClassSymbolData* key);

/**
* \brief �N���XT�ɑΉ�����C++�̃N���X�̃N���X�I�u�W�F�N�g��Ԃ��B
*/
template<class T>
inline const ClassPtr& cpp_class(){
	return cpp_class(&CppClassSymbol<T>::value);
}

const ClassPtr& cpp_class_index(uint_t index);

void set_cpp_class(CppClassSymbolData* key, const ClassPtr& cls);

/////////////////////////////////////////////////////

/**
* \internal
* \brief T�^�̃I�u�W�F�N�g����������o��
*/
const AnyPtr& cpp_value(CppValueSymbolData* key);

/**
* \brief T�^�̃I�u�W�F�N�g����������o��
*/
template<class T>
const SmartPtr<T>& cpp_value(){
	return unchecked_ptr_cast<T>(cpp_value(&CppValueSymbol<T>::value));
}

/////////////////////////////////////////////////////

/**
* \internal
* \brief �N���X�̃����o�����o���B
*/
//const AnyPtr& cache_member(Base* target_class, const IDPtr& primary_key, const AnyPtr& secondary_key, int_t& accessibility);

/**
* \internal
* \brief �N���X�̌p���֌W�𒲂ׂ�B
*/
//bool cache_is(const AnyPtr& target_class, const AnyPtr& klass);

/**
* \internal
* \brief �N���X�̃R���X�g���N�^���L���b�V������Ă��邩�璲�ׂ�B
*/
//bool cache_ctor(const AnyPtr& target_class, int_t kind);

/**
* \internal
* \brief �L���b�V���������B
*/
void clear_cache();

/**
* \internal
* \brief �����o�[�̃L���b�V���e�[�u���ɓo�^����Ă���f�[�^�𖳌��ɂ���B
*/
void invalidate_cache_member();

/**
* \internal
* \brief �p���֌W�̃L���b�V���e�[�u���ɓo�^����Ă���f�[�^�𖳌��ɂ���B
*/
void invalidate_cache_is();

/////////////////////////////////////////////////////

/**
* \internal
* \brief VMachine�C���X�^���X�������^������B
*/
VMachinePtr vmachine_take_over();

/**
* \internal
* \brief VMachine�C���X�^���X��ԋp����B
*/
void vmachine_take_back(const VMachinePtr& vm);

/**
* \internal
* \brief �f�t�H���g�Ŏg�p����VMachine�C���X�^���X��ݒ肷��B
*/
VMachinePtr set_vmachine(const VMachinePtr& vm);

/////////////////////////////////////////////////////

/// \name ���C�u�����I�u�W�F�N�g�擾�֐�
// @{

/**
* \brief builtin�I�u�W�F�N�g��Ԃ�
*/
const ClassPtr& builtin();

/**
* \brief lib�I�u�W�F�N�g��Ԃ�
*/
const LibPtr& lib();

/**
* \brief global�I�u�W�F�N�g��Ԃ�
*/
const ClassPtr& global();

/**
* \brief stdin�X�g���[���I�u�W�F�N�g��Ԃ�
*/
const StreamPtr& stdin_stream();

/**
* \brief stdout�X�g���[���I�u�W�F�N�g��Ԃ�
*/
const StreamPtr& stdout_stream();

/**
* \brief stderr�X�g���[���I�u�W�F�N�g��Ԃ�
*/
const StreamPtr& stderr_stream();

/**
* \brief VMachinePtr�I�u�W�F�N�g��Ԃ�
*
* �O���[�o����VMachinePtr�I�u�W�F�N�g��Ԃ��B
* �X���b�h���ɂ��̃O���[�o��VMachinePtr�I�u�W�F�N�g�͑��݂���B
*/
const VMachinePtr& vmachine();

VMachine* vmachine2();

const VMachinePtr& setup_call();
const VMachinePtr& setup_call(int_t need_result_count);

// @}

/**
* \brief ��������C���^�[���ςݕ�����ɕϊ�����
*/
IDPtr intern(const char_t* str);

/**
* \brief C�����񂩂�size���̒��������o���C���^�[���ςݕ�����ɕϊ�����
*
*/
IDPtr intern(const char_t* str, uint_t size);

/**
* \brief C�����񂩂�size���̒��������o���C���^�[���ςݕ�����ɕϊ�����
*
*/
IDPtr intern(const char_t* str, uint_t size, String::long_lived_t);

/**
* \brief C�����񃊃e��������C���^�[���ςݕ�����ɕϊ�����
*
*/
template<int N>
inline IDPtr intern(const LongLivedStringN<N>& str){
	return intern(str.str(), str.size(), String::long_lived_t());
}

/**
* \brief C�����񃊃e��������C���^�[���ςݕ�����ɕϊ�����
*
*/
IDPtr intern(const LongLivedString& str);

/**
* \brief String����C���^�[���ςݕ�����ɕϊ�����
*
*/
IDPtr intern(const StringPtr& name);		

/**
* \brief ��`�ς݃C���^�[���ςݕ�������擾����
*
*/
IDPtr fetch_defined_id(uint_t index);

/**
* \brief �������b�N����
*/
void xlock();

/**
* \brief �����A�����b�N����
*/
void xunlock();

struct XUnlock{
	XUnlock(int){ xunlock(); }
	~XUnlock(){ xlock(); }
	operator bool() const{ return true; }
private:
	XUnlock(const XUnlock&);
	void operator =(const XUnlock&);
};

struct XLock{
	XLock(int){ xlock(); }
	~XLock(){ xunlock(); }
	operator bool() const{ return true; }
private:
	XLock(const XLock&);
	void operator =(const XLock&);
};

#ifdef XTAL_NO_THREAD
#	define XTAL_UNLOCK 
#	define XTAL_LOCK 
#	define XTAL_UNLOCK_DIRECT 
#	define XTAL_LOCK_DIRECT 
#else
#	define XTAL_UNLOCK if(const ::xtal::XUnlock& xunlock = (XTAL_UNUSED_VAR(xunlock), 0))
#	define XTAL_LOCK if(const ::xtal::XLock& xlock = (XTAL_UNUSED_VAR(xlock), 0))
#	define XTAL_UNLOCK_DIRECT ::xtal::xunlock()
#	define XTAL_LOCK_DIRECT ::xtal::xlock()
#endif


bool register_thread(Environment*);
void unregister_thread(Environment*);

bool register_thread();
void unregister_thread();

struct XInterpose{
	XInterpose(int){ registered_ = register_thread(); }
	~XInterpose(){ if(registered_)unregister_thread(); }
	operator bool() const{ return true; }
private:
	XInterpose(const XInterpose&);
	void operator =(const XInterpose&);
private:
	bool registered_;
};

#define XTAL_INTERPOSE if(const ::xtal::XInterpose& xinterpose = (XTAL_UNUSED_VAR(xinterpose), 0))

/**
* \brief �e�L�X�g�}�b�v��Ԃ�
*/
const MapPtr& text_map();

/**
* \internal
* \brief �擪�o�C�g�����āA���̃}���`�o�C�g���������������𒲂ׂ�B
*
* �}�C�i�X�̒l���Ԃ��ꂽ�ꍇ�A�Œᕶ������Ԃ��B
* -2�̏ꍇ�A�Œ�2�����ȏ�̕����ŁA�{���̒�����2�����ڂ�ǂ܂Ȃ��Ɣ��f�ł��Ȃ��A�Ƃ����Ӗ��ƂȂ�B
*/
int_t ch_len(char_t lead);

/**
* \internal
* \brief �}���`�o�C�g���������������𒲂ׂ�B
*
* int_t ch_len(char_t lead)���Ă΂ꂽ��A�}�C�i�X�̒l��Ԃ����ꍇ�ɑ����ČĂԁB
* ch_len��-2�̒l��Ԃ�����́Astr�̐�ɂ͍Œ�2�o�C�g���̃f�[�^���i�[���邱�ƁB
*/
int_t ch_len2(const char_t* str);

/**
* \internal
* \brief ���̕�����Ԃ�
*
* �Ⴆ�� a ��n�����ꍇ�Ab ���Ԃ�
*/
int_t ch_inc(const char_t* data, int_t data_size, char_t* dest, int_t dest_size);

/**
* \internal
* \brief �����̑召����
*
* ���̒l a �̕����̕���b�̕�����菬����
* 0�̒l ������
* ���̒l b�̕����̕���a�̕�����菬����
*/
int_t ch_cmp(const char_t* a, uint_t asize, const char_t* b, uint_t bsize);


StreamPtr open(const StringPtr& file_name, const StringPtr& mode);

/// \name �R���p�C���n�֐�
//@{

/**
* \xbind lib::builtin
* \brief file_name�t�@�C�����R���p�C������B
* ���̖߂�l��serialize����ƁA�o�C�g�R�[�h�`���ŕۑ������B
* \param file_name Xtal�X�N���v�g���L�q���ꂽ�t�@�C���̖��O
* \return ���s�ł���Code�I�u�W�F�N�g
*/
CodePtr compile_file(const StringPtr& file_name);

/**
* \xbind lib::builtin
* \brief source���R���p�C������B
* ���̖߂�l��serialize����ƁA�o�C�g�R�[�h�`���ŕۑ������B
* \param source Xtal�X�N���v�g���L�q���ꂽString��Stream
* \return ���s�ł���Code�I�u�W�F�N�g
*/
CodePtr compile(const AnyPtr& source, const StringPtr& source_name = empty_string);

/**
* \xbind lib::builtin
* \brief file_name�t�@�C�����R���p�C�����Ď��s����B
* \param file_name Xtal�X�N���v�g���L�q���ꂽ�t�@�C���̖��O
* \return �X�N���v�g����return���ꂽ�l
*/
AnyPtr load(const StringPtr& file_name);

//@}

CodePtr source(const char_t* src, int_t size);
void exec_source(const char_t* src, int_t size);

CodePtr compiled_source(const void* src, int_t size);
void exec_compiled_source(const void* src, int_t size);

/**
* \brief require�̎��s��hook����֐���o�^����
* \param hook �t�b�N����֐�
*/
void append_require_source_hook(const AnyPtr& hook);
inline void set_require_source_hook(const AnyPtr& hook){ append_require_source_hook(hook); }

/**
* \brief require�̎��s��hook����֐���o�^��������
* \param hook �o�^�����֐�
*/
void remove_require_source_hook(const AnyPtr& hook);

/**
* \xbind lib::builtin
* \brief name�X�N���v�g��v�����ACode�I�u�W�F�N�g�̏�ԂŕԂ�
* \param name�X�N���v�g��
* �Ⴆ�΁Atest.xtal��ǂݍ��݂����ꍇ�Arequire_source("test")�Ƃ���Btest.xtalc������΂������ǂ݁A�Ȃ����test.xtal��ǂ����Ƃ���
* �������Aset_require_source_hook�Ńt�b�N���ꂽ�֐�������ꍇ�A
* \return name�X�N���v�g��Code�I�u�W�F�N�g
*/
CodePtr require_source(const StringPtr& name);

/**
* \xbind lib::builtin
* \brief name�X�N���v�g��v�����A���s�������ʂ�Ԃ�
* \param name�X�N���v�g��
* �Ⴆ�΁Atest.xtal��ǂݍ��݂����ꍇ�Arequire("test")�Ƃ���Btest.xtalc������΂������ǂ݁A�Ȃ����test.xtal��ǂ����Ƃ���
* \return name�X�N���v�g���Ԃ��I�u�W�F�N�g�Bname�X�N���v�g�������I�ɒl��Ԃ��Ȃ��ꍇ�ACode�I�u�W�F�N�g���Ԃ�B
*/
AnyPtr require(const StringPtr& name);

#ifndef XTAL_NO_PARSER

CodePtr eval_compile(const AnyPtr& source);

#endif

}

#endif // XTAL_ENVIRONMENT_H_INCLUDE_GUARD
