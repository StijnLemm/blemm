#define BLEMM_IMPL
#define BLEMM_LOG_LVL 5
#include "blemm.h"

int main(int argc, char** argv)
{
	BLEMM_LOGI("--- BEGIN BLEMM BUILD SCRIPT ---");
	CMD(build);
	BLEMM_LOGI("Created build command");
	CMD_APPEND(build, "cc");
	CMD_APPEND(build, "blemm.c");
	CMD_APPEND(build, "-o");
	CMD_APPEND(build, "blemm");
	BLEMM_LOGI("Added args to build command");
	CMD_EXEC_SYNC(build);
	BLEMM_LOGI("Executed build command");
	BLEMM_LOGI("--- END BLEMM BUILD SCRIPT ---");
}

