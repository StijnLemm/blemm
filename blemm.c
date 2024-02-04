#define BLEMM_IMPL
#define BLEMM_LOG_LVL 5
#include "blemm.h"

#define DEFAULT_BUILD_DIR "./build/"

void build_example_curses()
{
	BLEMM_LOGT("Going to build example_curses!");
	const char* example_path = "./examples/program_curses/main.c";
	const char* example_name = DEFAULT_BUILD_DIR"program_curses";
	CMD(example);
	CMD_APPEND(example, "cc");
	CMD_APPEND(example, example_path);
	CMD_APPEND(example, "-lcurses");
	CMD_APPEND(example, "-o");
	CMD_APPEND(example, example_name);
	CMD_EXEC_SYNC(example);
	CMD_FREE(example);
}

void run_example_curses()
{
	BLEMM_LOGT("Going to run example_curses");
	const char* example_name = DEFAULT_BUILD_DIR"program_curses";
	CMD(example);
	CMD_APPEND(example, example_name);
	CMD_EXEC_SYNC(example);
	CMD_FREE(example);
}

void print_usage()
{
	BLEMM_LOGI("Usage: ./blemm [arg]\n   build -> builds example_curses\n   run -> builds example_curses and run");
}

int main(int argc, char** argv)
{
	BLEMM_REBUILD_ME(argc, argv);
	BLEMM_LOGI("--- BEGIN BLEMM BUILD SCRIPT ---");
	if (argc < 2)
	{
		print_usage();
		return 0;		
	}

	build_example_curses();
	SKIP_ARG();
	const int run = strcmp(blemm_shift_args(&argc,&argv), "run") == 0;
	if (run)
	{
		run_example_curses();
	}
	BLEMM_LOGI("--- END BLEMM BUILD SCRIPT ---");
}

