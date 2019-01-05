#ifndef __ERRNO_H__
#define __ERRNO_H__

#define ENO_OK           (0)
#define ENO_ERROR        (-1)
#define ENO_TIMEOUT      (-2)
#define ENO_FULL         (-3)
#define ENO_EMPTY        (-4)
#define ENO_NOMEM        (-5)
#define ENO_BUSY         (-6)
#define ENO_INVAL        (-7)
#define ENO_INIT         (-8)


const char* errno_get_reason(int errno);

#endif