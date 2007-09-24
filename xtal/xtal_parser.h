
#pragma once

#ifndef XTAL_NO_PARSER

#include "xtal_ch.h"
#include "xtal_allocator.h"
#include "xtal_expr.h"

namespace xtal{

class Token{
public:

	enum Flag{
		FLAG_LEFT_SPACE = 1<<0,
		FLAG_RIGHT_SPACE = 1<<1
	};
	
	enum Type{	
		TYPE_TOKEN = 0,
		TYPE_INT = 1,
		TYPE_FLOAT = 2,
		TYPE_IDENTIFIER = 3,
		TYPE_KEYWORD = 4
	};
	
	enum Keyword{
		KEYWORD_NONE,
		
		KEYWORD_IF,
		KEYWORD_FOR,
		KEYWORD_ELSE,
		KEYWORD_FUN,
		KEYWORD_METHOD,
		KEYWORD_DO,
		KEYWORD_WHILE,
		KEYWORD_CONTINUE,
		KEYWORD_BREAK,
		KEYWORD_FIBER,
		KEYWORD_YIELD,
		KEYWORD_RETURN,
		KEYWORD_ONCE,
		KEYWORD_NULL,
		KEYWORD_NOP,
		KEYWORD_FALSE,
		KEYWORD_TRUE,
		KEYWORD_XTAL,
		KEYWORD_TRY,
		KEYWORD_CATCH,
		KEYWORD_FINALLY,
		KEYWORD_THROW,
		KEYWORD_CLASS,
		KEYWORD_CALLEE,
		KEYWORD_THIS,
		KEYWORD_CURRENT_CONTEXT,
		KEYWORD_DOFUN,
		KEYWORD_IS,
		KEYWORD_UNITTEST,
		KEYWORD_ASSERT,
		KEYWORD_PURE,
		KEYWORD_NOBREAK,
		KEYWORD_SWITCH,
		KEYWORD_CASE,
		KEYWORD_DEFAULT,
		KEYWORD_SINGLETON,

		KEYWORD_MAX
	};

	Token(){}
	
	Token(int_t type, const AnyPtr& value, int_t flags, int_t keyword_number = 0)
		:type_((u8)type), flags_((u8)flags), value_(value), keyword_number_(keyword_number){}

	int_t type() const{ return type_; }
	bool left_space() const{ return (flags_ & FLAG_LEFT_SPACE) != 0; }
	bool right_space() const{ return (flags_ & FLAG_RIGHT_SPACE) != 0; }
	int_t ivalue() const{ return ::xtal::ivalue((Innocence&)value_); }
	float_t fvalue() const{ return ::xtal::fvalue(value_); }
	const InternedStringPtr& identifier() const{ return *(InternedStringPtr*)&value_; }
	u16 keyword_number() const{ return keyword_number_; }

	StringPtr to_s() const{ 
		if(type_==TYPE_TOKEN){
			char_t buf[4] = {((ivalue()>>0)&0xff), ((ivalue()>>8)&0xff), ((ivalue()>>16)&0xff), 0};
			return buf;
		}else{
			return value_->to_s();
		}
	}

private:

	u8 type_;
	u8 flags_;
	u16 keyword_number_;
	AnyPtr value_;
	
};

class Reader{
public:

	Reader();

	void set_stream(const StreamPtr& stream){
		stream_ = stream;
	}

	/**
	* @brief 読み進める。
	*/
	int_t read();

	/**
	* @brief 次の要素を読む。
	*/
	int_t peek();

	/**
	* @brief 次の要素が引数chと同じだったら読み進める。
	* @param ch この値と次の要素が等しい場合に読み進める。
	* @retval true 次の要素はchと同じで、読み進めた。
	* @retval false 次の要素はchと異なり、読み進められなかった。
	*/
	bool eat(int_t ch);

	/**
	* @brief 要素を一つ戻す。
	*/
	void putback(int_t ch);

private:

	enum{ BUF_SIZE = 1024, BUF_MASK = BUF_SIZE-1 };

	StreamPtr stream_;

	char buf_[BUF_SIZE];

	uint_t pos_;
	uint_t read_;
};
	
class CompileError{
public:	

	void init(const StringPtr& source_file_name);
	void error(int_t line, const AnyPtr& message);

	ArrayPtr errors;
	StringPtr source_file_name;
};

/*
* XTALプログラムソースをトークン列に変換して取り出す
*/
class Lexer{
public:

	Lexer();
	
