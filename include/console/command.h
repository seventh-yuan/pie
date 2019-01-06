/**
 * This is the difinition of command
 * 
 * Author: Arno
 *
 * Created 2018/07/23
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__
#include <common/pie.h>


#ifdef __cplusplus
extern "C"{
#endif

enum{
    ERRNO_SUCCESS = 0,
    ERRNO_PARAMETER = 1,
    ERRNO_EXECUTE = 2,
    ERRNO_NO_COMMAND = 3,
};

#define COMMAND_NEWLINE "\r\n"

typedef int (*command_handle_t)(device_t *stdio, char* params[], int nparam);

typedef struct command {
    const char * const name;
    const command_handle_t handle;
    const char * const brief;
    void *next;
} command_t;

int command_return_success(device_t *stdio, const char *message);

int command_return_error(device_t *stdio, int errno, const char *message);

int command_register(command_t *commands, int num);

int command_parse_parameter(char* paramstr, char* params[], int param_num, char delim);

int command_parse_message(char* cmdstr, char** paramstr);

int command_execute(device_t *stdio, char* cmdstr, char* params[], int nparam);

int command_count_parameter(const char *cmdstr);

const char *command_get_parameter(const char *cmdstr, int param_index, size_t *param_len);

#ifdef __cplusplus
}
#endif

#endif
