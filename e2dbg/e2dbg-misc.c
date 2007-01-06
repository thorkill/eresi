/*
** e2dbg-misc.c for e2dbg
**
** Misc routines for the debugger
** 
** Started on  Fri Jun 05 15:21:56 2005 mayhem
**
*/
#include "e2dbg.h"






/* Get the parent object of a breakpoint */
/* Thats needed for the mprotect stuff inside the breakpoint handler */
elfshobj_t	*vm_get_parent_object(elfsh_Addr addr)
{
  elfsh_Phdr	*cur;
  elfshobj_t	*curfile;
  elfshsect_t	*cursect;
  char		**keys;
  int		index;
  int		keynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(&world.curjob->loaded, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      curfile = hash_get(&world.curjob->loaded, keys[index]);
      cursect = elfsh_get_parent_section(curfile, addr, NULL);
      if (cursect)
	{
	  cur = elfsh_get_parent_segment(curfile, cursect);
	  if (cur)
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, curfile);
	}
    }
  
  /* Parent object not found */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get parent object addr", (NULL));
}



/* Concatenate all parameters and return a single string */
char	*vm_get_string(char **params)
{
  char	buff[BUFSIZ];
  int	idx;
  int	len;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (len = idx = 0; params[idx]; idx++)
    len += snprintf(buff + len, BUFSIZ - len, "%s%s", 
		    (idx ? " " : ""), params[idx]);
   

  if (len)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       elfsh_strdup(buff));
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Empty display", NULL);
}



