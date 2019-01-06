#ifndef __W25QXX_H__
#define __W25QXX_H__
#include <common/pie.h>
#include <bus/spi.h>
#include <bus/pin.h>


#define MODULE_W25QXX(n)      MODULE_DEFINE(w25qxx, n)
#define IMPORT_W25QXX(n)      IMPORT_MODULE(w25qxx, n)
#define W25QXX(n)             MODULE(w25qxx, n)


typedef struct w25qxx {
    int cs_pin;
    pin_t* pin;
    spi_t* spi;
} w25qxx_t;

void w25qxx_init(w25qxx_t* w25qxx, spi_t* spi, pin_t* pin, int cs_pin);

int w25qxx_sector_erase(w25qxx_t* w25qxx, uint32_t address);

int w25qxx_block32_erase(w25qxx_t* w25qxx, uint32_t address);

int w25qxx_block64_erase(w25qxx_t* w25qxx, uint32_t address);

int w25qxx_write(w25qxx_t* w25qxx, uint32_t address, uint8_t* wr_data, size_t wr_len);

int w25qxx_read(w25qxx_t* w25qxx, uint32_t address, uint8_t*rd_data, size_t rd_len);

int w25qxx_read_manufacturer_device_id(w25qxx_t* w25qxx);

#endif