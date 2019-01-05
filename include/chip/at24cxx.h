#ifndef __AT24CXX_H__
#define __AT24CXX_H__
#include <common/pie.h>
#include <bus/i2c.h>
#include <chip/eeprom.h>

typedef enum {
    AT24C01,
    AT24C02,
    AT24C04,
    AT24C08,
    AT24C16,
    AT24C32,
    AT24C64,
    AT24C128,
    AT24C256,
    AT24C512,
} at24cxx_chip_t;

typedef struct at24cxx {
    eeprom_t eeprom;
    const uint16_t dev_addr;
    const at24cxx_chip_t chip_type;
    i2c_t* bus;
} at24cxx_t;



void at24cxx_attach_eeprom(at24cxx_t* at24cxx);

#endif