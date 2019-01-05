#ifndef __TIME_H__
#define __TIME_H__
#include <common/piedef.h>

#ifdef __cplusplus
extern "C"{
#endif

#define time_after(unknow, known)    ((long)(known)-(long)(unknow)<0)
#define time_before(unknow, known)    ((long)(unknow)-(long)(known)<0)
#define time_after_eq(unknow, known)  ((long)(unknow)-(long)(known)>=0)
#define time_before_eq(unknow, known) ((long)(known)-(long)(unknow)>=0)
    
tick_t get_ticks(void);

millis_t get_millis(void);

void delay_us(usec_t us);

#define sleep_us delay_us

void delay_ms(millis_t millis);

void sleep_ms(millis_t millis);

void sleep(float sec);

void tick_periodic(void);
#ifdef __cplusplus
}
#endif
#endif