	/**
	* @brief 初期化
	*/
	void init(const StreamPtr& stream, CompileError* error);
	
	/**
	* @brief 読み進める
	*/
	Token read();

	/**
	* @brief 次の要素を読む
	*/
	Token peek();

	/**
	* @brief 読み込んだ要素を一つ戻す
	*/
	void putback();
	
	/**
	* @brief 指定したトークンを一つ戻す
	* 次のreadやpeekでは、これで戻した値が得られる
	*/
	void putback(const Token& ch);
	
	/**
	/**
	* @brief 現在の行数を返す
	*/
	int_t lineno(){ return lineno_; }
	
	/**
	* @brief 現在の行数を設定する
	*/
	void set_lineno(int_t v){ lineno_ = v; }

	/**
	* @brief 文字列の記録を開始する
	*/
	void begin_record();

	/**
	* @brief 文字列の記録を終了して、それを返す。
	*/
	StringPtr end_record();

	int_t read_direct();

	StringPtr read_string(int_t open, int_t close);

private:
	
	void do_read();

	void push(int_t v);
	void push_int(int_t v);
	void push_float(float_t v);
	void push_keyword(const InternedStringPtr& v, int_t num);
	void push_identifier(const InternedStringPtr& v);

	void deplete_space();

	InternedStringPtr parse_identifier();
	int_t parse_integer();
	int_t parse_hex();
	int_t parse_bin();

	void parse_number_suffix(int_t val);
	void parse_number_suffix(float_t val);
	void parse_number();
	
	int_t test_right_space(int_t ch);
	int_t read_from_reader();
	bool eat_from_reader(int_t ch);
	void putback_to_reader(int_t ch);

private:

	Reader reader_;
	MapPtr keyword_map_;

	string_t recorded_string_;
	bool recording_;

	enum{ BUF_SIZE = 64, BUF_MASK = BUF_SIZE-1 };
	Token buf_[BUF_SIZE];

	uint_t pos_;
	uint_t read_;
	int_t left_space_;
	uint_t lineno_;

	CompileError* error_;
};

class Parser{
public:

	Parser();

	ExprPtr parse(const StreamPtr& stream, CompileError* error);

	ExprPtr parse_stmt(const StreamPtr& stream, CompileError* error);

private:

	int_t lineno(){ return lexer_.lineno(); }
	
	void expect(int_t ch);
	
	bool eat(int_t ch);
	bool eat(Token::Keyword kw);

	ExprPtr parse_pre_expr(int_t code, int_t expr_priority);
	ExprPtr parse_post_expr(int_t code, int_t pri, int_t space, ExprPtr lhs, bool left, int_t expr_priority);
	ExprPtr parse_bin_expr(int_t code, int_t pri, int_t space, ExprPtr lhs, bool left, int_t expr_priority);
	ExprPtr parse_bin_comp_expr(int_t code, int_t pri, int_t space, ExprPtr lhs, bool left, int_t expr_priority);
	ExprPtr parse_post(ExprPtr lhs, int_t pri);
	ExprPtr parse_assign_stmt();
	ExprPtr parse_stmt();
	ExprPtr parse_stmt_must();
	ExprPtr parse_assert();
	ExprPtr parse_each(const InternedStringPtr& label, ExprPtr lhs);
	ArrayPtr parse_stmts();
	ArrayPtr parse_exprs(bool* discard = 0);
	InternedStringPtr parse_var();
	InternedStringPtr parse_identifier();
	InternedStringPtr parse_identifier_or_keyword();
	ExprPtr parse_if();
	ExprPtr parse_term();
	ExprPtr parse_toplevel();
	ExprPtr parse_scope();
	ExprPtr parse_switch();
	ExprPtr parse_frame(int_t kind);
	ExprPtr parse_class(int_t kind);
	ExprPtr parse_prop();
	ExprPtr parse_loop();
	ExprPtr parse_fun(int_t kind);
	ExprPtr parse_call(ExprPtr lhs);
	ExprPtr parse_expr(int_t pri);
	ExprPtr parse_expr();
	ExprPtr parse_expr_must(int_t pri);
	ExprPtr parse_expr_must();
	ExprPtr parse_array();
	ExprPtr parse_for(const InternedStringPtr& label = 0);
	ExprPtr parse_try();
	int_t parse_number_suffix();
	ExprPtr parse_number();
	ExprPtr parse_while(const InternedStringPtr& label = 0);
		
public:

	Lexer lexer_;
	bool expr_end_flag_;
	CompileError* error_;
};


}

#endif

