#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <common/pie.h>


typedef struct eeprom {
    int (*write)(struct eeprom* eeprom, uint32_t addr, const uint8_t* wr_data, size_t wr_len);
    int (*read)(struct eeprom* eeprom, uint32_t addr, uint8_t* rd_data, size_t rd_len);
} eeprom_t;

#define TO_EEPROM(dev)      (&dev->eeprom)

int eeprom_write(eeprom_t* eeprom, uint32_t addr, const uint8_t* wr_data, size_t wr_len);

int eeprom_read(eeprom_t* eeprom, uint32_t addr, uint8_t* rd_data, size_t rd_len);

#endif