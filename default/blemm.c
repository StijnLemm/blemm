#define BLEMM_IMPL
#define BLEMM_LOG_LVL 5
#include "blemm.h"

#define DEFAULT_BUILD_DIR "./build/"

void mkdir_build()
{
	CMD(mkdir);
	CMD_APPEND(mkdir, "mkdir");
	CMD_APPEND(mkdir, DEFAULT_BUILD_DIR);
	CMD_EXEC_SYNC(mkdir);
	CMD_FREE(mkdir);
}

void build()
{
	mkdir_build();
}

void run()
{
}

void print_usage()
{
	BLEMM_LOGI("Usage: ./blemm [arg]\n   * -> builds\n   run -> builds and runs");
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

	build();

	SKIP_ARG();
	const int should_run = strcmp(blemm_shift_args(&argc,&argv), "run") == 0;
	if (should_run)
	{
		run();
	}
	BLEMM_LOGI("--- END BLEMM BUILD SCRIPT ---");
}

