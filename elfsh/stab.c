/*
** stab.c for elfsh
** 
** Started on  Fri Nov  2 15:18:56 2001 mayhem
**
*/
#include "elfsh.h"





/* Display the stab section */
int			cmd_stab()
{
  elfshstabent_t	*entry;
  regex_t		*tmp;
  char			*name;
  char			*type;
  u_int			num;
  u_int			index;
  u_int			typenum;
  char			buff[256];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  entry = elfsh_get_stab(world.curjob->current, &num);
  if (entry == NULL)
    RET(-1);
  FIRSTREGX(tmp);
  vm_output("\n[STAB TABLE]\n");
  
  for (index = 0; index < num; index++)
    {
      if ((name = elfsh_get_stab_name(world.curjob->current, entry + index)) == NULL)
	RET(-1);
      typenum = elfsh_get_stab_type(entry + index);
      type = (typenum > ELFSH_STAB_MAX ? NULL : elfsh_stab_type[typenum]);
      snprintf(buff, sizeof(buff), XFMT " : \t %s \nType \t   : \t %s \n\n",
	       elfsh_get_stab_offset(entry + index),
	       (!name || !name[0] ? "NULL" : name),
	       (!type || !type[0] ? "NULL" : type));
      if (!tmp || (tmp && type && !regexec(tmp, buff, 0, 0, 0)))
	vm_output(buff);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


