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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(file, sizeof(elfshobj_t), NULL);
  XOPEN(file->fd, name, O_RDONLY, 0, NULL);
  file->name = strdup(name);
  file->hdr = elfsh_get_hdr(file);
  file->rights = O_RDONLY;
  if (file->hdr == NULL || file->name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get ELF header", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (file));
}



/* Free the ELF descriptor and its fields */
void		elfsh_unload_obj(elfshobj_t *file)
{
  elfshsect_t	*sect;
  elfshsect_t	*next;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(file->hdr);
  XFREE(file->pht);
  XFREE(file->sht);
  for (sect = file->sectlist; sect; sect = next)
    {
      XFREE(sect->data);
      if (sect->altdata)
	XFREE(sect->altdata);
      if (sect->terdata)
	XFREE(sect->terdata);
      XFREE(sect->name);
      next = sect->next;
      XFREE(sect);
    }
  if (file->fd)
    close(file->fd);
  XFREE(file->name);
  XFREE(file);
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}
