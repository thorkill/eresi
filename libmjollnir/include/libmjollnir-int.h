/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-int.h,v 1.10 2007-01-02 06:21:23 may Exp $
 *
 */

#define __DEBUG_MJOLLNIR__	0
#define __DEBUG_CALLS__		0
#define __DEBUG_READ__		0
#define __DEBUG_PTR__		0
#define __DEBUG_OPERAND__	0
#define __DEBUG_BLOCKS__	0
#define __DEBUG_FLOW__		0

/* Debug descriptor */
#define D_DESC		stdout

#define mjrHashVerySmall 64
#define mjrHashSmall 	 1024
#define mjrHashMedium 	 10240
#define mjrHashLarge  	 102400
#define mjrHahsXXL	 1024000

#define MJR_MAX_FUNCTION_LEN 1024000

