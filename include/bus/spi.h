/**
 * This is the difinition of spi
 *
 * Author: Arno
 *
 * Created 2018/07/28
 */
#ifndef __SPI_H__
#define __SPI_H__
#include <common/pie.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
} spi_mode_t;

typedef struct spi {
    int8_t inited;
    const char* dev_name;
    list_t node;
    const struct spi_operations *ops;
} spi_t;

struct spi_operations{
    int (*init)(spi_t* spi);
    int (*set_speed)(spi_t* spi, uint32_t speed);
    int (*set_mode)(spi_t* spi, spi_mode_t mode);
    int (*write)(spi_t* spi, const uint8_t* wr_data, size_t wr_len);
    int (*read)(spi_t* spi, uint8_t* rd_data, size_t rd_len);
    int (*transfer)(spi_t* spi, const uint8_t *wr_data, uint8_t *rd_data, size_t len);
};


int spi_register(const char* dev_name, spi_t* spi);

spi_t* spi_open(const char* dev_name);

void spi_close(spi_t* spi);

int spi_set_mode(spi_t* spi, spi_mode_t mode);

int spi_set_speed(spi_t* spi, uint32_t speed);
/**
  * @brief This function is used to write data to spi.
  * @param spi: spi bus pointer.
  * @param wr_data: data to write.
  * @param wr_len: length of data to write.
  * @retval 0 for success, otherwise -1.
  */
int spi_write(spi_t* spi, const uint8_t *wr_data, size_t wr_len);

/**
  * @brief This function is used to read data from spi.
  * @param spi: spi bus pointer.
  * @param rd_data: data read.
  * @param rd_len: length of data to read.
  * @retval 0 for success, otherwise -1.
  */
int spi_read(spi_t* spi, uint8_t *rd_data, size_t rd_len);

/**
  * @brief This function is used to read data from spi.
  * @param spi: spi bus pointer.
  * @param wr_data: data to write.
  * @param rd_data: data read.
  * @param len: length of data to write and read.
  * @retval 0 for success, otherwise -1.
  */
int spi_transfer(spi_t* spi, const uint8_t *wr_data, uint8_t *rd_data, size_t len);

#ifdef __cplusplus
}
#endif
#endif
