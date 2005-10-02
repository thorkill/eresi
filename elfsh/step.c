/*
 * Only useful for linking the symbol inside the static mode
 *
 *
 *
 *
 */
#include "elfsh.h"



/* Step command */
int		cmd_step()
{
  printf(" [*] No stepping in static mode\n\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
