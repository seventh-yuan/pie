#ifndef __SDCARD_H__
#define __SDCARD_H__
#include <common/pie.h>
#include <bus/pin.h>
#include <bus/spi.h>

#define MODULE_SDCARD(n)      MODULE_DEFINE(sdcard, n)
#define IMPORT_SDCARD(n)      IMPORT_MODULE(sdcard, n)
#define SDCARD(n)             MODULE(sdcard, n)

typedef struct sdcard {
    int cs_pin;
    int type;
    pin_t* pin;
    spi_t* spi;
} sdcard_t;

int sd_init(sdcard_t* sd, spi_t* spi, pin_t* pin, int cs_pin);

int sd_deselect(sdcard_t* sd);

int sd_select(sdcard_t* sd);

int sd_get_cid(sdcard_t* sd, uint8_t* cid);

int sd_get_csd(sdcard_t* sd, uint8_t*csd);

int sd_get_sector_count(struct  sdcard* sd);

int sd_initialize(sdcard_t* sd);

int sd_read_disk(sdcard_t* sd, uint32_t sector, uint8_t* rd_data, size_t nsec);

int sd_write_disk(sdcard_t* sd, uint32_t sector, uint8_t* wr_data, size_t nsec);

#endif