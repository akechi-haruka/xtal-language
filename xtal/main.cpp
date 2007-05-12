
#include "xtal/xtal.h"
#include "xtal/all_src.h"
#include "xtal/message_jp_sjis.txt"

using namespace xtal;

static void print_usage(){
	fprintf(stderr,
		"usage: xtal [options] [script [args]].\n"
		"Available options are:\n"
		"  -v       show version information\n"
	);
}

static void handle_argv(char** argv){
	int i;
	for(i=1; argv[i]!=0; i++){
		if(argv[i][0]!='-')
			break;

		switch(argv[i][1]){
		case 'v':
			fprintf(stderr, "xtal %d.%d.%d.%d\n", VERSION1, VERSION2, VERSION3, VERSION4);
			break;

		default:
			print_usage();
			return;
		}
	
	}

	if(argv[i]!=0){
		Array arg_list;
		const char *filename = argv[i++];
		for(i=1; argv[i]!=0; i++){
			arg_list.push_back(argv[i]);
		}
		builtin().def("argv", arg_list);
		load(filename);
	}
}

int main(int argc, char** argv){

	try{

		set_thread();
		initialize();
		
		MemoryStream ms(message_data, sizeof(message_data));
		set_get_text_map(object_load(ms));

		handle_argv(argv);

		/*
		load("../test/test_fib.xtal");
		load("../test/test_calc.xtal");
		load("../test/test_for.xtal");
		load("../test/test_iter.xtal");
		load("../test/test_except.xtal");
		load("../test/test_fiber.xtal");
		load("../test/test_if.xtal");
		load("../test/test_nested_loops.xtal");
		load("../test/test_assign.xtal");
		load("../test/test_op_assign.xtal");
		load("../test/test_inc.xtal");
		load("../test/test_toplevel.xtal");
		*/
	
		//*
		MemoryStream out;
		object_dump(compile_file("../message.xtal"), out);
		unsigned char* data = (unsigned char*)out.data();
		int size = out.tell();

		for(int i=0; i<size; ++i){
			printf("0x%02x, ", data[i]);
			if((i%8)==7){
				printf("\n");
			}
		}
		printf("\n");
		//*/

		//ix();

	}catch(Any e){
		fprintf(stderr, "%s\n", e.to_s().c_str());
	}

	uninitialize();

	return 0;
}
/**/