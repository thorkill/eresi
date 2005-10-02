/*
** notes.c for elfsh
** 
** Started on  Fri Nov  2 15:20:58 2001 mayhem
**
*/
#include "elfsh.h"




/* Display the note section */
int			cmd_notes()
{
  elfshsect_t		*sect;
  elfshnotent_t		*e;
  regex_t		*tmp;
  int			index;
  int			range;
  char			buff[256];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buff, sizeof(buff),
	   " [SHT_NOTES]\n [Object %s]\n\n", 
	   world.curjob->current->name);
  vm_output(buff);

  FIRSTREGX(tmp);
  range = 0;
  sect = elfsh_get_notes(world.curjob->current, range);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get notes", (-1));

  while (sect)
    {
      snprintf(buff, sizeof(buff),
	       " {Section %s} \n", elfsh_get_section_name(world.curjob->current, sect));
      vm_output(buff);
      for (index = 0, e = sect->altdata; e; e = e->next, index++)
	{
	  snprintf(buff, sizeof(buff), " [%02u] \t NOTES_ENTRY(%s)\n", 
		   index, e->note);
	  if (!tmp || (tmp && e->note && !regexec(tmp, buff, 0, 0, 0)))
	    vm_output(buff);
	}
      vm_output("\n");
      range++;
      sect = elfsh_get_notes(world.curjob->current, range);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}








