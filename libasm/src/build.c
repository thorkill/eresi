/**
 * @file build.c
 * $Id: build.c,v 1.1 2007-05-29 00:40:27 heroine Exp $
 */
#include <libasm.h>

/**
 * Return build date.
 * @return build date.
 */

char	*asm_get_build(void)
{
  return (__DATE__);
}
