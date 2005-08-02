/*
** ctors.c for elfsh
** 
** Started on  Tue Feb 26 22:07:58 2002 mayhem
**
*/
#include "elfsh.h"




/* Display the constructor section */
int		cmd_ctors()
{
  regex_t	*tmp;
  elfsh_Addr	*ctors;
  int		size;
  int		index;
  elfsh_SAddr  	offset;
  elfsh_SAddr	doffset;
  char		*name;
  char		*dname;
  char		off[30];
  char		buff[256];
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ctors = elfsh_get_ctors(world.curjob->current, &size);
  if (ctors == NULL)
    RET(-1);
  FIRSTREGX(tmp);
  snprintf(logbuf, BUFSIZ - 1,
	   " [Constructors array .::. CTORS]\n [Object %s]\n\n", 
	   world.curjob->current->name);
  vm_output(logbuf);
  off[0] = 0;

  for (index = 0; index < size; index++)
    {

      name = elfsh_reverse_symbol(world.curjob->current, ctors[index], &offset);
      dname = elfsh_reverse_dynsymbol(world.curjob->current, ctors[index], &doffset);
      if (dname && doffset < offset)
	{
	  name = dname;
	  offset = doffset;
	}

      if (off)
	snprintf(off, sizeof(off), " %c %u", 
		 (offset < 0 ? '-' : '+'), 
		 (u_int) (offset > 0 ? offset : offset - offset - offset));
      
      snprintf(buff, sizeof(buff), " [%02u]  " XFMT " \t <%s%s>\n", 
	       index, 
	       (elfsh_Addr) ctors[index], 
	       (name ? name : "?"), 
	       (name && offset ? off : ""));
      
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	vm_output(buff);

    }


  vm_output("\n");
  return (0); 
}
