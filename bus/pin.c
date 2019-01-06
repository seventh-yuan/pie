#include <string.h>
#include <common/pie.h>
#include <bus/pin.h>

void pin_init(pin_t* pin, const pin_ops_t* ops)
{
    ASSERT(pin);
    ASSERT(pin->ops);

    pin->ops = ops;
}

int pin_set_direction(pin_t* pin, int index, pin_dir_t dir)
{
    ASSERT(pin);
    ASSERT(pin->ops);
    ASSERT(pin->ops->set_direction);

    int ret;

    if((ret = pin->ops->set_direction(pin, index, dir)) != 0)
        return ret;

    return 0;
}

int pin_get_direction(struct pin* pin, int index)
{
    ASSERT(pin);
    ASSERT(pin->ops);
    ASSERT(pin->ops->get_direction);

    pin_dir_t dir = PIN_DIR_INPUT;

    int ret;

    if((ret = pin->ops->get_direction(pin, index, &dir)) != 0)
        return ret;

    return dir;
}

int pin_get_output(struct pin* pin, int index)
{
    ASSERT(pin);
    ASSERT(pin->ops);
    ASSERT(pin->ops->get_output);

    pin_level_t level = PIN_LEVEL_LOW;

    int ret;

    if((ret = pin->ops->get_output(pin, index, &level)) != 0)
        return ret;

    return level;
}

int pin_set_level(struct pin* pin, int index, pin_level_t level)
{
    ASSERT(pin);
    ASSERT(pin->ops);
    ASSERT(pin->ops->set_level);

    int ret;

    if((ret = pin->ops->set_level(pin, index, level)) != 0)
        return ret;

    return 0;
}

int pin_get_level(struct pin* pin, int index)
{
    ASSERT(pin);
    ASSERT(pin->ops);
    ASSERT(pin->ops->get_level);

    int ret;

    pin_level_t level = PIN_LEVEL_LOW;
    if((ret = pin->ops->get_level(pin, index, &level)) != 0)
        return ret;

    return level;
}
