#include <common/pie.h>

#if CONFIG_PIE_USING_RTOS==0
static  LIST_HEAD(co_thread_list);

void co_thread_start(co_thread_t* pthread, void* arg)
{
    list_add(&co_thread_list, &pthread->node);
}

void co_thread_stop(co_thread_t* pthread)
{
    list_del(&pthread->node);
}

static void co_thread_dispatch(void)
{
    co_thread_t* thread;
    list_t* node;

    list_for_each(node, &co_thread_list)
    {
        thread = list_entry(node, co_thread_t, node);
        if(thread->entry != NULL)
        {
            thread->entry(thread->arg);
        }
    }
}
#endif

void pie_kernel_start(void)
{
#if CONFIG_PIE_USING_RTOS==1
    osKernelStart();
#else
    for(;;)
    {
        co_thread_dispatch();
    }
#endif
}

void pie_kernel_periodic(void)
{
#if CONFIG_PIE_USING_RTOS==1
    osSystickHandler();
#endif
    tick_periodic();
}