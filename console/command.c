/**
  *******************************************************
  *@file        command.c
  *@author      Arno
  *@version     V1.0.0
  *@date        2018/7/23
  *@brief       command  module.
  *             This is the kernel part of ecode.
  *******************************************************
  */
/* Includes----------------------------------------------*/
#include <common/pie.h>
#include <console/command.h>
#include <console/console.h>
#include <string.h>

/*Private variables--------------------------------------*/
static int help_handle(device_t *stdio, char* params[], int nparam);

static struct command _help_command = {
    .name = "help",
    .brief = "\r\nhelp()\r\n Lists all registered commands.\r\n",
    .handle = help_handle,
    .next = NULL,
};

static struct command *register_commands = &_help_command;

const char *_errno_reason[]={
    "Success",
    "Parameter error",
    "Execute failure",
    "No command",
};

/*Public variables---------------------------------------*/
static int help_handle(device_t *stdio, char* params[], int nparam)
{
    struct command *pcommand = NULL;
    
    for(pcommand = register_commands; pcommand!=NULL; pcommand = pcommand->next)
    {
        int ret = CONSOLE_WRITE(stdio, pcommand->brief, strlen(pcommand->brief));
        
        if(ret < 0)
            return command_return_error(stdio, ERRNO_EXECUTE, "execute failed.");
    }
    
    return command_return_success(stdio, NULL);
} 

/*Private functions--------------------------------------*/
/*Public functions---------------------------------------*/

static const char * command_get_error_reason(int errno)
{
    return _errno_reason[errno];
}

int command_return_success(device_t *stdio, const char *message)
{
    if(message == NULL)
        return 0;
    CONSOLE_WRITE(stdio, message, strlen(message));
    CONSOLE_WRITE(stdio, KERNEL_NL, strlen(KERNEL_NL));
    return 0;
}

int command_return_error(device_t *stdio, int errno, const char *message)
{
    int ret = 0;
    const char *error_reason = command_get_error_reason(errno);
    ret += CONSOLE_WRITE(stdio, error_reason, strlen(error_reason));
    ret += stdio_fputc(stdio, ':');
    ret += CONSOLE_WRITE(stdio, message, strlen(message));
    CONSOLE_WRITE(stdio, KERNEL_NL, strlen(KERNEL_NL));
    return ret;
}


int command_register(struct command *commands, int num)
{
    struct command* command = commands;

    for(int i=0; i < num; i++)
    {
        command->next = register_commands;
        register_commands = command;
        command ++;
    }
    
    return 0;
}

int command_parse_parameter(char* paramstr, char* params[], int param_num, char delim)
{
    ASSERT(paramstr);
    ASSERT(params);

    int count = 0;

    while(count < param_num)
    {
        while(' ' == *paramstr)
            paramstr ++;

        if('\0' == *paramstr)
            return count;

        params[count ++] = paramstr;
        char* ptmp = strchr(paramstr, delim);

        if(NULL == ptmp)
            return count;

        if(count < param_num)
        {
            paramstr = ptmp;
            *paramstr = '\0';
            paramstr ++;
        }
    }

    return count;
}

int command_parse_message(char* cmdstr, char** paramstr)
{
    char* end = strchr(cmdstr, ')');

    if(end != NULL)
        *end = '\0';
    else
    {
        pie_print("Can not find command end ')'.");
        return -1;
    }

    char* stemp = strchr(cmdstr, '(');
    if(stemp == NULL)
    {
        pie_print("Can not find command parameter start ')'.");
        return -1;
    }
    *stemp = '\0';

    stemp ++;

    *paramstr = stemp;

    return 0;
}

int command_execute(device_t *stdio, char* cmdstr, char* params[], int nparam)
{
    struct command *command;
    size_t cmdstr_len = 0;
    
    
    for(command = register_commands; command != NULL; command = command->next)
    {
        cmdstr_len = strlen(command->name);
        if((cmdstr[cmdstr_len] == ' ') || (cmdstr[cmdstr_len] == 0x00))
        {
            if(strncmp(cmdstr, command->name, cmdstr_len) == 0)
            {
                break;
            }
        }
    }
    
    if(command != NULL)
    {
       return command->handle(stdio, params, nparam);
    }
    
    return command_return_error(stdio, ERRNO_NO_COMMAND, "command not registered. Enter 'help' to view a list of available commands");
}

const char *command_find_parameter(const char *cmdstr, size_t *param_len)
{
    const char *param_str = NULL;
    *param_len = 0;
    while((*cmdstr!=0x00)&&(*cmdstr==' '))
        cmdstr ++;
    param_str = cmdstr;
    while((*cmdstr!=0x00)&&((*cmdstr)!=' '))
    {
        (*param_len) ++;
        cmdstr ++;
    }
    return param_str;
}

int command_count_parameter(const char *cmdstr)
{
    int param_count = 0;
    int param_len = 0;
    while((cmdstr = command_find_parameter(cmdstr+param_len, (size_t*)&param_len))!=NULL)
    {
        if(param_len==0)
            return -1;
        param_count ++;
    }
    return param_count-1;
}

const char *command_get_parameter(const char *cmdstr, int param_index, size_t *param_len)
{
    int param_count = 0;
    cmdstr = command_find_parameter(cmdstr, param_len);
    
    while((cmdstr = command_find_parameter(cmdstr + *param_len, param_len))!=NULL)
    {
        if(param_len==0)
            return NULL;
        param_count ++;
        if(param_count == param_index)
            return cmdstr;
    }
    return NULL;
}

