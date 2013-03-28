/**
 * @file libmjollnir/include/libmjollnir-int.h
 * @ingroup libmjollnir
 * @{
 * (C) 2006-2008 Asgard Labs, thorolf
 *     2008-2010 ERESI Team
 *
 */

#define __DEBUG_MJOLLNIR__	1
#define __DEBUG_CALLS__		1
#define __DEBUG_READ__		1
#define __DEBUG_PTR__		1
#define __DEBUG_OPERAND__	0
#define __DEBUG_BLOCKS__	1
#define __DEBUG_BLK_LOOKUP__	1
#define __DEBUG_FLOW__		1
#define __DEBUG_CNTNR__		1
#define __DEBUG_CNTNR_LOOKUP__	1
#define __DEBUG_FUNCS__		1
#define __DEBUG_FUCNS_LOOKUP__	1
#define __DEBUG_FNG__		0
#define	__DEBUG_ONDISK__	1
#define	__DEBUG_LINKS__		0

/* Debug descriptor */
#define D_DESC		stderr

#define mjrHashVerySmall 64
#define mjrHashSmall     1024
#define mjrHashMedium    10240
#define mjrHashLarge     102400
#define mjrHahsXXL       1024000

#define MJR_MAX_FUNCTION_LEN 1024000

/** @} */
