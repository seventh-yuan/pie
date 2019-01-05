/**
 * This is the difinition of iic
 * 
 * Author: Arno
 *
 * Created 2018/07/23
 */
#ifndef __PIN_H__
#define __PIN_H__
#include <common/piedef.h>

#ifdef __cplusplus
extern "C"{
#endif


typedef enum{
    PIN_DIR_OUTPUT,
    PIN_DIR_INPUT,
}pin_dir_t;

typedef enum{
    PIN_LEVEL_LOW,
    PIN_LEVEL_HIGH,
}pin_level_t;

typedef struct pin {
    int8_t inited;
    const char* dev_name;
    list_t node;
    const struct pin_operations *ops;
} pin_t;

struct pin_operations{
    int (*init)(pin_t* pin);
    int (*set_direction)(pin_t* pin, int index, pin_dir_t dir);
    int (*get_direction)(pin_t* pin, int index, pin_dir_t *dir);
    int (*get_output)(pin_t* pin, int index, pin_level_t *level);
    int (*set_level)(pin_t* pin, int index, pin_level_t level);
    int (*get_level)(pin_t* pin, int index, pin_level_t *level);
};

int pin_register(const char* dev_name, pin_t* pin);

pin_t* pin_open(const char* dev_name);

void pin_close(pin_t* pin);

int pin_set_direction(pin_t* pin, int index, pin_dir_t dir);

int pin_get_direction(pin_t* pin, int index);

int pin_get_output(pin_t* pin, int index);

int pin_set_level(pin_t* pin, int index, pin_level_t level);

int pin_get_level(pin_t* pin, int index);

#ifdef __cplusplus
}
#endif

#endif