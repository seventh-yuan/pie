#include <common/pie.h>

extern void driver_init(void);
extern void application_init(void);


void startup(void)
{
    driver_init();

    application_init();
}

void pie_init(void)
{
    startup();

    pie_kernel_start();  
}
