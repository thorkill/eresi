/*
** dtors.c for elfsh
** 
** Started on  Tue Feb 26 22:08:20 2002 mayhem
** Last update Wed Apr 16 19:08:49 2003 mayhem
*/
#include "elfsh.h"




/* Display the destructor section */
int		cmd_dtors()
{
  regex_t	*tmp;
  u_long	*dtors;
  int		size;
  int		index;
  int		offset;
  int		doffset;
  char		*name;
  char		*dname;
  char		off[30];
  char		buff[256];

  dtors = elfsh_get_dtors(world.current, &size);
  if (dtors == NULL)
    RET(-1);
  CHOOSE_REGX(tmp);
  printf(" [Destructors array .::. DTORS]\n [Object %s]\n\n", 
	 world.current->name);
  off[0] = 0;

  for (index = 0; index < size; index++)
    {

      name = elfsh_reverse_symbol(world.current, dtors[index], &offset);
      dname = elfsh_reverse_dynsymbol(world.current, dtors[index], &doffset);
      if (dname && doffset < offset)
	{
	  name = dname;
	  offset = doffset;
	}

      if (off)
	snprintf(off, sizeof(off), " %c %u", 
		 (offset < 0 ? '-' : '+'), 
		 (offset > 0 ? offset : offset - offset - offset));
      
      snprintf(buff, sizeof(buff), " [%02u]  0x%08X \t <%s%s>", 
	       index, (u_int) dtors[index], 
	       (name ? name : "?"), 
	       (name && offset ? off : ""));
      
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);

    }


  puts("\n");
  return (0); 
}
