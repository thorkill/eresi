/*
** comment.c for elfsh in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Sun Dec  1 09:22:45 2002 mayhem
** Last update Mon Apr 14 16:55:54 2003 mayhem
*/
#include "libelfsh.h"


/* Get comment section */
elfshsect_t		*elfsh_get_comments(elfshobj_t *file)
{
  elfshsect_t		*new;
  u_int			size;

  if (NULL == file)
    ELFSH_SETERROR("[libelfsh:get_comments] Invalid NULL paramater\n", NULL);
  new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_COMMENT, 
				  NULL, NULL, &size);
  if (NULL == new)
    return (NULL);
  if (NULL == new->data)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (NULL == new->data)
	return (NULL);
    }
  file->secthash[ELFSH_SECTION_COMMENT] = new;
  return (new);
}


/* Get an entry from .comment */
char			*elfsh_get_comments_entry(elfshobj_t *file, u_int range)
{
  int			index;
  int			act;
  char			*data;

#define	CHECK_SZ	(index < file->secthash[ELFSH_SECTION_COMMENT]->shdr->sh_size)

  if (!file->secthash[ELFSH_SECTION_COMMENT] && !elfsh_get_comments(file))
    return (NULL);
  index = act = 0;
  data = file->secthash[ELFSH_SECTION_COMMENT]->data;
  while (!data[index] && CHECK_SZ)
    index++;
  while (act != range && CHECK_SZ) 
    if (!data[index])
      {
	act++;
	while (!data[index] && CHECK_SZ)
	  index++;
      }
    else
      index++;

#undef CHECK_SZ

  if (index < file->secthash[ELFSH_SECTION_COMMENT]->shdr->sh_size)
    return (data + index);
  return (NULL);
}
