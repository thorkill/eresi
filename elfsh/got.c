/*
** got.c for elfsh
** 
** Started on  Fri Nov  2 15:20:18 2001 mayhem
** Last update Sun May 18 14:44:37 2003 mayhem
*/
#include "elfsh.h"


/* Command handler for GOT command */
int		cmd_got()
{
  regex_t	*tmp;
  u_long	*got;
  int		size;
  int		index;
  int		offset;
  int		doffset;
  char		*name;
  char		*dname;
  char		off[30];
  char		buff[256];
  
  /* Get the regx and fetch the GOT data */
  CHOOSE_REGX(tmp);
  if ((got = elfsh_get_got(world.current, &size)) == NULL)
    RET(-1);
  printf(" [Global Offset Table .::. GOT]\n [Object %s]\n\n", 
	 world.current->name);
  off[0] = 0;

  /* Loop and print all entry and their resolve */
  for (index = 0; index < size; index++)
    {

      name = elfsh_reverse_symbol(world.current, got[index], &offset);
      dname = elfsh_reverse_dynsymbol(world.current, got[index], &doffset);
      if (dname && doffset < offset)
	{
	  name = dname;
	  offset = doffset;
	}

      if (off != NULL)
	snprintf(off, sizeof(off), " %c %u", (offset < 0 ? '-' : '+'), 
		 (offset > 0 ? offset : offset - offset - offset));

      snprintf(buff, sizeof(buff), " [%08X:%5u] 0x%08X \t <%s%s>", 
	       world.current->secthash[ELFSH_SECTION_GOT]->shdr->sh_addr + (index * sizeof(u_long)),
	       index, (u_int) got[index], (name != NULL ? name : "?"), 
	       (off[0] && name && offset ? off : ""));

      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);

    }

  puts("\n");
  return (0); 
}


