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
void blemm_append_arg(blemm_cmd_t* cmd, const char* arg);
bool blemm_exec_cmd_sync(blemm_cmd_t* cmd);

#define CMD(name) blemm_cmd_t name = {0}
#define CMD_APPEND(cmd, arg) blemm_append_arg(&cmd, arg) 
#define CMD_EXEC_SYNC(cmd) blemm_exec_cmd_sync(&cmd)

#define BLEMM_LOGT(fmt, ...) printf("[TRACE] "fmt"\n", ## __VA_ARGS__)
#define BLEMM_LOGI(fmt, ...) printf("[INFO] "fmt"\n", ## __VA_ARGS__)
#define BLEMM_LOGW(fmt, ...) printf("[WARN] "fmt"\n", ## __VA_ARGS__)
#define BLEMM_LOGE(fmt, ...) printf("[ERROR] "fmt"\n", ## __VA_ARGS__)
#define BLEMM_LOGF(fmt, ...) printf("[FATAL] "fmt"\n", ## __VA_ARGS__)

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
		(array)->items = BLEMM_MALLOC(BLEMM_DAR_INIT_SIZE * sizeof(item));			\
	}												\
	if ((array)->count++ >= (array)->capacity) 							\
	{  												\
		(array)->capacity += BLEMM_DAR_CHUNK_SIZE;  						\
		(array)->items = BLEMM_REALLOC((array)->items, (array)->capacity * sizeof(item));	\
	}       											\
	(array)->items[(array)->count] = item;       							\

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

void blemm_append_arg(blemm_cmd_t* cmd, const char* arg)
{
	blemm_cstr_t str = {0};
	blemm_cpy_to_cstr(&str, arg);
	BLEMM_DAR_AP(cmd, str);
}

bool blemm_exec_cmd_sync(blemm_cmd_t* cmd)
{
	assert(0 && "not implemented");
	blemm_cstr_builder_t builder = {0};
	
}

#endif // BLEMM_IMPL

#endif // BLEMM_H
