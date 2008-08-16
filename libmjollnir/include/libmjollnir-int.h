/**
 * @file libmjollnir-int.h
 * (C) 2006-2008 Asgard Labs, thorolf
 *
 */

#define __DEBUG_MJOLLNIR__	0
#define __DEBUG_CALLS__		0
#define __DEBUG_READ__		0
#define __DEBUG_PTR__		0
#define __DEBUG_OPERAND__	0
#define __DEBUG_BLOCKS__	0
#define __DEBUG_BLK_LOOKUP__	0
#define __DEBUG_FLOW__		0
#define __DEBUG_CNTNR__		0
#define __DEBUG_CNTNR_LOOKUP__	0
#define __DEBUG_FUNCS__		0
#define __DEBUG_FUCNS_LOOKUP__	0
#define __DEBUG_FNG__		0
#define	__DEBUG_ONDISK__	0
#define	__DEBUG_LINKS__		0

/* Debug descriptor */
#define D_DESC		stderr

#define mjrHashVerySmall 64
#define mjrHashSmall 	 1024
#define mjrHashMedium 	 10240
#define mjrHashLarge  	 102400
#define mjrHahsXXL	 1024000

#define MJR_MAX_FUNCTION_LEN 1024000

