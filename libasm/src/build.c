/**
 * @file build.c
 * @brief Contains latest build date
 * $Id: build.c,v 1.2 2007-10-14 00:01:41 heroine Exp $
 */
#include <libasm.h>

/**
 * @brief Return build date.
 * @return build date.
 */

char	*asm_get_build(void)
{
  return (__DATE__);
}

/**
 *
 */

char	*g_asm_features [] =
  {
    
    
    0
  };

/**
 *
 */

char	**asm_get_features()
{
  return g_asm_features;
}
