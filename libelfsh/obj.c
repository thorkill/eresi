/*
** obj.c for elfsh
**
** Started on  Wed Feb 12 00:07:06 2003 mayhem
** Last update Sat Aug 16 13:32:24 2003 jv
*/
#include "libelfsh.h"


/* Open the file 'name' and returns the elfshobj_t */
elfshobj_t		*elfsh_load_obj(char *name)
{
  elfshobj_t	*file;

  XALLOC(file, sizeof(elfshobj_t), NULL);
  XOPEN(file->fd, name, O_RDONLY, 0, NULL);
  file->name = strdup(name);
  file->hdr = elfsh_get_hdr(file);
  file->rights = O_RDONLY;
  if (file->hdr == NULL || file->name == NULL)
    return (NULL);
  return (file);
}



/* Free the ELF descriptor and its fields */
void		elfsh_unload_obj(elfshobj_t *file)
{
  elfshsect_t	*sect;
  elfshsect_t	*next;

  free(file->pht);
  free(file->sht);
  for (sect = file->sectlist; sect; sect = next)
    {
      free(sect->data);
      free(sect->name);
      next = sect->next;
      free(sect);
    }
  close(file->fd);
  free(file->name);
  free(file);
}
