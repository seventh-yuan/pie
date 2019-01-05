#ifndef __SDCARD_H__
#define __SDCARD_H__
#include <common/pie.h>
#include <bus/pin.h>
#include <bus/spi.h>


struct sdcard{
    int cs_pin;
    int type;
    struct pin* pin;
    struct spi* spi;
    struct sddcard* (*open)(int index);
};

int sd_init(struct sdcard* sd);

int sd_deselect(struct sdcard* sd);

int sd_select(struct sdcard* sd);

int sd_get_cid(struct sdcard* sd, uint8_t* cid);

int sd_get_csd(struct sdcard* sd, uint8_t*csd);

int sd_get_sector_count(struct  sdcard* sd);

int sd_initialize(struct sdcard* sd);

int sd_read_disk(struct sdcard* sd, uint32_t sector, uint8_t* rd_data, size_t nsec);

int sd_write_disk(struct sdcard* sd, uint32_t sector, uint8_t* wr_data, size_t nsec);

#endif