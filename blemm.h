#ifndef BLEMM_H
#define BLEMM_H

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

// declarations of blemm.
enum  blemm_err_e
{
	NO_ERROR,
	NO_MEM,
};
typedef enum blemm_err_e blemm_err_t ;

typedef struct
{
	char* items;
	size_t count;
} blemm_cstr_t;

typedef struct 
{
	blemm_cstr_t* items;
	size_t count;
	size_t capacity;
} blemm_cstr_builder_t;

typedef struct
{
	blemm_cstr_t* items;
	size_t count;
	size_t capacity;
} blemm_cmd_t;

void blemm_cpy_to_cstr(blemm_cstr_t* cstr, const char* str);
void blemm_join_cstr(blemm_cstr_t* dest, const blemm_cstr_t* src_list, size_t src_list_size);

void blemm_append_arg(blemm_cmd_t* cmd, const char* arg);
bool blemm_exec_cmd_sync(blemm_cmd_t* cmd);
void blemm_execv_cmd(blemm_cmd_t* cmd);

int blemm_path1_older_path2_cstr(const blemm_cstr_t* path1, const blemm_cstr_t* path2);
int blemm_path1_older_path2(const char* path1, const char* path2);
bool blemm_rebuild_me(int argc, char** argv);

char* blemm_shift_args(int* argc, char*** argv);

#define BLEMM_SCRIPT_FILE "blemm.c"
#define BLEMM_BIN_FILE "blemm"

#define BLEMM_REBUILD_ME(argc, argv) blemm_rebuild_me(argc, argv)
#define PATH_OLDER(path1, path2) blemm_path1_older_path2(path1, path2)

#define CMD(name) blemm_cmd_t name = {0}
#define CMD_FREE(cmd) free((cmd).items)
#define CMD_APPEND(cmd, arg) blemm_append_arg(&cmd, arg) 
#define CMD_EXEC_SYNC(cmd) blemm_exec_cmd_sync(&cmd)
#define CMD_EXECV(cmd) blemm_execv_cmd(&cmd)

#define SKIP_ARG() (void)blemm_shift_args(&argc, &argv)

#ifndef BLEMM_LOG_LVL
#error "please define BLEMM_LOG_LVL"
#endif

#if BLEMM_LOG_LVL < 0 || BLEMM_LOG_LVL > 5
#error "BLEMM_LOG_LVL incorrect, use 0 as NONE up to 5 being TRACE"
#endif

#if BLEMM_LOG_LVL > 4
#define BLEMM_LOGT(fmt, ...) printf("[TRACE] "fmt"\n", ## __VA_ARGS__)
#else
#define BLEMM_LOGT(fmt, ...)
#endif

#if BLEMM_LOG_LVL > 3
#define BLEMM_LOGI(fmt, ...) printf("[INFO] "fmt"\n", ## __VA_ARGS__)
#else
#define BLEMM_LOGI(fmt, ...)
#endif

#if BLEMM_LOG_LVL > 2
#define BLEMM_LOGW(fmt, ...) printf("[WARN] "fmt"\n", ## __VA_ARGS__)
#else
#define BLEMM_LOGW(fmt, ...)
#endif

#if BLEMM_LOG_LVL > 1
#define BLEMM_LOGE(fmt, ...) printf("[ERROR] "fmt"\n", ## __VA_ARGS__)
#else
#define BLEMM_LOGE(fmt, ...)
#endif

#if BLEMM_LOG_LVL > 0
#define BLEMM_LOGF(fmt, ...) printf("[FATAL] "fmt"\n", ## __VA_ARGS__)
#else
#define BLEMM_LOGF(fmt, ...)
#endif

// implementations of blemm.
#ifdef BLEMM_IMPL

#define BLEMM_DAR_INIT_SIZE 16
#define BLEMM_DAR_CHUNK_SIZE 16

#define BLEMM_MEMCPY(dest, src, size) memcpy(dest, src, size)
#define BLEMM_REALLOC(ptr, size) realloc(ptr, size)
#define BLEMM_MALLOC(size) malloc(size)

#ifndef __cplusplus

#define BLEMM_DAR_AP(array, item)								\
if ((array)->capacity < BLEMM_DAR_INIT_SIZE)							\
{                    										\
	(array)->capacity = BLEMM_DAR_CHUNK_SIZE;  						\
	(array)->items = BLEMM_MALLOC(BLEMM_DAR_INIT_SIZE * sizeof(item));			\
}												\
if ((array)->count++ >= (array)->capacity) 							\
{  												\
	(array)->capacity += BLEMM_DAR_CHUNK_SIZE;  						\
	(array)->items = BLEMM_REALLOC((array)->items, (array)->capacity * sizeof(item));	\
}       											\
(array)->items[(array)->count - 1] = item;       					        \

#else

#define BLEMM_DAR_AP(array, item)   								\
if ((array)->capacity < BLEMM_DAR_INIT_SIZE)							\
{                    										\
	(array)->capacity = BLEMM_DAR_CHUNK_SIZE;  						\
	(array)->items = (decltype((array)->items))BLEMM_MALLOC(BLEMM_DAR_INIT_SIZE * sizeof(item));	\
}												\
if ((array)->count++ >= (array)->capacity) 							\
{  												\
	(array)->capacity += BLEMM_DAR_CHUNK_SIZE;  						\
	(array)->items = (decltype((array)->items))BLEMM_REALLOC((array)->items, (array)->capacity * sizeof(item));	\
}       											\
(array)->items[(array)->count - 1] = item;       						\

#endif

