#define BLEMM_IMPL
#define BLEMM_LOG_LVL 5
#include "blemm.h"

#define DEFAULT_BUILD_DIR "./build/"

void build_examples()
{
	BLEMM_LOGT("Going to build examples!");
	const char* example_path = "./examples/program_curses/main.c";
	const char* example_name = DEFAULT_BUILD_DIR"program_curses";
	CMD(example);
	CMD_APPEND(example, "cc");
	CMD_APPEND(example, example_path);
	CMD_APPEND(example, "-lcurses");
	CMD_APPEND(example, "-o");
	CMD_APPEND(example, example_name);
	CMD_EXEC_SYNC(example);
}

int main(int argc, char** argv)
{
	BLEMM_REBUILD_ME();
	BLEMM_LOGI("--- BEGIN BLEMM BUILD SCRIPT ---");
	build_examples();
	BLEMM_LOGI("--- END BLEMM BUILD SCRIPT ---");
}

