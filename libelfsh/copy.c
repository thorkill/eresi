/**
 * @file copy.c
 * @ingroup libelfsh
** copy.c for elfsh
** 
** Started on  Tue Mar  4 01:15:23 2003 jfv
** Last update Tue Mar 11 21:26:07 2003 jfv
**
** $Id: copy.c,v 1.11 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"

/**
 * Used by elfsh_save_obj(). Do not use outside ELFsh 
 * May not be perfect, to modify if some param is zeroified because of the copy 
 *
 * @param file
 * @return
 */
elfshobj_t	*elfsh_copy_obj(elfshobj_t *file)
{
  elfshobj_t	*copy;
  elfshsect_t	*cur;
  elfshsect_t	*enew;
  elfshsect_t	*tmp;
  int		range;
  int		cnt;
  int 		index;
  elfsh_Phdr 	*actual;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  if (elfsh_read_obj(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't copy file object", NULL);
   

  /* Do copy */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,copy, sizeof(elfshobj_t), NULL);
  copy->name = strdup(file->name);
 
  XALLOC(__FILE__, __FUNCTION__, __LINE__,copy->hdr, sizeof(elfsh_Ehdr), NULL);
  memcpy(copy->hdr, file->hdr, sizeof(elfsh_Ehdr));
  copy->rights = file->rights;
  XALLOC(__FILE__, __FUNCTION__, __LINE__,copy->pht, sizeof(elfsh_Phdr) * file->hdr->e_phnum, NULL);
  memcpy(copy->pht, file->pht, sizeof(elfsh_Phdr) * file->hdr->e_phnum);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,copy->sht, sizeof(elfsh_Shdr) * file->hdr->e_shnum, NULL);
  memcpy(copy->sht, file->sht, sizeof(elfsh_Shdr) * file->hdr->e_shnum);
  copy->read  = file->read;
  copy->shtrm = file->shtrm;
  copy->shtrb = file->shtrb;
  copy->strip = file->strip;
  copy->nbrm  = file->nbrm;
  copy->fstat = file->fstat;
  copy->id = file->id;
  copy->fd = file->fd;
  copy->pending = file->pending;
  copy->linkmap = file->linkmap;
  copy->rhdr.base = file->rhdr.base;
  copy->original = file;
  
  /* Arrange sectlist. Partially ripped from elfsh_add_section() . */
  for (range = 0, cur = file->sectlist; cur; cur = cur->next, range++)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,enew, sizeof(elfshsect_t), NULL);
      enew->shdr = copy->sht + range;
      enew->parent = copy;
      enew->index = range;
      enew->name = strdup(cur->name);
 
      enew->flags = cur->flags;

      if (enew->shdr->sh_size && cur->data)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,enew->data, enew->shdr->sh_size, NULL);
	  memcpy(enew->data, cur->data, enew->shdr->sh_size);
	}

      /* Set phdr pointer */
      for (index = 0, actual = enew->parent->pht; index < enew->parent->hdr->e_phnum;
	   index++)
	if (INTERVAL(actual[index].p_vaddr, enew->shdr->sh_addr,
		     actual[index].p_vaddr + actual[index].p_memsz))
	  enew->phdr = actual + index;

      enew->curend = cur->curend;

      /* Insert enew section in sectlist */
      for (tmp = copy->sectlist; tmp != NULL && tmp->next != NULL; tmp = tmp->next)
	if (tmp->index == range)
	  {
	    /* Insert the enew section */
	    enew->prev = tmp->prev;
	    enew->next = tmp;
	    if (enew->prev != NULL)
	      enew->prev->next = enew;
	    else
	      copy->sectlist = enew;
	    tmp->prev = enew;
	  }
      
      /* The section is the first one inserted */
      if (!tmp)
	{
	  copy->sectlist = enew;
	  copy->sectlist->prev = enew;
	}
      
      /* The section must be inserted at the last place */
      else if (!tmp->next)
	{
	  tmp->next = enew;
	  enew->prev = tmp;
	  copy->sectlist->prev = enew;
	}

      /* Fill secthash */
      for (cnt = 0; cnt < ELFSH_SECTION_MAX; cnt++)
	if (file->secthash[cnt] == cur)
	  copy->secthash[cnt] = enew;
      
    }

  /* XXX-runtimelist to copy as well */
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (copy));
}
