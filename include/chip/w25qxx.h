#ifndef __W25QXX_H__
#define __W25QXX_H__
#include <common/pie.h>
#include <bus/spi.h>
#include <bus/pin.h>


typedef struct w25qxx {
    int cs_pin;
    pin_t* pin;
    spi_t* spi;
} w25qxx_t;

int w25qxx_sector_erase(w25qxx_t* w25qxx, uint32_t address);

int w25qxx_block32_erase(w25qxx_t* w25qxx, uint32_t address);

int w25qxx_block64_erase(w25qxx_t* w25qxx, uint32_t address);

int w25qxx_write(w25qxx_t* w25qxx, uint32_t address, uint8_t* wr_data, size_t wr_len);

int w25qxx_read(w25qxx_t* w25qxx, uint32_t address, uint8_t*rd_data, size_t rd_len);

int w25qxx_read_manufacturer_device_id(w25qxx_t* w25qxx);

#endif