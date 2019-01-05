#ifndef __MEM_H__
#define __MEM_H__
#include <common/piedef.h>

#ifdef __cplusplus
extern "C"{
#endif

void *mem_alloc(size_t size);

void *mem_realloc(void *ptr, size_t size);

void mem_free(void *ptr);
#ifdef __cplusplus
}
#endif
#endif