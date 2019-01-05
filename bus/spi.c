#include <string.h>
#include <bus/spi.h>
#include <common/pie.h>

LIST_HEAD(spi_list_head);

int spi_register(const char* dev_name, spi_t* spi)
{
    ASSERT(dev_name);
    ASSERT(spi);
    ASSERT(spi->ops);

    spi->dev_name = dev_name;
    list_add(&spi_list_head, &spi->node);

    return 0;
}

spi_t* spi_open(const char* dev_name)
{
    ASSERT(dev_name);

    spi_t* spi = NULL;
    list_t* node;
    list_for_each(node, &spi_list_head)
    {
        spi = list_entry(node, spi_t, node);
        if (0 == strcmp(spi->dev_name, dev_name))
            break;
    }

    if (spi && (!spi->inited))
    {
        if (spi->ops->init && !spi->ops->init(spi))
            return NULL;
        spi->inited = TRUE;
    }
    return spi;
}

void spi_close(spi_t* spi)
{
    if (spi && spi->inited)
        spi->inited = FALSE;
}

int spi_set_mode(spi_t *spi, spi_mode_t mode)
{
    ASSERT(spi);

    if (spi->ops->set_mode)
        return spi->ops->set_mode(spi, mode);

    return 0;
}

int spi_set_speed(spi_t *spi, uint32_t speed)
{
    ASSERT(spi);

    if (spi->ops->set_speed)
        return spi->ops->set_speed(spi, speed);

    return 0;
}

int spi_write(struct spi *spi, const uint8_t *wr_data, size_t wr_len)
{
    ASSERT(spi);
    ASSERT(wr_data);

    if (spi->ops->write)
        return spi->ops->write(spi, wr_data, wr_len);
    return 0;
}

int spi_read(struct spi *spi, uint8_t *rd_data, size_t rd_len)
{
    ASSERT(spi);
    ASSERT(rd_data);

    if (spi->ops->read)
        return spi->ops->read(spi, rd_data, rd_len);

    return 0;
}

int spi_transfer(struct spi *spi, const uint8_t *wr_data, uint8_t *rd_data, size_t len)
{
    ASSERT(spi);
    ASSERT(wr_data);
    ASSERT(rd_data);

    if (spi->ops->transfer)
        return spi->ops->transfer(spi, wr_data, rd_data, len);
    return 0;
}
