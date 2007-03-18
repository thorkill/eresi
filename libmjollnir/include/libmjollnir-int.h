/*
 * (C) 2006-2007 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-int.h,v 1.12 2007-03-18 23:11:03 thor Exp $
 *
 */

#define __DEBUG_MJOLLNIR__	0
#define __DEBUG_CALLS__		0
#define __DEBUG_READ__		0
#define __DEBUG_PTR__		0
#define __DEBUG_OPERAND__	0
#define __DEBUG_BLOCKS__	1
#define __DEBUG_FLOW__		1

/* Debug Fingerprinting */
#define __DEBUG_FNG__		0

/* Debug descriptor */
#define D_DESC		stdout

#define mjrHashVerySmall 64
#define mjrHashSmall 	 1024
#define mjrHashMedium 	 10240
#define mjrHashLarge  	 102400
#define mjrHahsXXL	 1024000

#define MJR_MAX_FUNCTION_LEN 1024000

