/*
** notes.c for elfsh
** 
** Started on  Fri Nov  2 15:20:58 2001 mayhem
** Last update Mon Mar 17 03:01:37 2003 mayhem
*/
#include "elfsh.h"




/* Display the note section */
int			cmd_notes()
{
  elfshsect_t		*sect;
  elfshnotent_t	*e;
  regex_t		*tmp;
  int			index;
  int			range;
  char			buff[256];
  
  printf(" [SHT_NOTES]\n [Object %s]\n\n", world.current->name);
  CHOOSE_REGX(tmp);
  range = 0;
  sect = elfsh_get_notes(world.current, range);
  while (sect)
    {
      printf(" {Section %s} \n", elfsh_get_section_name(world.current, sect));
      for (index = 0, e = sect->altdata; e; e = e->next, index++)
	{
	  snprintf(buff, sizeof(buff), " [%02u] \t NOTES_ENTRY(%s)", 
		   index, e->note);
	  if (!tmp || (tmp && e->note && !regexec(tmp, buff, 0, 0, 0)))
	    puts(buff);
	}
      puts("");
      range++;
      sect = elfsh_get_notes(world.current, range);
    }
  return (0);
}








