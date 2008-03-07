/**
 * @file build.c
 * @brief Contains latest build date
 * $Id: build.c,v 1.3 2008/01/01 14:55:00 heroine Exp $
 */

/**
 * State of the project.
 *
 * Libasm developpement is not regular. Some time is spent on documenting it.
 * Some time to fix bugs. Some time to implement.
 * 
 * Currently, there are two architectures supported :
 * -
 *
 *
 *
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
