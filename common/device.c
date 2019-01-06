#include <common/pie.h>

void device_init(device_t* device, const device_ops_t* ops)
{
    ASSERT(device);
    ASSERT(ops);

    device->ops = ops;
}

int device_write(device_t* device, const uint8_t* wr_data, size_t wr_len)
{
    ASSERT(device);
    ASSERT(wr_data);
    ASSERT(device->ops);

    if (device->ops->write)
        return device->ops->write(device, wr_data, wr_len);

    return 0;
}

int device_read(device_t* device, uint8_t* rd_data, size_t rd_len)
{
    ASSERT(device);
    ASSERT(rd_data);

    if (device->ops->read)
        return device->ops->read(device, rd_data, rd_len);

    return 0;
}