/*
** comment.c in elfsh
**
** Made by <mayhem@devhell.org>
**
** Started on  Tue Dec 17 06:43:01 2002 mayhem
**
*/
#include "elfsh.h"


/* Display the note section */
int			cmd_comments()
{
  elfshsect_t		*sect;
  regex_t		*tmp;
  int			range;
  char			buff[256];
  char			*ent;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("\n [SHT_COMMENT]\n");
  sect = elfsh_get_comments(world.curjob->current);
  if (!sect)
    RET(-1);
  FIRSTREGX(tmp);

  ent = elfsh_get_comments_entry(world.curjob->current, 0);
  for (range = 0; ent != NULL; range++)
    {
      snprintf(buff, sizeof(buff),
	       " [%02u] \t COMMENT ENTRY: %s\n", range, ent);
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	vm_output(buff);
      ent = elfsh_get_comments_entry(world.curjob->current, range + 1);
    }

  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
