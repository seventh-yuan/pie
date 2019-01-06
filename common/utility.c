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

#if CONFIG_PIE_USING_STDIO == 1
MODULE_STDIO() = {.ops = NULL};
#endif

#ifndef CONFIG_PIE_PRINT_BUF_SIZE
#define CONFIG_PIE_PRINT_BUF_SIZE    100
#endif

static char stdio_buffer[CONFIG_PIE_PRINT_BUF_SIZE];


void stdio_set_port(void* port, const device_ops_t* ops)
{
    STDIO()->private_data = port;
    STDIO()->ops = ops;
}

int stdio_fputc(device_t* device, int ch)
{
    ASSERT(device);

    return device_write(device, (const uint8_t*)&ch, 1);
}

int stdio_fgetc(device_t* device)
{
    ASSERT(device);

    int ch = 0;
    int ret = 0;

    if ((ret = device_read(device, (uint8_t*)&ch, 1)) <= 0)
        return -1;

    return ch;
}

int stdio_putc(int ch)
{
#if CONFIG_PIE_USING_STDIO == 1
    if (STDIO()->ops)
        return device_write(STDIO(), (const uint8_t*)&ch, 1);
#endif
    return 0;
}

int stdio_getc(void)
{
    int ch = 0;
#if CONFIG_PIE_USING_STDIO == 1
    if (STDIO()->ops)
        return device_read(STDIO(), (uint8_t*)&ch, 1);
#endif
    return 0;
}

int pie_print(const char* fmt, ...)
{
#if CONFIG_PIE_USING_STDIO == 1
    int ret = 0;
    va_list args;
    va_start(args, fmt);
    ret = vsnprintf((char*)stdio_buffer, sizeof(stdio_buffer) - 1, fmt, args);
    va_end(args);

    if (STDIO()->ops)
        device_write(STDIO(), (const uint8_t*)stdio_buffer, ret);
#endif
    return 0;
}


PUTCHAR_PROTOTYPE
{
    stdio_putc(ch);
    return ch;
}