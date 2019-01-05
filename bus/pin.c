#include <string.h>
#include <common/pie.h>
#include <bus/pin.h>

LIST_HEAD(pin_list_head);

int pin_register(const char* dev_name, pin_t* pin)
{
    ASSERT(dev_name);
    ASSERT(pin);
    ASSERT(pin->ops);

    pin->dev_name = dev_name;

    list_add(&pin_list_head, &pin->node);

    return 0;
}

pin_t* pin_open(const char* dev_name)
{
    ASSERT(dev_name);

    pin_t* pin = NULL;
    list_t* node;
    list_for_each(node, &pin_list_head)
    {
        pin = list_entry(node, pin_t, node);
        if (0 == strcmp(pin->dev_name, dev_name))
            break;
    }

    if (pin && (!pin->inited))
    {
        if (pin->ops->init && !pin->ops->init(pin))
            return NULL;
        pin->inited = TRUE;
    }
    return pin;
}

void pin_close(pin_t* pin)
{
    if (pin && pin->inited)
        pin->inited = FALSE;
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
