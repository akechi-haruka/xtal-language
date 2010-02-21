/** \file src/xtal/xtal_any.h
* \brief src/xtal/xtal_any.h
*/

#ifndef XTAL_ANY_H_INCLUDE_GUARD
#define XTAL_ANY_H_INCLUDE_GUARD

#pragma once

namespace xtal{

enum{
	SMALL_STRING_MAX = sizeof(void*) / sizeof(char_t)
};

struct AnyRawValue{
public:

	void init_primitive(PrimitiveType t){ type = t; ivalue = 0; }

	void init_int(char v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(signed char v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(unsigned char v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(short v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(unsigned short v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(int v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(unsigned int v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(long v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(unsigned long v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(long long v){ type = TYPE_INT; ivalue = (int_t)v; }
	void init_int(unsigned long long v){ type = TYPE_INT; ivalue = (int_t)v; }

	void init_float(float v){ type = TYPE_FLOAT; fvalue = (float_t)v; }
	void init_float(double v){ type = TYPE_FLOAT; fvalue = (float_t)v; }
	void init_float(long double v){ type = TYPE_FLOAT; fvalue = (float_t)v; }

	void init_bool(bool b){ type = TYPE_FALSE + (int)b; ivalue = 0; }

	void init_base(const Base* v){  
		type = TYPE_BASE; 
		pvalue = const_cast<Base*>(v); 
	}

	void init_rcbase(int t, const RefCountingBase* v){ 
		type = t; 
		rcpvalue = const_cast<RefCountingBase*>(v); 
	}

	void init_literal_string(const StringLiteral& v){ 
		type = TYPE_LITERAL_STRING | (v.size()<<STRING_SIZE_SHIFT); 
		spvalue = v.str(); 
	}

	void init_small_string(uint_t size){ 
		type = TYPE_SMALL_STRING | (size<<STRING_SIZE_SHIFT);
		ivalue = 0;	
	}

	void init_small_string(const char_t* str, uint_t size){ 
		type = TYPE_SMALL_STRING | (size<<STRING_SIZE_SHIFT);
		ivalue = 0;
		for(uint_t i=0; i<size; ++i){
			svalue[i] = str[i];
		}
	}

	void init_interned_string(const char_t* str, uint_t size){ 
		type = TYPE_INTERNED_STRING | (size<<STRING_SIZE_SHIFT);
		spvalue = str; 
	}

	void init_pointer(const void* v, uint_t i){ 
		type = TYPE_POINTER | (i<<CPP_CLASS_INDEX_SHIFT); 
		vpvalue = const_cast<void*>(v);
	}

	void init_stateless_native_method(const param_types_holder_n* v){  
		type = TYPE_STATELESS_NATIVE_METHOD; 
		pthvalue = v; 
	}

	void init_immediate_value(int_t value1, int_t value2){
		type = TYPE_IMMEDIATE_VALUE | (value1<<8);		
		ivalue = value2;
	}

	void init_immediate_value(int_t value1, float_t value2){
		type = TYPE_IMMEDIATE_VALUE | (value1<<8);		
		fvalue = value2;
	}

public:

	enum{
		HAVE_FINALIZER_FLAG_SHIFT = TYPE_SHIFT+1,
		HAVE_FINALIZER_FLAG_BIT = 1<<HAVE_FINALIZER_FLAG_SHIFT,

		REF_COUNT_SHIFT = HAVE_FINALIZER_FLAG_SHIFT+1,
		REF_COUNT_MASK = ~((1<<REF_COUNT_SHIFT)-1),

		STRING_SIZE_SHIFT = TYPE_SHIFT+1,
		STRING_SIZE_MASK = ~((1<<STRING_SIZE_SHIFT)-1),

		CPP_CLASS_INDEX_SHIFT = TYPE_SHIFT+1,
		CPP_CLASS_INDEX_MASK = ~((1<<CPP_CLASS_INDEX_SHIFT)-1)
	};

	bool have_finalizer() const{ return (type & HAVE_FINALIZER_FLAG_BIT)!=0; }
	void set_finalizer_flag(){ type |= HAVE_FINALIZER_FLAG_BIT; }
	void unset_finalizer_flag(){ type &= ~HAVE_FINALIZER_FLAG_BIT; }

	uint_t ref_count() const{ return (type & REF_COUNT_MASK)>>REF_COUNT_SHIFT; }
	void add_ref_count(int_t rc){ type += rc<<REF_COUNT_SHIFT; }
	void inc_ref_count(){ type += 1<<REF_COUNT_SHIFT; }
	void dec_ref_count(){ type -= 1<<REF_COUNT_SHIFT; }
	uint_t can_not_gc() const{ return type&(REF_COUNT_MASK|HAVE_FINALIZER_FLAG_BIT); }

	uint_t string_size() const{ return (type & STRING_SIZE_MASK)>>STRING_SIZE_SHIFT; }
	uint_t string_literal_size() const{ return (type & STRING_SIZE_MASK)>>STRING_SIZE_SHIFT; }
	uint_t cpp_class_index() const{ return (type & CPP_CLASS_INDEX_MASK)>>CPP_CLASS_INDEX_SHIFT; }

	int_t immediate_first_value() const{ return (int_t)(type>>8); }
	int_t immediate_second_value() const{ return ivalue; }
	float_t immediate_secondf_value() const{ return fvalue; }

public:

	int_t t() const{ return type; }
	int_t i() const{ return ivalue; }
	uint_t u() const{ return uvalue; }
	float_t f() const{ return fvalue; }
	Base* p() const{ return pvalue; }
	RefCountingBase* r() const{ return rcpvalue; }
	char_t* s(){ return svalue; }
	const char_t* s() const{ return svalue; }
	const char_t* sp() const{ return spvalue; }
	void* vp() const{ return vpvalue; }
	const param_types_holder_n* pth() const{ return pthvalue; }
	
	friend bool raweq(const AnyRawValue& a, const AnyRawValue& b){
		return (a.type&TYPE_MASK)==(b.type&TYPE_MASK) && a.ivalue==b.ivalue;
		//return rawbitxor(a, b)==0;
	}

	friend bool rawlt(const AnyRawValue& a, const AnyRawValue& b){
		int atype = a.type&TYPE_MASK;
		int btype = b.type&TYPE_MASK;
		if(atype<btype)return true;
		if(btype<atype)return false;
		return a.ivalue<b.ivalue;
	}

	friend uint_t rawbitxor(const AnyRawValue& a, const AnyRawValue& b){
		return ((a.type^b.type)&TYPE_MASK) | (a.ivalue^b.ivalue);
	}

public:
	union{
		int_t ivalue;
		uint_t uvalue;
		float_t fvalue;
		Base* pvalue;
		RefCountingBase* rcpvalue;
		const char_t* spvalue;
		char_t svalue[SMALL_STRING_MAX];
		void* vpvalue;
		const param_types_holder_n* pthvalue;
	};

	int_t type;
};

/**
* \xbind lib::builtin
* \brief すべてのクラスの基底クラス
*/
class Any{
public:

	/**
	* \brief 関数オブジェクトとみなし、関数呼び出しをする。
	* 引数や戻り値はvmを通してやり取りする。
	*/
	void rawcall(const VMachinePtr& vm) const;
	
	/**
	* \brief メソッド呼び出しをする
	* 引数や戻り値はvmを通してやり取りする。
	* \param vm 仮想マシン
	* \param primary_key メンバ名
	* \param secondary_key セカンダリキー
	* \param self 可触性に影響するオブジェクト
	* \param inherited_too 継承元クラスからもメソッド検索をするかどうか
	*/
	void rawsend(const VMachinePtr& vm, const IDPtr& primary_key, const AnyPtr& secondary_key, bool inherited_too, bool q) const;
	void rawsend(const VMachinePtr& vm, const IDPtr& primary_key) const;
	void rawsend(const VMachinePtr& vm, const IDPtr& primary_key, const AnyPtr& secondary_key) const;
	void rawsend(const VMachinePtr& vm, const IDPtr& primary_key, const AnyPtr& secondary_key, bool inherited_too) const;

	/**
	* \brief メンバを取得する。
	* \retval undefined そのメンバは存在しない
	* \retval 非undefined nameに対応したメンバ  
	*/
	const AnyPtr& member(const IDPtr& primary_key, const AnyPtr& secondary_key = (const AnyPtr&)undefined, bool inherited_too = true, int_t& accessibility = Temp()) const;

	/**
	* \brief メンバを定義する。
	* \param primary_key メンバ名
	* \param secondary_key セカンダリキー
	*/
	void def(const IDPtr& primary_key, const AnyPtr& value, const AnyPtr& secondary_key = (const AnyPtr&)undefined, int_t accessibility = 0) const;

	/**
	* \xbind
	* \brief このオブジェクトが所属するクラスを返す。
	*/
	const ClassPtr& get_class() const;

	/**
	* \brief 整数に変換して返す。
	*/
	int_t to_i() const;
	
	/**
	* \brief 浮動小数点数に変換して返す。
	*/
	float_t to_f() const;
	
	/**
	* \brief 文字列に変換して返す。
	*/
	StringPtr to_s() const;

	/**
	* \brief 配列に変換して返す。
	*/
	ArrayPtr to_a() const;

	/**
	* \brief 連想配列に変換して返す。
	*/
	MapPtr to_m() const;

	/**
	* \brief klassクラスのインスタンスか調べる。
	*/
	bool is(const AnyPtr& klass) const;

	bool is(CppClassSymbolData* key) const;
		
	/**
	* \brief klassクラスを継承しているか調べる
	*/
	bool is_inherited(const AnyPtr& klass) const;
	
	/**
	* \brief このオブジェクトがメンバとなっている親のクラスを返す。
	*/
	const ClassPtr& object_parent() const;
	
	/**
	* \brief このオブジェクトに親を設定する。
	* 親を持てないオブジェクトや、前に付けられた親の方が強い場合無視される。
	* \param parent 親
	*/
	void set_object_parent(const ClassPtr& parent) const;

	/**
	* \xbind
	* \brief オブジェクトの名前を返す。
	*/
	StringPtr object_name() const;

	/**
	* \xbind
	* \brief オブジェクトの名前のリストを返す
	* 一番最上位の親からの名前のリストを返す
	*/
	ArrayPtr object_name_list() const;

	/**
	* \xbind
	* \brief 自身を文字列化してprintlnする。
	* 文字列化にはto_sメソッドが呼ばれる。
	* \return 自身を返す。
	*/
	const AnyPtr& p() const;

public:

	MapPtr s_save() const;

	void s_load(const MapPtr& v) const;

	AnyPtr save_instance_variables(const ClassPtr& cls) const;

	void load_instance_variables(const ClassPtr& cls, const AnyPtr& v) const;

	void visit_members(Visitor& m) const;

	StringPtr ask_object_name_to_parent() const;

	InstanceVariables* instance_variables() const;

	bool op_eq( const AnyPtr& v ) const;

public:

	/// \brief primary_keyメソッドを呼び出す
	AnyPtr send(const IDPtr& primary_key) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key) const;

	/// \brief 関数を呼び出す
	AnyPtr call() const;

//{REPEAT{{
/*
	/// \brief primary_keyメソッドを呼び出す
	template<class A0 #COMMA_REPEAT#class A`i+1`#>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  #COMMA_REPEAT#const A`i+1`& a`i+1`#) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 #COMMA_REPEAT#class A`i+1`#>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  #COMMA_REPEAT#const A`i+1`& a`i+1`#) const;

	/// \brief 関数を呼び出す
	template<class A0 #COMMA_REPEAT#class A`i+1`#>
	AnyPtr call(const A0& a0  #COMMA_REPEAT#const A`i+1`& a`i+1`#) const;
*/

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 >
	AnyPtr send(const IDPtr& primary_key, const A0& a0  ) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 >
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  ) const;

	/// \brief 関数を呼び出す
	template<class A0 >
	AnyPtr call(const A0& a0  ) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1>
	AnyPtr call(const A0& a0  , const A1& a1) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12) const;

	/// \brief primary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
	AnyPtr send(const IDPtr& primary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12, const A13& a13) const;

	/// \brief primary_key#secondary_keyメソッドを呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
	AnyPtr send2(const IDPtr& primary_key, const AnyPtr& secondary_key, const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12, const A13& a13) const;

	/// \brief 関数を呼び出す
	template<class A0 , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
	AnyPtr call(const A0& a0  , const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10, const A11& a11, const A12& a12, const A13& a13) const;

//}}REPEAT}

private:

	// call(VMachinePtr)がコンパイルエラーとなるように
	struct cmpitle_error{ cmpitle_error(const VMachinePtr& vm); };
	void call(cmpitle_error) const;

private:

	StringPtr defined_place_name(const CodePtr& code, int_t pc, int_t name_number) const;

protected:

	struct noinit_t{};

	void init(RefCountingBase* p);

public: // 

	AnyRawValue value_;

public:

	friend const AnyRawValue& rawvalue(const Any& v);
	friend AnyRawValue& rawvalue(Any& v);
};

/////////////////////////////////////////////////////////

inline const AnyRawValue& rawvalue(const Any& v){
	return v.value_;
}

inline AnyRawValue& rawvalue(Any& v){
	return v.value_;
}

inline uint_t rawtype(const Any& v){
	return (uint_t)(rawvalue(v).t());
}

inline int_t type(const Any& v){ 
	return rawtype(v) & TYPE_MASK; 
}

inline int_t ivalue(const Any& v){ 
	return rawvalue(v).i(); 
}

inline float_t fvalue(const Any& v){ 
	return rawvalue(v).f(); 
}

inline int_t chvalue(const Any& v){ 
	return rawvalue(v).s()[0];
}

inline Base* pvalue(const Any& v){ 
	XTAL_ASSERT(type(v)==TYPE_BASE || type(v)==TYPE_NULL); 
	return rawvalue(v).p(); 
}

inline RefCountingBase* rcpvalue(const Any& v){ 
	XTAL_ASSERT(type(v)>=TYPE_BASE || type(v)==TYPE_NULL); 
	return rawvalue(v).r(); 
}

inline bool is_undefined(const Any& a){
	return rawtype(a)==TYPE_UNDEFINED;
}

inline bool is_null(const Any& a){
	return rawtype(a)==TYPE_NULL;
}

inline bool raweq(const Any& a, const Any& b){
	return raweq(rawvalue(a), rawvalue(b));
}

inline bool rawne(const Any& a, const Any& b){
	return !raweq(a, b);
}

inline bool rawlt(const Any& a, const Any& b){
	return rawlt(rawvalue(a), rawvalue(b));
}

inline uint_t rawbitxor(const Any& a, const Any& b){
	return rawbitxor(rawvalue(a), rawvalue(b));
}

inline void copy_any(Any& v, const Any& u){
	v = u;
}

inline void swap(Any& a, Any& b){
	Any temp = a;
	a = b;
	b = temp;
}

}

#endif // XTAL_ANY_H_INCLUDE_GUARD
