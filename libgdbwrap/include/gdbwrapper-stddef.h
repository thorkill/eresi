/* This file has to be merged with eresi */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#define LOWORD(_dw)     ((_dw) & 0xffff)
#define HIWORD(_dw)     (((_dw) >> 16) & 0xffff)
#define LOBYTE(_w)      ((_w) & 0xff)
#define HIBYTE(_w)      (((_w) >> 8) & 0xff)

#define HIDWORD(_qw)    ((uint32)((_qw) >> 32))                                                                                                
#define LODWORD(_qw)    ((uint32)(_qw))                                                                                                        
#define QWORD(_hi, _lo) ((((uint64)(_hi)) << 32) | ((uint32)(_lo)))


typedef ptrdiff_t ptrdiff;   /* Isn't that boring this _t ? */
typedef uint32_t  uint32;    /* Isn't that boring this _t ? */
typedef uint8_t   uint8;     /* Isn't that boring this _t ? */
typedef uint32    ureg32;    /* 32b register. */

typedef uint32    la32;      /* 32b linear address.  */
