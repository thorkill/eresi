/*
** comment.c<2> for elfsh in elfsh
**
** Made by mayhem
** Login   <mayhem@devhell.org>
**
** Started on  Tue Dec 17 06:43:01 2002 mayhem
** Last update Sat Aug 16 13:38:22 2003 jv
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

  puts("\n [SHT_COMMENT]\n");
  sect = elfsh_get_comments(world.current);
  if (!sect)
    RET(-1);
  CHOOSE_REGX(tmp);

  ent = elfsh_get_comments_entry(world.current, 0);
  for (range = 0; ent != NULL; range++)
    {
      snprintf(buff, sizeof(buff), " [%02u] \t COMMENT ENTRY: %s", range, ent);
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);
      ent = elfsh_get_comments_entry(world.current, range + 1);
    }

  puts("");
  return (0);
}
