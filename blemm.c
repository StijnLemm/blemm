#include <stdio.h>
#define BLEMM_IMPL
#include "blemm.h"

int main(int argc, char** argv)
{
	BLEMM_LOGI("--- BEGIN BLEMM BUILD SCRIPT ---");
	CMD(build);
	BLEMM_LOGT("make build");
	CMD_APPEND(build, "cc");
	BLEMM_LOGI("make append cc");
	CMD_APPEND(build, "blemm.c");
	CMD_APPEND(build, "-o");
	CMD_APPEND(build, "blemm");
	CMD_EXEC_SYNC(build);
	BLEMM_LOGI("--- END BLEMM BUILD SCRIPT ---");
}

