/*
** bp.c for elfsh
** 
** Started Fri Jul 29 15:41:20 2005 ym
** Updated Tue Oct 11 19:07:13 2005 mm
** 
*/
#include "libelfsh.h"


/* Add a breakpoint */
int		elfsh_bp_add(hash_t	*bps, 
			     elfshobj_t *file, 
			     char	*resolv, 
			     elfsh_Addr addr, 
			     u_char	flags)
{
  static int	lastbpid = 1;
  elfshbp_t	*bp;
  char		tmp[32];
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || addr == 0 || bps == 0) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* Breakpoints handlers must be initialized */
  elfsh_setup_hooks();
  XALLOC(bp , sizeof(elfshbp_t), (-1));
  bp->obj     = file;
  bp->type    = INSTR;
  bp->addr    = addr;
  bp->symname = elfsh_strdup(resolv);
  bp->flags   = flags;
  snprintf(tmp, 32, XFMT, addr);   
  if (hash_get(bps, tmp))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Breakpoint already exist", -1);

  /* Call the architecture dependent hook for breakpoints */
  ret = elfsh_setbreak(file, bp);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Breakpoint insertion failed", (-1));

  /* Add new breakpoint to hash table */
  bp->id = lastbpid++;
  hash_add(bps, elfsh_strdup(tmp), bp); 
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
