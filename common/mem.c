#include <common/pie.h>
#include <string.h>

void *mem_alloc(size_t size)
{
#if CONFIG_PIE_USING_RTOS==1
    return pvPortMalloc(size);
#else
    return NULL;
#endif
}

void *mem_realloc(void *ptr,size_t size)
{
    void *new = NULL;
#if CONFIG_PIE_USING_RTOS==1
    new = pvPortMalloc(size);
    if(ptr)
    {
        memcpy(new, ptr, size);
        vPortFree(ptr);
    }
#endif
    return new;
}

void mem_free(void *ptr)
{
#if CONFIG_PIE_USING_RTOS==1
    vPortFree(ptr);
#endif
}