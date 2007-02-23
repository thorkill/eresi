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
 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,file, sizeof(elfshobj_t), NULL);
  XOPEN(file->fd, name, O_RDONLY, 0, NULL);

  file->name = aproxy_strdup(name);
 

  /* Get the file size on disk */
  if (0 != fstat(file->fd,&file->fstat))
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get fstat(2)", NULL);

  file->hdr = elfsh_get_hdr(file);
  file->rights = O_RDONLY;
  if (file->hdr == NULL || file->name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get ELF header", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file));
}



/* Free the ELF descriptor and its fields */
void		elfsh_unload_obj(elfshobj_t *file)
{
  elfshsect_t	*sect;
  elfshsect_t	*next;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(__FILE__, __FUNCTION__, __LINE__,file->hdr);
  XFREE(__FILE__, __FUNCTION__, __LINE__,file->pht);
  XFREE(__FILE__, __FUNCTION__, __LINE__,file->sht);
  for (sect = file->sectlist; sect; sect = next)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__,sect->data);
      if (sect->altdata)
	XFREE(__FILE__, __FUNCTION__, __LINE__,sect->altdata);
      if (sect->terdata)
	XFREE(__FILE__, __FUNCTION__, __LINE__,sect->terdata);
      XFREE(__FILE__, __FUNCTION__, __LINE__,sect->name);
      next = sect->next;
      XFREE(__FILE__, __FUNCTION__, __LINE__,sect);
    }
  if (file->fd)
    close(file->fd);
  XFREE(__FILE__, __FUNCTION__, __LINE__,file->name);
  XFREE(__FILE__, __FUNCTION__, __LINE__,file);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}
