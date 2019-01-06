#ifndef __UTILITY_H__
#define __UTILITY_H__
#include <common/piecfg.h>
#include <common/piedef.h>
#ifdef __cplusplus
extern "C"{
#endif

#if CONFIG_PIE_USING_ASSERT == 1
#define ASSERT(expr)     if(!(expr))\
                                do{\
                                    fprintf(stderr, "file %s: line%d: function %s: Assertion '%s' failed\n",\
                                    __FILE__, __LINE__, __FUNCTION__, #expr);\
                                    while(1);\
                                }while(0)
#else
#define ASSERT(expr)     ((void)0) 
#endif

void stdio_register(device_t* device);

int stdio_fputc(device_t* device, int ch);

int stdio_fgetc(device_t* device);

int stdio_putc(int ch);

int stdio_getc(void);

int pie_print(const char* fmt, ...);

#ifdef __cplusplus
}
#endif
#endif