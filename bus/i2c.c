#include <string.h>
#include <bus/i2c.h>

LIST_HEAD(i2c_list_head);
//#define I2C_DEBUG

#ifdef I2C_DEBUG
#define I2C_DBG(fmt, ...)   print(fmt, ##__VA_ARGS__)
#else
#define I2C_DBG(fmt, ...)
#endif


static int i2c_start(i2c_t* i2c)
{
    return i2c->ops->start(i2c);
}

static int i2c_stop(i2c_t* i2c)
{
    return i2c->ops->stop(i2c);
}

static int i2c_writeb(i2c_t* i2c, uint8_t data, i2c_ack_t *ack)
{
    return i2c->ops->writeb(i2c, data, ack);
}

static int i2c_readb(i2c_t* i2c, uint8_t* data, i2c_ack_t ack)
{
    return i2c->ops->readb(i2c, data, ack);
}


void i2c_init(i2c_t* i2c, const i2c_ops_t* ops)
{
    ASSERT(i2c);
    ASSERT(i2c->ops);

    i2c->ops = ops;
}

/**
  * @brief This function is used to write data to i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be write.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_write(i2c_t* i2c, uint8_t dev_addr, const uint8_t* data, size_t len)
{
    i2c_ack_t ack;
    ASSERT(i2c != NULL);

    if(i2c_start(i2c) < 0)
        goto i2c_error;
    if(i2c_writeb(i2c, (dev_addr<<1)|0x00, &ack)<0 || ack != I2C_ACK)
        goto i2c_error;
    for(int i=0; i<len;i++)
    {
        if(i2c_writeb(i2c, *data ++, &ack) < 0 || ack != I2C_ACK)
            goto i2c_error;
        I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
    }
    i2c_stop(i2c);
    return 0;
i2c_error:
    i2c_stop(i2c);
    I2C_DBG("i2c write failed.");
    return -1;
}
/**
  * @brief This function is used to read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be read.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_read(i2c_t* i2c, uint8_t dev_addr, uint8_t* rd_data, size_t len)
{
    i2c_ack_t ack;
    ASSERT(i2c != NULL);

    if(i2c_start(i2c) < 0)
        goto i2c_error;
    if(i2c_writeb(i2c, (dev_addr<<1)|0x01, &ack)<0 || ack != I2C_ACK)
        goto i2c_error;
    for(int i=0; i<len;i++)
    {
        if(i==len-1)
        {
            if(i2c_readb(i2c, rd_data ++, I2C_NACK)<0)
                goto i2c_error;
            I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
        }
        else
        {
            if(i2c_readb(i2c, rd_data ++, I2C_ACK)<0)
                goto i2c_error;
            I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
        }
    }
    i2c_stop(i2c);
    return 0;
i2c_error:
    i2c_stop(i2c);
    I2C_DBG("i2c read failed.\r\n");
    return -1;
}

/**
  * @brief This function is used to write and read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address
  * @param address: slave device memory or register address.
  * @param flag: address option, 0 for 7 bit address, 1 for 10 bit address.
  * @param wr_data: data to be write.
  * @param wr_len: length of data to be write.
  * @retval 0 for success, otherwise -1.
  */
int i2c_address_write(i2c_t* i2c, uint8_t dev_addr, uint16_t address, uint8_t flag, const uint8_t* wr_data, size_t wr_len)
{
    i2c_ack_t ack;
    ASSERT(i2c != NULL);

    I2C_DBG("i2c_address_write(0x%x, 0x%x, %d, %d)", dev_addr, address, flag, wr_len);
    if(i2c_start(i2c) < 0)
    {
        I2C_DBG("i2c start failed.\r\n");
        goto i2c_error;
    }
    if(i2c_writeb(i2c, (dev_addr<<1)|0x00, &ack)<0 || ack != I2C_ACK)
    {
        I2C_DBG("i2c write dev_addr failed.");
        goto i2c_error;
    }
    I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
    if(flag == I2C_BYTE_ADDRESS)
    {
        if(i2c_writeb(i2c, (uint8_t)address, &ack)<0 || ack != I2C_ACK)
        {
            I2C_DBG("i2c write address failed.\r\n");
            goto i2c_error;
        }
        I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
    }
    else if(flag == I2C_WORD_ADDRESS)
    {
        if(i2c_writeb(i2c, (uint8_t)address, &ack)<0 || ack != I2C_ACK)
            goto i2c_error;
        if(i2c_writeb(i2c, (uint8_t)((address>>8)&0xFF), &ack)<0 || ack != I2C_ACK)
            goto i2c_error;
    }
    for(int i=0; i<wr_len;i++)
    {
        if(i2c_writeb(i2c, *wr_data ++, &ack) < 0 || ack != I2C_ACK)
        {
            I2C_DBG("i2c write data failed.\r\n");
            I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
            goto i2c_error;
        }
        I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
    }
    i2c_stop(i2c);
    return 0;
i2c_error:
    i2c_stop(i2c);
    I2C_DBG("i2c address write failed.\r\n");
    return -1;
}

