#ifndef __ILI93XX_H__
#define __ILI93XX_H__
#include <common/pie.h>

#define MODULE_ILI93XX(n)      MODULE_DEFINE(ili93xx, n)
#define IMPORT_ILI93XX(n)      IMPORT_MODULE(ili93xx, n)
#define ILI93XX(n)             MODULE(ili93xx, n)

typedef struct ili93xx_operations{
    void (*write_reg)(uint16_t value);
    void (*write_data)(uint16_t value);
    uint16_t (*read_data)(void);
    void (*backlight_control)(int state);
} ili93xx_ops_t;

typedef struct ili93xx {
    int dir;
    uint16_t width;
    uint16_t height;
    uint16_t setxcmd;
    uint16_t setycmd;
    uint16_t wramcmd;
    const ili93xx_ops_t* ops;
} ili93xx_t;

void ili93xx_init(ili93xx_t* ili93xx, const ili93xx_ops_t* ops);

void ili93xx_write_reg(ili93xx_t* ili93xx, uint16_t value);

void ili93xx_write_data(ili93xx_t* ili93xx, uint16_t value);

uint16_t ili93xx_read_data(ili93xx_t* ili93xx);

void ili93xx_write_register(ili93xx_t* ili93xx, uint16_t reg, uint16_t value);

uint16_t ili93xx_read_register(ili93xx_t* ili93xx, uint16_t reg);

void ili93xx_writeram_prepare(ili93xx_t* ili93xx);

void ili93xx_set_cursor(ili93xx_t* ili93xx, uint16_t x, uint16_t y);

void ili93xx_fill(ili93xx_t* ili93xx, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

void ili93xx_set_dir(ili93xx_t* ili93xx, uint8_t dir);

#endif