void blemm_cpy_to_cstr(blemm_cstr_t* cstr, const char* str)
{
	assert(cstr && "Cstr ptr is null");
	assert(str && "Str ptr is null");

	const size_t len = strlen(str);
	char* cpy_str = (char*)BLEMM_MALLOC(len * sizeof(char));

	assert(cpy_str && "Malloc failed!");

	BLEMM_MEMCPY((void*)cpy_str, str, len * sizeof(char));
	cstr->items = cpy_str;
	cstr->count = len;
}

void blemm_join_cstr(blemm_cstr_t* dest, const blemm_cstr_t* src_list, size_t src_list_size)
{
	assert(dest && "dest cstr is null");
	assert(src_list && "src_list is null");
	assert(src_list_size && "src_list_size is zero");

	size_t needed_size = 0u;
	for(size_t i = 0; i < src_list_size; i++)
	{
		needed_size += src_list[i].count;
	}

	assert(needed_size && "join needed size is zero?");

	dest->items = (char*)BLEMM_MALLOC(needed_size * sizeof(char));
	char* ptr = dest->items;

	assert(ptr && "join memory allocation failed!");

	for(size_t i = 0; i < src_list_size; i++)
	{
		BLEMM_MEMCPY(ptr, src_list[i].items, src_list[i].count);
		ptr += (src_list[i].count * sizeof(char));
	}
}

void blemm_append_arg(blemm_cmd_t* cmd, const char* arg)
{
	assert(cmd && "Trying to append arg to null cmd!");
	assert(arg && "Trying to append null arg to cmd!");

	blemm_cstr_t str = {0};

	const size_t len = strlen(arg);
	const size_t new_len = len + 3u;
	str.items = (char*)BLEMM_MALLOC(new_len * sizeof(char));
	BLEMM_MEMCPY(&str.items[1], arg, len);

	// this sucks
	str.items[0] = '\"';
	str.items[len + 1] = '\"';
	str.items[len + 2] = ' ';
	str.count = new_len;

	BLEMM_DAR_AP(cmd, str);
}

bool blemm_exec_cmd_sync(blemm_cmd_t* cmd)
{
	assert(cmd && "Trying to execute null cmd");

	blemm_cstr_t cstr = {0};
	blemm_join_cstr(&cstr, cmd->items, cmd->count);
	BLEMM_LOGT("Executing command: ");
	BLEMM_LOGT("%s", cstr.items);

	BLEMM_LOGT("--- CMD -> STDOUT ---");
	const int ret = system(cstr.items);
	BLEMM_LOGT("--- CMD x> STDOUT ---");
	const bool failed = (ret != 0);
	if (failed)
		BLEMM_LOGE("System cmd returned non-zero: %d", ret);

	free(cstr.items);
	return !failed;
}

void blemm_execv_cmd(blemm_cmd_t* cmd)
{
	assert(cmd && "Trying to execute null cmd");

	blemm_cstr_t cstr = {0};
	blemm_join_cstr(&cstr, cmd->items, cmd->count);
	BLEMM_LOGT("Execv command: ");
	BLEMM_LOGT("%s", cstr.items);

	execv(cstr.items, NULL);
	free(cstr.items);
}

int blemm_path1_older_path2_cstr(const blemm_cstr_t* path1, const blemm_cstr_t* path2)
{
	assert(path1 && "path1 is null");
	assert(path2 && "path2 is null");
	return blemm_path1_older_path2(path1->items, path2->items);
}

int blemm_path1_older_path2(const char* path1, const char* path2)
{
	assert(path1 && "path1 is null");
	assert(path2 && "path2 is null");

	int error = -1;
	struct stat fb;

	error = stat(path1, &fb);

	if (error != 0) {
		BLEMM_LOGW("stat failed error: %d, path: %s", error, path1);
		return error;
	}

	const time_t fb1_time = fb.st_mtime;

	error = stat(path2, &fb);

	if (error != 0) {
		BLEMM_LOGW("stat failed error: %d, path: %s", error, path2);
		return error;
	}

	const time_t fb2_time = fb.st_mtime;

	// TODO return sensable value?
	return (fb1_time > fb2_time) ? 1 : 0;
}

bool blemm_rebuild_me(int argc, char** argv)
{
	assert(argc && "Argc is zero!");
	assert(argv && "Argv is null!");

	const char* program = argv[0];
	const int is_older = PATH_OLDER(BLEMM_SCRIPT_FILE, program);
	const int header_changed = PATH_OLDER("blemm.h", program); // TODO: for development of this header!

	if (!is_older && !header_changed)
	{
		return true;
	}

	BLEMM_LOGI("Blemm detected a change, going to rebuild itself!");

	CMD(mov);
	CMD_APPEND(mov, "mv");
	CMD_APPEND(mov, program);
	CMD_APPEND(mov, "blemm.old");
	CMD_EXEC_SYNC(mov);
	CMD_FREE(mov);

	CMD(build);
	CMD_APPEND(build, "cc");
	CMD_APPEND(build, "blemm.c");
	CMD_APPEND(build, "-o");
	CMD_APPEND(build, "blemm");

	if(!CMD_EXEC_SYNC(build))
	{
		BLEMM_LOGE("Rebuilding blemm failed. Check your blemm.c for errors!");
		exit(1);
	}

	CMD(blemm);
	for (size_t i = 0; i < argc; i++)
	{
		CMD_APPEND(blemm, argv[i]);
	}
	BLEMM_LOGT("Going to open new blemm!");
	CMD_EXECV(blemm);

	return false;
}

char* blemm_shift_args(int* argc, char*** argv)
{
	assert(*argc > 0 && "Shift arguments failed");
	char* result = **argv;
	(*argc)--;
	(*argv)++;
	return result;
}

#endif // BLEMM_IMPL

#endif // BLEMM_H
