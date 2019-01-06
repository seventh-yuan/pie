#ifndef __DEVICE_H__
#define __DEVICE_H__
#include <common/piedef.h>
#ifdef __cplusplus
extern "C"{
#endif

#define MODULE_DEV(n)      MODULE_DEFINE(dev, n)
#define IMPORT_DEV(n)      IMPORT_MODULE(devs, n)
#define DEV(n)             MODULE(dev, n)


void device_init(device_t* device, const device_ops_t* ops);

int device_write(device_t* device, const uint8_t* wr_data, size_t wr_len);

int device_read(device_t* device, uint8_t* rd_data, size_t rd_len);

#ifdef __cplusplus
}
#endif
#endif