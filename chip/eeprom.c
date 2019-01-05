#include <chip/eeprom.h>

int eeprom_write(eeprom_t* eeprom, uint32_t addr, const uint8_t* wr_data, size_t wr_len)
{
    ASSERT(eeprom);
    ASSERT(wr_data);
    ASSERT(eeprom->write);

    return eeprom->write(eeprom, addr, wr_data, wr_len);
}

int eeprom_read(eeprom_t* eeprom, uint32_t addr, uint8_t* rd_data, size_t rd_len)
{
    ASSERT(eeprom);
    ASSERT(rd_data);
    ASSERT(eeprom->read);

    return eeprom->read(eeprom, addr, rd_data, rd_len);
}
