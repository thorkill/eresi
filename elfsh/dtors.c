/*
** dtors.c for elfsh
** 
** Started on  Tue Feb 26 22:08:20 2002 mayhem
**
*/
#include "elfsh.h"




/* Display the destructor section */
int		cmd_dtors()
{
  regex_t	*tmp;
  elfsh_Addr	*dtors;
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

  dtors = elfsh_get_dtors(world.curjob->current, &size);
  if (dtors == NULL)
    RET(-1);
  FIRSTREGX(tmp);
  snprintf(logbuf, BUFSIZ - 1,
	   " [Destructors array .::. DTORS]\n [Object %s]\n\n", 
	   world.curjob->current->name);
  vm_output(logbuf);
  off[0] = 0;

  for (index = 0; index < size; index++)
    {

      name = elfsh_reverse_symbol(world.curjob->current, dtors[index], &offset);
      dname = elfsh_reverse_dynsymbol(world.curjob->current, dtors[index], &doffset);
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
	       index, (elfsh_Addr) dtors[index], 
	       (name ? name : "?"), 
	       (name && offset ? off : ""));
      
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	vm_output(buff);

    }


  vm_output("\n");
  return (0); 
}
