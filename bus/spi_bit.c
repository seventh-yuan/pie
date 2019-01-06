#include <common/pie.h>
#include <bus/spi_bit.h>
#include <bus/spi.h>


#ifdef SPI_DEBUG
#define spi_dbg(fmt, ...)   kernel_printf(fmt, ##__VA_ARGS__)
#else
#define spi_dbg(fmt, ...)
#endif

#define sck_set(ops, state) (ops->sck_set(state))
#define mosi_set(ops, state) (ops->mosi_set(state))
#define miso_get(ops)        (ops->miso_get())

#define SCK_H(ops)          sck_set(ops, 1)
#define SCK_L(ops)          sck_set(ops, 0)
#define MOSI_H(ops)          mosi_set(ops, 1)
#define MOSI_L(ops)          mosi_set(ops, 0)
#define MISO_STATE(ops)      miso_get(ops)

pie_inline void spi_udelay(const struct spi_bit_operations *ops, int us)
{
    if(ops->udelay)
        ops->udelay(us);
}

pie_inline spi_bit_t *spi_to_spi_bit(spi_t *spi)
{
    return spi->private_data;
}

pie_inline int spi_read_writeb(spi_bit_t* spi_bit, uint8_t wr_data, spi_mode_t mode)
{
    uint8_t rd_data = 0;

    spi_udelay(spi_bit->ops, spi_bit->delay_us);
    if(mode == SPI_MODE_0)
    {
        // CPOL=0, CPHA=0
        for(int i=0; i < 8; i++)
        {
            if(wr_data & 0x80)
                MOSI_H(spi_bit->ops);
            else
                MOSI_L(spi_bit->ops);
            wr_data <<= 1;
            spi_udelay(spi_bit->ops, spi_bit->delay_us);
            SCK_H(spi_bit->ops);
            spi_udelay(spi_bit->ops, spi_bit->delay_us);
            rd_data <<= 1;
            if(MISO_STATE(spi_bit->ops))
                rd_data |= 1;
            SCK_L(spi_bit->ops);
        }
    }
    else if(mode == SPI_MODE_1)
    {
        //CPOL=0, CPHA=1
        for(int i=0; i < 8; i++)
        {
            SCK_H(spi_bit->ops);

            if(wr_data & 0x80)
                MOSI_H(spi_bit->ops);
            else
                MOSI_L(spi_bit->ops);
            wr_data <<= 1;

            spi_udelay(spi_bit->ops, spi_bit->delay_us);
            rd_data <<= 1;
            if(MISO_STATE(spi_bit->ops))
                rd_data |= 1;

            SCK_L(spi_bit->ops);
            spi_udelay(spi_bit->ops, spi_bit->delay_us);

        }
    }
    else if(mode == SPI_MODE_2)
    {
        //CPOL=1, CPHA=0
        for(int i=0; i < 8; i++)
        {
            if(wr_data & 0x80)
                MOSI_H(spi_bit->ops);
            else
                MOSI_L(spi_bit->ops);
            wr_data <<= 1;
            spi_udelay(spi_bit->ops, spi_bit->delay_us);
            SCK_L(spi_bit->ops);
            spi_udelay(spi_bit->ops, spi_bit->delay_us);
            rd_data <<= 1;
            if(MISO_STATE(spi_bit->ops))
                rd_data |= 1;
            SCK_H(spi_bit->ops);
        }
    }
    else if(mode == SPI_MODE_3)
    {
        //CPOL=1, CPHA=1
        for(int i=0; i < 8; i++)
        {
            SCK_L(spi_bit->ops);

            if(wr_data & 0x80)
                MOSI_H(spi_bit->ops);
            else
                MOSI_L(spi_bit->ops);
            wr_data <<= 1;

            spi_udelay(spi_bit->ops, spi_bit->delay_us);

            SCK_H(spi_bit->ops);

            spi_udelay(spi_bit->ops, spi_bit->delay_us);

            rd_data <<= 1;
            if(MISO_STATE(spi_bit->ops))
                rd_data |= 1;
        }
    }
    spi_udelay(spi_bit->ops, spi_bit->delay_us);
    return rd_data;        
}

static int spi_bit_set_mode(spi_t* spi, spi_mode_t mode)
{
    spi_bit_t* spi_bit = spi_to_spi_bit(spi);

    spi_bit->mode = mode;

    switch (mode)
    {
    case SPI_MODE_0:
    case SPI_MODE_1:
        SCK_L(spi_bit->ops);
        break;
    case SPI_MODE_2:
    case SPI_MODE_3:
        SCK_H(spi_bit->ops);
        break;
    }

    return 0;
}

static int spi_bit_set_speed(spi_t* spi, uint32_t speed)
{
    spi_bit_t* spi_bit = spi_to_spi_bit(spi);

    spi_bit->delay_us = 1000000 / speed / 2;

    return 0;
}

static int spi_bit_write(spi_t* spi, const uint8_t* wr_data, size_t len)
{
    spi_bit_t* spi_bit = spi_to_spi_bit(spi);

    for(int i=0; i < len; i++)
    {
        spi_read_writeb(spi_bit, *wr_data++, spi_bit->mode);
    }

    return len;
}

static int spi_bit_read(spi_t* spi, uint8_t* rd_data, size_t len)
{
    spi_bit_t* spi_bit = spi_to_spi_bit(spi);

    for(int i=0; i < len; i++)
    {
        *rd_data++ = spi_read_writeb(spi_bit, 0xFF, spi_bit->mode);
    }

    return len;
}

static int spi_bit_transfer(spi_t* spi, const uint8_t *wr_data, uint8_t *rd_data, size_t len)
{
    spi_bit_t* spi_bit = spi_to_spi_bit(spi);

    for(int i=0; i < len; i++)
    {
        *rd_data++ = spi_read_writeb(spi_bit, *wr_data++, spi_bit->mode);
    }

    return len;
}

const struct spi_operations spi_ops={
    .set_mode = spi_bit_set_mode,
    .set_speed = spi_bit_set_speed,
    .write = spi_bit_write,
    .read = spi_bit_read,
    .transfer = spi_bit_transfer,
};

void spi_bit_init(spi_t* spi, spi_bit_t* spi_bit)
{
    ASSERT(spi);
    ASSERT(spi_bit);
    ASSERT(spi_bit->ops);
    
    spi->private_data = spi_bit;
}
