/**
 * This is the difinition of console
 * 
 * Author: Arno
 *
 * Created 2018/09/17
 */
#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include <common/pie.h>

#ifndef CONFIG_MAX_PARAMS
#define CONFIG_MAX_PARAMS   50
#endif

#define MODULE_CONSOLE(n)      MODULE_DEFINE(console, n)
#define IMPORT_CONSOLE(n)      IMPORT_MODULE(console, n)
#define CONSOLE(n)             MODULE(console, n)


typedef struct console {
    device_t* stdio;
    int newline;
    int pos;
    char* rxbuf;
    int bufsz;
    char* params[CONFIG_MAX_PARAMS];
    int nparam;
} console_t;

#define CONSOLE_WRITE(console, wr_data, wr_len)         device_write(console, (const uint8_t*)wr_data, wr_len)
#define CONSOLE_READ(console, rd_data, rd_len)          device_read(console, (uint8_t*)rd_data, rd_len)

int console_server_start(console_t* console, char* rxbuf, size_t bufsz, size_t stacksz);
#endif