/**
  * @brief This function is used to write and read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address
  * @param address: slave device memory or register address.
  * @param flag: address option, 0 for 7 bit address, 1 for 10 bit address.
  * @param wr_data: data to be write.
  * @param wr_len: length of data to be read.
  * @retval 0 for success, otherwise -1.
  */
int i2c_address_read(i2c_t* i2c, uint8_t dev_addr, uint16_t address, uint8_t flag, uint8_t* rd_data, size_t rd_len)
{
    i2c_ack_t ack;
    ASSERT(i2c != NULL);

    I2C_DBG("i2c_address_read(0x%x, 0x%x, %d, %d)\r\n", dev_addr, address, flag, rd_len);
    I2C_DBG("i2c start...\r\n");
    if(i2c_start(i2c) < 0)
    {
        I2C_DBG("i2c address read start failed.\r\n");
        goto i2c_error;
    }
    I2C_DBG("i2c write dev_addr:0x%x\r\n", dev_addr);
    if(i2c_writeb(i2c, (dev_addr<<1)|0x00, &ack)<0 || ack != I2C_ACK)
    {
        I2C_DBG("i2c write dev_addr 0x%02x failed, ack=%d.", dev_addr, ack);
        goto i2c_error;
    }
    I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
    I2C_DBG("i2c write address:0x%x\r\n", address);
    if(flag == I2C_BYTE_ADDRESS)
    {
        if(i2c_writeb(i2c, (uint8_t)address, &ack)<0 || ack != I2C_ACK)
        {
            I2C_DBG("i2c write address 0x%02x failed, ack=%d.\r\n", address, ack);
            goto i2c_error;
        }
        I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
    }
    else if(flag == I2C_WORD_ADDRESS)
    {
        if(i2c_writeb(i2c, address, &ack)<0 || ack != I2C_ACK)
            goto i2c_error;
        if(i2c_writeb(i2c, (address>>8)&0xFF, &ack)<0 || ack != I2C_ACK)
            goto i2c_error;
    }
    I2C_DBG("i2c restart ...\r\n");
    if(i2c_start(i2c) < 0)
    {
        I2C_DBG("i2c restart failed.\r\n");
        goto i2c_error;
    }
    I2C_DBG("i2c write dev_addr:0x%x\r\n", dev_addr);
    if(i2c_writeb(i2c, (dev_addr<<1)|0x01, &ack)<0 || ack != I2C_ACK)
    {
        I2C_DBG("i2c rewrite dev_addr 0x%02x failed, ack=%d.\r\n", dev_addr, ack);
        goto i2c_error;
    }
    I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");

    for(int i=0; i<rd_len;i++)
    {
        if(i == rd_len-1)
        {
            I2C_DBG("i2c readb...\r\n");
            if(i2c_readb(i2c, rd_data ++, I2C_NACK)<0)
                goto i2c_error;
            I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
        }
        else
        {
            if(i2c_readb(i2c, rd_data ++, I2C_ACK)<0)
                goto i2c_error;
            I2C_DBG("%s\r\n", (ack==I2C_ACK)?"ACK":"NACK");
        }
    }
    i2c_stop(i2c);
    return 0;
i2c_error:
    i2c_stop(i2c);
    I2C_DBG("i2c address read failed.\r\n");
    return -1;
}
