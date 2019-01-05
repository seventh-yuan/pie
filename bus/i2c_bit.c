#include <common/pie.h>
#include <bus/i2c_bit.h>


#define scl_set(ops, state)    (ops->scl_set(state))
#define sda_set(ops, state)    (ops->sda_set(state))
#define sda_get(ops)        (ops->sda_get())

#define SCL_H(ops)            scl_set(ops, 1)
#define SCL_L(ops)            scl_set(ops, 0)
#define SDA_H(ops)            sda_set(ops, 1)
#define SDA_L(ops)            sda_set(ops, 0)
#define SDA_STATE(ops)        sda_get(ops)

pie_inline void i2c_udelay(const struct i2c_bit_operations *ops, int us)
{
    if(ops->udelay)
        ops->udelay(us);
}

pie_inline void i2c_enable_output(const struct i2c_bit_operations* ops)
{
    if(ops->enable_output)
        ops->enable_output();
}

pie_inline void i2c_enable_input(const struct i2c_bit_operations* ops)
{
    if(ops->enable_input)
        ops->enable_input();
}

pie_inline i2c_bit_t *i2c_to_i2c_bit(i2c_t *i2c)
{
    return CONTAINER_OF(i2c, i2c_bit_t, i2c);
}

int i2c_bit_start(i2c_t *i2c)
{
    i2c_bit_t *i2c_bit = i2c_to_i2c_bit(i2c);
    i2c_enable_output(i2c_bit->ops);
    SDA_H(i2c_bit->ops);
    SCL_H(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    SDA_L(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    SCL_L(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    return 0;
}

int i2c_bit_stop(i2c_t *i2c)
{
    i2c_bit_t *i2c_bit = i2c_to_i2c_bit(i2c);
    i2c_enable_output(i2c_bit->ops);
    SDA_L(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    SCL_H(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    SDA_H(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    return 0;
}
int i2c_bit_writeb(i2c_t *i2c, uint8_t data, i2c_ack_t *ack)
{
    i2c_bit_t *i2c_bit = i2c_to_i2c_bit(i2c);
    uint8_t temp = 0;

    i2c_enable_output(i2c_bit->ops);

    for(int i=0;i<8;i++)
    {
        temp = data&0x80;
        data <<= 1;
        if(temp)
            SDA_H(i2c_bit->ops);
        else
            SDA_L(i2c_bit->ops);
        i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
        SCL_H(i2c_bit->ops);
        i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
        SCL_L(i2c_bit->ops);
        i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    }

    SDA_H(i2c_bit->ops);

    i2c_enable_input(i2c_bit->ops);

    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    SCL_H(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    if(SDA_STATE(i2c_bit->ops))
        *ack = I2C_NACK;
    else
        *ack = I2C_ACK;

    SCL_L(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);

    return 0;
}
int i2c_bit_readb(i2c_t *i2c, uint8_t *data, i2c_ack_t ack)
{
    i2c_bit_t *i2c_bit = i2c_to_i2c_bit(i2c);

    int temp = 0;

    SDA_H(i2c_bit->ops);

    i2c_enable_input(i2c_bit->ops);

    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    for(int i=0;i<8;i++)
    {
        temp <<= 1;
        SCL_H(i2c_bit->ops);
        i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
        if(SDA_STATE(i2c_bit->ops))
            temp |= 0x01;
        SCL_L(i2c_bit->ops);
        i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    }

    i2c_enable_output(i2c_bit->ops);

    if(ack == I2C_ACK)
        SDA_L(i2c_bit->ops);
    else
        SDA_H(i2c_bit->ops);
    SCL_H(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    SCL_L(i2c_bit->ops);
    i2c_udelay(i2c_bit->ops, i2c_bit->delay_us);
    *data = temp;

    return 0;
}

const struct i2c_operations i2c_ops={
    .start = i2c_bit_start,
    .stop = i2c_bit_stop,
    .writeb = i2c_bit_writeb,
    .readb = i2c_bit_readb,
};


int i2c_bit_register(const char* dev_name, i2c_bit_t* i2c_bit)
{
    ASSERT(dev_name);
    ASSERT(i2c_bit);
    ASSERT(i2c_bit->ops);

    i2c_bit->i2c.ops = &i2c_ops;

    return i2c_register(dev_name, &i2c_bit->i2c);
}
