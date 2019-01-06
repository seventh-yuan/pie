/**
  *******************************************************
  *@file        console.c
  *@author      Arno
  *@version     V1.0.0
  *@date        2018/7/23
  *@brief       console  module.
  *             This is the kernel part of ecode.
  *******************************************************
  */
/* Includes----------------------------------------------*/
#include <string.h>
#include <common/pie.h>
#include <console/console.h>
#include <console/command.h>

#define CONSOLE_PROMOT      ">>"

/*Private variables--------------------------------------*/
/*Public variables---------------------------------------*/
/*Private functions--------------------------------------*/
/*Public functions---------------------------------------*/

int console_readline(console_t* console)
{
    int ch=0;
    while((ch = stdio_fgetc(console->stdio)) != -1)
    {
        stdio_fputc(console->stdio, ch);
        if(ch == '\r' || ch == '\n')
        {
            CONSOLE_WRITE(console->stdio, (const uint8_t*)KERNEL_NL, strlen(KERNEL_NL));
            console->rxbuf[console->pos] = '\0';
            console->newline = 1;
            return 0;
        }
        if('\b' == ch)
        {
            if(console->pos > 0)
            {
                console->pos --;
                stdio_fputc(console->stdio, ' ');
                stdio_fputc(console->stdio, '\b');
            }
            continue;
        }
        else
        {
            if((ch >= ' ') && (ch <= '~'))
            {
                if(console->pos < console->bufsz)
                    console->rxbuf[console->pos ++] = ch;
                else
                {
                    pie_print("Console buffer full.");
                    console->pos = 0;
                    return -1;
                }
            }
        }
    }
    return -1;
}

void console_process(console_t* console)
{
    char* paramstr;
    if(command_parse_message(console->rxbuf, &paramstr) == 0)
    {
        console->nparam = command_parse_parameter(paramstr, console->params, CONFIG_MAX_PARAMS, ',');
        command_execute(console->stdio, console->rxbuf, console->params, console->nparam);
    }
}

void console_thread(const void* args)
{
    console_t* console = (console_t*)args;

    for(;;)
    {
        int ret = console_readline(console);
        if(ret != -1 && console->pos > 0)
        {
            console_process(console);
            console->pos = 0;
        }
        if(console->newline)
        {
            console->newline = 0;
            CONSOLE_WRITE(console->stdio, (const uint8_t*)CONSOLE_PROMOT, strlen(CONSOLE_PROMOT));
        }
    }
}


int console_server_start(console_t* console, char* rxbuf, size_t bufsz, size_t stacksz)
{
    ASSERT(console);
    ASSERT(rxbuf);
    ASSERT(bufsz > 0);
    ASSERT(stacksz > 0);

    console->newline = 1;
    console->pos = 0;
    console->rxbuf = rxbuf;
    console->bufsz = bufsz;

    osThreadDef(console, console_thread, osPriorityNormal, 0, stacksz);
    osThreadCreate(osThread(console), console);

    return 0;
}