#include <common/pie.h>

volatile tick_t ticks = 0;

tick_t get_ticks(void)
{
    return ticks;
}

millis_t get_millis(void)
{
    return ticks * 1000 / KERNEL_HZ;
}

void delay_us(usec_t us)
{
    int count_1us = (CPU_CLOCK_HZ / 1000000);

    for(int i = 0; i < us; i ++)
        for(int j = 0; j < count_1us; j ++);
}

void delay_ms(millis_t millis)
{
    unsigned long timeout = ticks + millis * KERNEL_HZ / 1000;

    while(time_before(ticks, timeout));
}

void sleep_ms(millis_t millis)
{
#if CONFIG_PIE_USING_RTOS==1
#if (INCLUDE_xTaskGetSchedulerState == 1)
    if(xTaskGetSchedulerState()==taskSCHEDULER_NOT_STARTED)
    {
        delay_ms(millis);
    }
    else
    {
        osDelay(millis);
    }
#else
    osDelay(millis);
#endif
#else
    delay_ms(millis);
#endif
}

void sleep(float sec)
{
    usec_t usec = (usec_t)(sec * 1000000) % 1000;
    sleep_ms((millis_t)(sec * 1000));
    sleep_us(usec);
}

void tick_periodic(void)
{
    ticks ++;
}
