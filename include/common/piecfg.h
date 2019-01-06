#ifndef __PIE_CFG_H__
#define __PIE_CFG_H__
#include <configs.h>

#ifndef KERNEL_HZ
#define KERNEL_HZ       200
#endif

#ifndef CPU_CLOCK_HZ
#define CPU_CLOCK_HZ        72000000
#endif


#ifndef CONFIG_PIE_USING_ASSERT
#define CONFIG_PIE_USING_ASSERT             1
#endif

#ifndef CONFIG_PIE_USING_STDIO
#define CONFIG_PIE_USING_STDIO              1
#endif

#endif