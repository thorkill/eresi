/*
** stab.c for elfsh
** 
** Started on  Fri Nov  2 15:18:56 2001 mayhem
** Last update Tue Feb 18 05:03:26 2003 mayhem
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

  entry = elfsh_get_stab(world.current, &num);
  if (entry == NULL)
    RET(-1);
  CHOOSE_REGX(tmp);
  puts("\n[STAB TABLE]");
  
  for (index = 0; index < num; index++)
    {
      if ((name = elfsh_get_stab_name(world.current, entry + index)) == NULL)
	RET(-1);
      typenum = elfsh_get_stab_type(entry + index);
      type = (typenum > ELFSH_STAB_MAX ? NULL : elfsh_stab_type[typenum]);
      snprintf(buff, sizeof(buff), "%10p : \t %s \nType \t   : \t %s \n",
	       elfsh_get_stab_offset(entry + index),
	       (!name || !name[0] ? "NULL" : name),
	       (!type || !type[0] ? "NULL" : type));
      if (!tmp || (tmp && type && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);
    }

  return (0);
}


