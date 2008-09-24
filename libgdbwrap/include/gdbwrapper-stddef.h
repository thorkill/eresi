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

#define WORD_IN_BYTE    2        
#define DWORD_IN_BYTE   4
#define QWORD_IN_BYTE   8
#define BYTE_IN_BIT     8
#define WORD_IN_BIT     16        
#define DWORD_IN_BIT    32
#define QWORD_IN_BIT    64

#define NEXT_CHAR(_x)   _x + 1

#ifndef __RELEASE
#define ASSERT(_x)      assert(_x)
#define NOT_REACHED()   ASSERT(FALSE)
#endif

#define FALSE       0
#define TRUE        1

typedef uint32_t    ureg32;    /* 32b register. */
typedef uint32_t    la32;      /* 32b linear address.  */
