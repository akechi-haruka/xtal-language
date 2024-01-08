
#pragma once

namespace xtal{

inline void bind_error_message(){
	const char_t* messages[] = {
		XTAL_L("XCE1001"), XTAL_L("XCE1001:Syntax error."),
		XTAL_L("XCE1002"), XTAL_L("XCE1002:'%(required)s' expected but '%(char)s' was found."),
		XTAL_L("XCE1003"), XTAL_L("XCE1003:';' is missing."),
		XTAL_L("XCE1004"), XTAL_L("XCE1004:Illegal function argument."),
		XTAL_L("XCE1005"), XTAL_L("XCE1005:There is an unnamed argument after a named argument."),
		XTAL_L("XCE1006"), XTAL_L("XCE1006:Illegal break or continue statement."),
		XTAL_L("XCE1008"), XTAL_L("XCE1008:Illegal multiple assignment statement."),
		XTAL_L("XCE1009"), XTAL_L("XCE1009:An attempt is made to assign to a variable '%(name)s' which is not defined."),
		XTAL_L("XCE1010"), XTAL_L("XCE1010:Illegal numeric literal suffix."),
		XTAL_L("XCE1011"), XTAL_L("XCE1011:The file ended in the middle of a string literal."),
		XTAL_L("XCE1012"), XTAL_L("XCE1012:The left-hand side of an illegal assignment statement."),
		XTAL_L("XCE1013"), XTAL_L("XCE1013:We are trying to compute the result of a comparison expression."),
		XTAL_L("XCE1014"), XTAL_L("XCE1014:Invalid floating-point literal."),
		XTAL_L("XCE1015"), XTAL_L("XCE1015:Illegal %(n)d-decimal literal suffix."),
		XTAL_L("XCE1016"), XTAL_L("XCE1016:The number of arguments to the assert statement is incorrect."),
		XTAL_L("XCE1017"), XTAL_L("XCE1017:Illegal %%-notation literal."),
		XTAL_L("XCE1018"), XTAL_L("XCE1018:The default clause has been redundantly defined."),
		XTAL_L("XCE1019"), XTAL_L("XCE1019:'%(name)s' is not assignable."),
		XTAL_L("XCE1021"), XTAL_L("XCE1021:The file ended in the middle of a comment."),
		XTAL_L("XCE1022"), XTAL_L("XCE1022:The maximum number of multivalues that can be returned from the function is 255."),
		XTAL_L("XCE1023"), XTAL_L("XCE1023:Refers to an instance variable name '%(name)s' which is not defined."),
		XTAL_L("XCE1024"), XTAL_L("XCE1024:An instance variable name '%(name)s' of the same name has already been defined."),
		XTAL_L("XCE1025"), XTAL_L("XCE1025:We are trying to re-comparison the results of a comparison expression."),
		XTAL_L("XCE1026"), XTAL_L("XCE1026:In the same scope, the variable name '%(name)s' is already defined."),
		XTAL_L("XCE1027"), XTAL_L("XCE1027:The code was too large and the bytecode generation failed."),
		XTAL_L("XCE1028"), XTAL_L("XCE1028:The whitespace before and after the operator does not match the operator's precedence. This may be different from the expected precedence."),
		XTAL_L("XCE1029"), XTAL_L("XCE1029:For safety reasons, it is forbidden to use bitwise and as a conditional expression, e.g. a&1. Please compare in the form of (a&1)!=0."),
		XTAL_L("XCE1030"), XTAL_L("XCE1012:The left-hand side of the incorrect definition statement."),
		
		XTAL_L("XRE1001"), XTAL_L("XRE1001:The type of argument #'%(no)s' to the '%(object)s' function call is invalid: a value of type '%(type)s' was passed, even though type '%(required)s' was requested."),
		XTAL_L("XRE1002"), XTAL_L("XRE1002:A compile error occurred while compiling the source."),
		XTAL_L("XRE1003"), XTAL_L("XRE1003:Illegal instance variable reference."),
		XTAL_L("XRE1004"), XTAL_L("XRE1004:Type error. '%(required)s' type is required but a value of type '%(type)s' was passed."),
		XTAL_L("XRE1005"), XTAL_L("XRE1005:Illegal number of arguments in '%(object)s' function call: a function that accepts more than '%(min)s' arguments has been passed '%(value)s' arguments."),
		XTAL_L("XRE1006"), XTAL_L("XRE1006:Invalid number of arguments for '%(object)s' function call.' Passed '%(value)s' arguments to a function that accepts more than '%(min)s' and less than '%(max)s' arguments."),
		XTAL_L("XRE1007"), XTAL_L("XRE1007:Invalid number of arguments in '%(object)s' function call. Passed '%(value)s' arguments to a function that takes no arguments."),
		XTAL_L("XRE1008"), XTAL_L("XRE1008:'%(object)s' cannot be serialized."),
		XTAL_L("XRE1009"), XTAL_L("XRE1009:Incorrectly compiled Xtal file."),
		XTAL_L("XRE1010"), XTAL_L("XRE1010:A compile error has occurred."),
		XTAL_L("XRE1011"), XTAL_L("XRE1011:%(object)s :: '%(name)s' is already defined."),
		XTAL_L("XRE1012"), XTAL_L("XRE1012:yield was executed during the non-execution of fiber."),
		XTAL_L("XRE1013"), XTAL_L("XRE1013:Cannot create an instance of %(object)s because no constructor is registered for it."),
		XTAL_L("XRE1014"), XTAL_L("XRE1014:Cannot open file '%(name)s'."),
		XTAL_L("XRE1015"), XTAL_L("XRE1015:%(object)s not defined."),
		XTAL_L("XRE1016"), XTAL_L("XRE1016:A compile error occurred while compiling file '%(name)s'."),
		XTAL_L("XRE1017"), XTAL_L("XRE1017:%(object)s :: %(primary_key)s # %(secondary_key)s is %(accessibility)s."),
		XTAL_L("XRE1018"), XTAL_L("XRE1018:The stream is already closed."),
		XTAL_L("XRE1019"), XTAL_L("XRE1019:Multiple inheritance of classes defined in C++ is not allowed."),
		XTAL_L("XRE1020"), XTAL_L("XRE1020:This is an out-of-bounds access to the array."),
		XTAL_L("XRE1021"), XTAL_L("XRE1021:%(object)s is not defined.' You may have mistaken it for '%(pick)s'."),
		XTAL_L("XRE1023"), XTAL_L("XRE1023:Strings longer than 1 cannot be specified as range operators."),
		XTAL_L("XRE1024"), XTAL_L("XRE1024:Zero division error."),
		XTAL_L("XRE1025"), XTAL_L("XRE1025:ChRange must be a closed interval."),
		XTAL_L("XRE1026"), XTAL_L("XRE1026:Cannot convert to xpeg elements."),
		XTAL_L("XRE1027"), XTAL_L("XRE1027:The argument to the cap function is invalid, it should be a named argument like cap(name: value) or called cap(\"name\"), value)."),
		XTAL_L("XRE1028"), XTAL_L("XRE1028:Attempted to inherit from class '%(name)s' marked as final."),
		XTAL_L("XRE1029"), XTAL_L("XRE1029:The class '%(name)s' defined in C++ can only be inherited at class generation time."),
		XTAL_L("XRE1030"), XTAL_L("XRE1030:There is an implicit variable reference. %(name)s"),
		XTAL_L("XRE1031"), XTAL_L("XRE1031:Singleton classes can only be inherited by singleton classes."),
		XTAL_L("XRE1032"), XTAL_L("XRE1032:Cannot open file '%(name)s'."),
		XTAL_L("XRE1033"), XTAL_L("XRE1033:An attempt was made to read after the end of the stream."),
		XTAL_L("XRE1034"), XTAL_L("XRE1034:Attempted to execute an xpeg element which may cause an infinite loop"),
		XTAL_L("XRE1035"), XTAL_L("XRE1035:Illegal operation on a running, non-yielding fiber"),	
		XTAL_L("XRE1036"), XTAL_L("XRE1036:Incorrectly named argument for '%(object)s' function call. Named argument '%(name)s' not required by function was passed"),	
	};
	
	for(unsigned int i=0; i<sizeof(messages)/sizeof(*messages)/2; ++i){
		IDPtr key(*(LongLivedString*)messages[i*2+0]);
		text_map()->set_at(key, *(LongLivedString*)messages[i*2+1]);
	}
}
	
}
