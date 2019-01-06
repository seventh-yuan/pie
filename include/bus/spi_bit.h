/**
 * This is the difinition of spi
 *
 * Author: Arno
 *
 * Created 2018/09/19
 */
#ifndef __SPI_BIT_H__
#define __SPI_BIT_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <common/pie.h>
#include <bus/spi.h>

typedef struct spi_bit {
    spi_mode_t mode;
    int delay_us;
    const struct spi_bit_operations *ops;
} spi_bit_t;

typedef struct spi_bit_operations {
    int (*mosi_set)(int state);
    int (*miso_get)(void);
    int (*sck_set)(int state);
    void (*udelay)(int us);
} spi_bit_ops_t;

void spi_bit_init(spi_t* spi, spi_bit_t* spi_bit);

#ifdef __cplusplus
}
#endif
#endif