/*
** ctors.c for elfsh
** 
** Started on  Tue Feb 26 22:07:58 2002 mayhem
** Last update Wed Apr 16 19:07:38 2003 mayhem
*/
#include "elfsh.h"




/* Display the constructor section */
int		cmd_ctors()
{
  regex_t	*tmp;
  u_long	*ctors;
  int		size;
  int		index;
  int		offset;
  int		doffset;
  char		*name;
  char		*dname;
  char		off[30];
  char		buff[256];
  
  ctors = elfsh_get_ctors(world.current, &size);
  if (ctors == NULL)
    RET(-1);
  CHOOSE_REGX(tmp);
  printf(" [Constructors array .::. CTORS]\n [Object %s]\n\n", 
	 world.current->name);
  off[0] = 0;

  for (index = 0; index < size; index++)
    {

      name = elfsh_reverse_symbol(world.current, ctors[index], &offset);
      dname = elfsh_reverse_dynsymbol(world.current, ctors[index], &doffset);
      if (dname && doffset < offset)
	{
	  name = dname;
	  offset = doffset;
	}

      if (off)
	snprintf(off, sizeof(off), " %c %u", 
		 (offset < 0 ? '-' : '+'), 
		 (offset > 0 ? offset : offset - offset - offset));
      
      snprintf(buff, sizeof(buff), " [%02u]  %10p \t <%s%s>", 
	       index, 
	       (void *) ctors[index], 
	       (name ? name : "?"), 
	       (name && offset ? off : ""));
      
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);

    }


  puts("\n");
  return (0); 
}
