#ifndef __PIE_DEF_H__
#define __PIE_DEF_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif

#if 0
typedef signed   char                   int8_t;      /**<  8bit integer type */
typedef signed   short                  int16_t;     /**< 16bit integer type */
typedef signed   long                   int32_t;     /**< 32bit integer type */
typedef unsigned char                   uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned long                   uint32_t;    /**< 32bit unsigned integer type */
typedef uint32_t                        size_t;      /**< Type for size number */
/* maximum value of base type */
#define UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#endif
typedef int                             bool_t;      /**< boolean type */

/* 32bit CPU */
typedef long                            base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   ubase_t;     /**< Nbit unsigned CPU related data type */

typedef base_t                       err_t;       /**< Type for error number */
typedef uint32_t                     tick_t;      /**< Type for tick count */
typedef base_t                       flag_t;      /**< Type for flags */
typedef base_t                       off_t;       /**< Type for offset */
typedef uint32_t                      usec_t;
typedef uint32_t                      millis_t;

/* boolean type definitions */
#ifndef TRUE
#define TRUE                         1               /**< boolean true  */
#endif

#ifndef FALSE
#define FALSE                        0               /**< boolean fails */
#endif

#define TICK_MAX                     UINT32_MAX   /**< Maxium number of tick */


#if defined(__ICCARM__)
#define pie_volatile            volatile
#define pie_inline          static inline
#define pie_weak            __weak
#elif defined(__GNUC__)
#define pie_volatile       volatile
#define pie_inline          static __inline
#define pie_weak            __attribute__((weak))
#elif defined(__CC_ARM)
#define pie_volatile        __volatile
#define pie_inline           __inline
#define pie_weak            __weak
#endif


#define ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

#define ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#ifndef NULL
#define NULL                         (0)
#endif

#define CONTAINER_OF(node, type, member)\
    ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

#define PIE_BIT(b)    (1<<(b))

#define PIE_BIT_SET(x,b)            ((x)|=PIE_BIT(b))
#define PIE_BIT_RESET(x,b)          ((x)&=~(PIE_BIT(b)))
#define PIE_BITS_SET(x,bits)        ((x)|=bits)
#define PIE_BITS_RESET(x,bits)      ((x)&=~(bi/bits))
#define PIE_BIT_TEST(x,b)           (((x)&(PIE_BIT(b)))?1:0)
#define PIE_BITS_TEST(x,bits)       (((x)&(bits))?1:0)

#define MODULE_DEFINE(name, n)      name##_t name##_module##n
#define IMPORT_MODULE(name, n)      extern name##_t name##_module##n
#define MODULE(name, n)             (&(name##_module##n))

typedef struct list_head {
    struct list_head *next;
    struct list_head *prev;
} list_t;

typedef struct slist_head {
    struct slist_head* next;
} slist_t;

typedef struct{
    list_t node;
    err_t (*entry)(void* arg);
    void* arg;
} co_thread_t;

typedef struct device {
    void* private_data;
    const struct device_operations* ops;
} device_t;

typedef struct device_operations {
    int (*write)(struct device* device, const uint8_t* wr_data, size_t wr_len);
    int (*read)(struct device* device, uint8_t* rd_data, size_t rd_len);
} device_ops_t;

#ifdef __cplusplus
}
#endif
#endif
