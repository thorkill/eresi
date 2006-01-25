/*
 * Only useful for linking the symbol inside the static mode
 *
 *
 *
 *
 */
#include "elfsh.h"

#ifndef(E2DBG_INTERN)


/* Step command */
int		cmd_step()
{
  printf(" [*] No stepping in static mode\n\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


#endif
