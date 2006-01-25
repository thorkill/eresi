/*
** copy.c for elfsh
** 
** Started on  Tue Mar  4 01:15:23 2003 mayhem
** Last update Tue Mar 11 21:26:07 2003 mayhem
*/
#include "libelfsh.h"



/* Used by elfsh_save_obj(). Do not use outside ELFsh */
/* May not be perfect, to modify if some param is zeroified because of the copy */
elfshobj_t	*elfsh_copy_obj(elfshobj_t *file)
{
  elfshobj_t	*copy;
  elfshsect_t	*cur;
  elfshsect_t	*new;
  elfshsect_t	*tmp;
  int		range;
  int		cnt;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (!file)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  elfsh_read_obj(file);

  /* Do copy */
  XALLOC(copy, sizeof(elfshobj_t), NULL);
  copy->name = elfsh_strdup(file->name);
  XALLOC(copy->hdr, sizeof(elfsh_Ehdr), NULL);
  memcpy(copy->hdr, file->hdr, sizeof(elfsh_Ehdr));
  copy->rights = file->rights;
  XALLOC(copy->pht, sizeof(elfsh_Phdr) * file->hdr->e_phnum, NULL);
  memcpy(copy->pht, file->pht, sizeof(elfsh_Phdr) * file->hdr->e_phnum);
  XALLOC(copy->sht, sizeof(elfsh_Shdr) * file->hdr->e_shnum, NULL);
  memcpy(copy->sht, file->sht, sizeof(elfsh_Shdr) * file->hdr->e_shnum);
  copy->read  = file->read;
  copy->shtrm = file->shtrm;
  copy->strip = file->strip;
  copy->nbrm  = file->nbrm;
  copy->pending = file->pending;
  copy->linkmap = file->linkmap;
  copy->rhdr.base = file->rhdr.base;
  
  /* Arrange sectlist. Partially ripped from elfsh_add_section() . */
  for (range = 0, cur = file->sectlist; cur; cur = cur->next, range++)
    {
      XALLOC(new, sizeof(elfshsect_t), NULL);
      new->shdr = copy->sht + range;
      new->parent = copy;
      new->index = range;
      new->name = elfsh_strdup(cur->name);
      new->flags = cur->flags;

      if (new->shdr->sh_size && cur->data)
	{
	  XALLOC(new->data, new->shdr->sh_size, NULL);
	  memcpy(new->data, cur->data, new->shdr->sh_size);
	}

      new->phdr = copy->pht + ((cur->phdr - file->pht) / sizeof(elfsh_Phdr));
      new->curend = cur->curend;

      /* Insert new section in sectlist */
      for (tmp = copy->sectlist; tmp != NULL && tmp->next != NULL; tmp = tmp->next)
	if (tmp->index == range)
	  {
	    /* Insert the new section */
	    new->prev = tmp->prev;
	    new->next = tmp;
	    if (new->prev != NULL)
	      new->prev->next = new;
	    else
	      copy->sectlist = new;
	    tmp->prev = new;
	  }
      
      /* The section is the first one inserted */
      if (!tmp)
	{
	  copy->sectlist = new;
	  copy->sectlist->prev = new;
	}
      
      /* The section must be inserted at the last place */
      else if (!tmp->next)
	{
	  tmp->next = new;
	  new->prev = tmp;
	  copy->sectlist->prev = new;
	}

      /* Fill secthash */
      for (cnt = 0; cnt < ELFSH_SECTION_MAX; cnt++)
	if (file->secthash[cnt] == cur)
	  copy->secthash[cnt] = new;
      
    }

  /* XXX-runtimelist to copy as well */
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (copy));
}
