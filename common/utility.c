#include <common/pie.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/*Macro variables----------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_fputchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static device_t* stdio = NULL;

#ifndef CONFIG_PIE_PRINT_BUF_SIZE
#define CONFIG_PIE_PRINT_BUF_SIZE    100
#endif

static char stdio_buffer[CONFIG_PIE_PRINT_BUF_SIZE];

void stdio_register(device_t* device)
{
    ASSERT(device);

    stdio = device;
}

int stdio_putc(int ch)
{
    if (stdio)
        return device_write(stdio, (const uint8_t*)&ch, 1);

    return 0;
}

int stdio_getc(void)
{
    int ch = 0;

    if (stdio)
        return device_read(stdio, (uint8_t*)&ch, 1);

    return 0;
}

int pie_print(const char* fmt, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, fmt);
    ret = vsnprintf((char*)stdio_buffer, sizeof(stdio_buffer) - 1, fmt, args);
    va_end(args);

    return device_write(stdio, (const char*)stdio_buffer, ret);
}


PUTCHAR_PROTOTYPE
{
    stdio_putc(ch);
    return ch;
}