#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <common/pie.h>

#define MODULE_EEPROM(n)      MODULE_DEFINE(eeprom, n)
#define IMPORT_EEPROM(n)      IMPORT_MODULE(eeprom, n)
#define EEPROM(n)             MODULE(eeprom, n)


typedef struct eeprom {
    void* private_data;
    const struct eeprom_operations* ops;
} eeprom_t;

typedef struct eeprom_operations {
    int (*write)(struct eeprom* eeprom, uint32_t addr, const uint8_t* wr_data, size_t wr_len);
    int (*read)(struct eeprom* eeprom, uint32_t addr, uint8_t* rd_data, size_t rd_len);
} eeprom_ops_t;

void eeprom_init(eeprom_t* eeprom, const eeprom_ops_t* ops);

int eeprom_write(eeprom_t* eeprom, uint32_t addr, const uint8_t* wr_data, size_t wr_len);

int eeprom_read(eeprom_t* eeprom, uint32_t addr, uint8_t* rd_data, size_t rd_len);

#endif