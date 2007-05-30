
#pragma once

#include "xtal.h"

namespace xtal{
	
inline bool test_range(int ch, int begin, int end){
	return begin<=ch && ch<=end;
}

inline bool test_digit(int ch){
	return test_range(ch, '0', '9');
}

inline bool test_lalpha(int ch){
	return test_range(ch, 'a', 'z');
}

inline bool test_ualpha(int ch){
	return test_range(ch, 'A', 'Z');
}

inline bool test_alpha(int ch){
	return test_lalpha(ch) || test_ualpha(ch);
}

inline bool test_space(int ch){
	return ch==' ' || ch=='\t' || ch=='\n' || ch=='\r';
}

inline bool test_ident_first(int ch){
	return test_alpha(ch) || ch=='_';
}

inline bool test_ident_rest(int ch){
	return test_alpha(ch) || test_digit(ch) || ch=='_';
}

inline bool test_delim(int ch){
	return ch==';' || ch==':' || ch=='}' || ch==']' || ch==')' || ch==',';
}

inline bool test_xml_tag_name(int ch){
	return ch==';' || ch==':' || ch=='}' || ch==']' || ch==')' || ch==',';
}

inline int to_lalpha(int ch){
	using namespace std;
	return tolower(ch);
}
	
int read_ch(void* dest, const void* src);
	
void set_read_ch(int_t (*read_ch_fun)(char_t* dest, const void* src));

}