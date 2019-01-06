#include <string.h>
#include <bus/i2c.h>
#include <chip/at24cxx.h>

struct at24cxx_info {
    at24cxx_chip_t chip_type;
    uint8_t page_size;
    uint16_t page_num;
    uint8_t addr_size;
    uint8_t dev_mask;
    uint16_t mdelay;
};

static const struct at24cxx_info at24cxx_info[] = {
    {.chip_type = AT24C01, .page_size = 8, .page_num = 16, .addr_size = 1, .dev_mask = 0, .mdelay = 7},
    {.chip_type = AT24C02, .page_size = 8, .page_num = 32, .addr_size = 1, .dev_mask = 0, .mdelay = 12},
    {.chip_type = AT24C04, .page_size = 16, .page_num = 32, .addr_size = 1, .dev_mask = 0x01, .mdelay = 15},
    {.chip_type = AT24C08, .page_size = 16, .page_num = 64, .addr_size = 1, .dev_mask = 0x03, .mdelay = 15},
    {.chip_type = AT24C16, .page_size = 16, .page_num = 128, .addr_size = 1, .dev_mask = 0x07, .mdelay = 15},
    {.chip_type = AT24C32, .page_size = 32, .page_num = 128, .addr_size = 2, .dev_mask = 0, .mdelay = 15},
    {.chip_type = AT24C64, .page_size = 32, .page_num = 256, .addr_size = 2, .dev_mask = 0, .mdelay = 15},
    {.chip_type = AT24C128, .page_size = 64, .page_num = 256, .addr_size = 2, .dev_mask = 0, .mdelay = 15},
    {.chip_type = AT24C256, .page_size = 64, .page_num = 512, .addr_size = 2, .dev_mask = 0, .mdelay = 15},
    {.chip_type = AT24C512, .page_size = 128, .page_num = 512, .addr_size = 2, .dev_mask = 0, .mdelay = 15},
};


static struct at24cxx_info* _get_chip_info(at24cxx_chip_t chip_type)
{

    for(int i = 0; i < sizeof(at24cxx_info) / sizeof(at24cxx_info[0]); i ++)
    {
        if(at24cxx_info[i].chip_type == chip_type)
            return (struct at24cxx_info*)(at24cxx_info + i);
    }  

    return NULL;
}

static int at24cxx_write(at24cxx_t* at24cxx, uint32_t address, const uint8_t* wr_data, size_t len)
{
    ASSERT(at24cxx);
    ASSERT(address >= 0);
    ASSERT(wr_data);
    ASSERT(len > 0);

    struct at24cxx_info* chip_info = _get_chip_info(at24cxx->chip_type);

    ASSERT(chip_info);
    ASSERT(address + len <= chip_info->page_size * chip_info->page_num);

    int length = len;
    int wr_len = 0;

    int addr_flag = I2C_BYTE_ADDRESS;
    if(chip_info->addr_size == 2)
        addr_flag = I2C_WORD_ADDRESS;

    while(length > 0)
    {
        uint8_t dev_addr = at24cxx->dev_addr | ((address >> 8) & chip_info->dev_mask);

        if((address & (chip_info->page_size - 1)) + length > chip_info->page_size)
            wr_len = chip_info->page_size - (address & (chip_info->page_size - 1));
        else
            wr_len = length;

        if(i2c_address_write(at24cxx->i2c, dev_addr, address, addr_flag, wr_data, wr_len) < 0)
            break;
        length -= wr_len;
        wr_data += wr_len;
        address += wr_len;
        sleep_ms(chip_info->mdelay);
    }

    return (len - length);
}

static int at24cxx_read(at24cxx_t* at24cxx, uint32_t address, uint8_t* rd_data, size_t len)
{
    ASSERT(at24cxx);
    ASSERT(address >= 0);
    ASSERT(rd_data);
    ASSERT(len > 0);

    struct at24cxx_info* chip_info = _get_chip_info(at24cxx->chip_type);

    ASSERT(chip_info);
    ASSERT(address + len <= chip_info->page_size * chip_info->page_num);


    int length = len;
    int rd_len = 0;

    int addr_flag = I2C_BYTE_ADDRESS;
    if(chip_info->addr_size == 2)
        addr_flag = I2C_WORD_ADDRESS;

    while(length > 0)
    {
        uint8_t dev_addr = at24cxx->dev_addr | ((address >> 8) & chip_info->dev_mask);

        if((address & (chip_info->page_size - 1)) + length > chip_info->page_size)
            rd_len = chip_info->page_size - (address & (chip_info->page_size - 1));
        else
            rd_len = length;
        if(i2c_address_read(at24cxx->i2c, dev_addr, address, addr_flag, rd_data, rd_len) < 0)
            break;
        length -= rd_len;
        rd_data += rd_len;
        address += rd_len;
    }

    return (len - length);
}


static int at24cxx_eeprom_write(eeprom_t* eeprom, uint32_t address, const uint8_t* wr_data, size_t wr_len)
{
    ASSERT(eeprom);
    ASSERT(wr_data);

    at24cxx_t* at24cxx = eeprom->private_data;

    return at24cxx_write(at24cxx, address, wr_data, wr_len);
}

static int at24cxx_eeprom_read(eeprom_t* eeprom, uint32_t address, uint8_t* rd_data, size_t rd_len)
{
    ASSERT(eeprom);
    ASSERT(rd_data);

    at24cxx_t* at24cxx = eeprom->private_data;

    return at24cxx_read(at24cxx, address, rd_data, rd_len);
}

const eeprom_ops_t at24cxx_ops = {
    .write = at24cxx_eeprom_write,
    .read = at24cxx_eeprom_read,
};


void at24cxx_init(eeprom_t* eeprom, at24cxx_t* at24cxx, i2c_t* i2c, uint16_t dev_addr, at24cxx_chip_t chip_type)
{
    ASSERT(eeprom);
    ASSERT(at24cxx);
    ASSERT(i2c);

    at24cxx->i2c = i2c;
    at24cxx->dev_addr = dev_addr;
    at24cxx->chip_type = chip_type;
    eeprom->private_data = at24cxx;
    eeprom->ops = &at24cxx_ops;
}