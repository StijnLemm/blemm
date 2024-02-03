#ifndef BLEMM_H
#define BLEMM_H

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
void blemm_join_cstr(blemm_cstr_t* dest, blemm_cstr_t* src_list, size_t src_list_size);
void blemm_cstr_builder_create(blemm_cstr_t* dest, blemm_cstr_builder_t* builder);
void blemm_append_arg(blemm_cmd_t* cmd, const char* arg);
bool blemm_exec_cmd_sync(blemm_cmd_t* cmd);

#define CMD(name) blemm_cmd_t name = {0}
#define CMD_APPEND(cmd, arg) blemm_append_arg(&cmd, "\""arg"\" ") 
#define CMD_EXEC_SYNC(cmd) blemm_exec_cmd_sync(&cmd)

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

#define BLEMM_DAR_AP(array, item)   									\
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
	(array)->items[(array)->count - 1] = item;       						\

void blemm_cpy_to_cstr(blemm_cstr_t* cstr, const char* str)
{
	assert(cstr && "Cstr ptr is null");
	assert(str && "Str ptr is null");

	const size_t len = strlen(str);
	char* cpy_str = BLEMM_MALLOC(len * sizeof(char));
	if (!str)
		assert(0 && "Malloc failed!");

	BLEMM_MEMCPY((void*)cpy_str, str, len * sizeof(char));
	cstr->items = cpy_str;
	cstr->count = len;
}

void blemm_join_cstr(blemm_cstr_t* dest, blemm_cstr_t* src_list, size_t src_list_size)
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

	dest->items = BLEMM_MALLOC(needed_size * sizeof(char));
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
	blemm_cpy_to_cstr(&str, arg);
	BLEMM_DAR_AP(cmd, str);
}

bool blemm_exec_cmd_sync(blemm_cmd_t* cmd)
{
	assert(cmd && "Trying to execute null cmd");

	blemm_cstr_builder_t builder = {0};

	BLEMM_LOGT("Executing command: ");
	for (size_t i = 0; i < cmd->count; i++)
	{
		BLEMM_LOGT("%s", cmd->items[i].items);
	}

	blemm_cstr_t cstr = {0};
	blemm_join_cstr(&cstr, cmd->items, cmd->items->count);
	BLEMM_LOGT("joined cmd: %s ", cstr.items);

	BLEMM_LOGI("--- CMD -> STDOUT ---");
	const int ret = system(cstr.items);
	BLEMM_LOGI("--- CMD x> STDOUT ---");
	const bool failed = (ret != 0);
	if (failed)
		BLEMM_LOGE("System cmd returned non-zero: %d", ret);

	free(cstr.items);
	return failed;
}

#endif // BLEMM_IMPL

#endif // BLEMM_H
