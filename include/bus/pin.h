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

#define MODULE_PIN(n)      MODULE_DEFINE(pin, n)
#define IMPORT_PIN(n)      IMPORT_MODULE(pin, n)
#define PIN(n)             MODULE(pin, n)

typedef enum{
    PIN_DIR_OUTPUT,
    PIN_DIR_INPUT,
}pin_dir_t;

typedef enum{
    PIN_LEVEL_LOW,
    PIN_LEVEL_HIGH,
}pin_level_t;

typedef struct pin {
    void* private_data;
    const struct pin_operations *ops;
} pin_t;

typedef struct pin_operations{
    int (*set_direction)(pin_t* pin, int index, pin_dir_t dir);
    int (*get_direction)(pin_t* pin, int index, pin_dir_t *dir);
    int (*get_output)(pin_t* pin, int index, pin_level_t *level);
    int (*set_level)(pin_t* pin, int index, pin_level_t level);
    int (*get_level)(pin_t* pin, int index, pin_level_t *level);
} pin_ops_t;

void pin_init(pin_t* pin, const pin_ops_t* ops);

int pin_set_direction(pin_t* pin, int index, pin_dir_t dir);

int pin_get_direction(pin_t* pin, int index);

int pin_get_output(pin_t* pin, int index);

int pin_set_level(pin_t* pin, int index, pin_level_t level);

int pin_get_level(pin_t* pin, int index);

#ifdef __cplusplus
}
#endif

#endif