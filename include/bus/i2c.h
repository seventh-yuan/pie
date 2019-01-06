/**
 * This is the difinition of i2c
 * 
 * Author: Arno
 *
 * Created 2018/07/23
 */
#ifndef __I2C_H__
#define __I2C_H__
#include <common/pie.h>

#ifdef __cplusplus
extern "C"{
#endif

#define I2C_BYTE_ADDRESS    0
#define I2C_WORD_ADDRESS    1

#define MODULE_I2C(n)      MODULE_DEFINE(i2c, n)
#define IMPORT_I2C(n)      IMPORT_MODULE(i2c, n)
#define I2C(n)             MODULE(i2c, n)

typedef enum {
    I2C_ACK = 0,
    I2C_NACK,
} i2c_ack_t;

typedef struct i2c {
    uint8_t inited;
    list_t node;
    const char* dev_name;
    const struct i2c_operations *ops;
    void* private_data;
} i2c_t;

typedef struct i2c_operations {
    int (*start)(i2c_t* i2c);
    int (*stop)(i2c_t* i2c);
    int (*writeb)(i2c_t* i2c, uint8_t data, i2c_ack_t* ack);
    int (*readb)(i2c_t* i2c, uint8_t* data, i2c_ack_t ack);
} i2c_ops_t;


void i2c_init(i2c_t* i2c, const i2c_ops_t* ops);

/**
  * @brief This function is used to write data to i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be write.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_write(i2c_t* i2c, uint8_t dev_addr, const uint8_t* data, size_t len);

/**
  * @brief This function is used to read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be read.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_read(i2c_t* i2c, uint8_t dev_addr, uint8_t* data, size_t len);

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
int i2c_address_write(i2c_t* i2c, uint8_t dev_addr, uint16_t address, uint8_t flag, const uint8_t* wr_data, size_t wr_len);

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
int i2c_address_read(i2c_t* i2c, uint8_t dev_addr, uint16_t address, uint8_t flag, uint8_t* rd_data, size_t rd_len);

#ifdef __cplusplus
}
#endif
#endif
