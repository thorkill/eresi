/**
 * @file raw.c
 * @ingroup libelfsh
** raw.c for libelfsh
** 
** Started on  Wed Feb 27 19:41:45 2002 jfv
** Last update Thu Mar 20 05:47:12 2003 jfv
**
** $Id: raw.c,v 1.8 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * @brief Get the buffered address from the real virtual address 
 */
void		*elfsh_get_raw_by_addr(elfshobj_t *current, void *addr)
{
  elfshsect_t	*sect;
  elfsh_SAddr	offset;
#if defined(KERNSH)
  void		*dataptr;
#endif

  sect = elfsh_get_parent_section(current, (eresi_Addr) addr, &offset);

#if defined(KERNSH)
  dataptr = kernsh_get_raw_by_addr(addr);
  if (dataptr != NULL)
    return dataptr;
#endif

  /* This happens when the object is a ERESI variable */
  if (!sect)
    return (addr);

  if (elfsh_is_debug_mode())
    {
      if (!elfsh_section_is_runtime(sect))
	return ((void *) (sect->parent->rhdr.base + sect->shdr->sh_addr + offset));
      else if (!sect->shdr->sh_addr)
	return ((void *) (char *) sect->data + offset);
      else
	return ((void *) (sect->shdr->sh_addr + offset));
    }
  else
    return ((void *) (char *) sect->data + offset);
  
}


/**
 * @brief Nice embedded debugging trick : return a pointer on the section data.
 * @brief This function makes the difference between static data and runtime data.
 * @param sect Section to return the data buffer from.
 */
void			*elfsh_get_raw(elfshsect_t *sect)
{
  void			*dataptr = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(KERNSH)
  dataptr = kernsh_get_raw(sect);
  if (dataptr != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dataptr);
#endif
  
  if (elfsh_is_debug_mode())
    {

      /* The address of the section */
      dataptr = (void *) sect->shdr->sh_addr;
      
      /* For runtime injected sections, do not add the base address of the object */
      if (!elfsh_section_is_runtime(sect))
	dataptr += sect->parent->rhdr.base;

      /* For unmapped sections */
      if (!dataptr)
	dataptr = sect->data;

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dataptr);
    }
  if (sect)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->data));

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid parameter", NULL);
}


/**
 * Perform a raw write on the object cache data 
 * @param file
 * @param foffset
 * @param src_buff
 * @param len
 * @return
 */
int		elfsh_raw_write(elfshobj_t	*file, 
				unsigned int		foffset, 
				void		*src_buff, 
				int		len)
{
  elfshsect_t	*sect;
  int		sect_off;
  void		*dst;
  int		prot;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(KERNSH)
  if (kernsh_raw_write(file, foffset, src_buff, len) == len)
    {
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, len);
    }
#endif

  sect = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid virtual address", -1);

  sect_off = foffset - sect->shdr->sh_offset;
  if (sect_off + len > sect->shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section too short", -1);

  dst = elfsh_get_anonymous_section(file, sect);
  if (dst == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (elfsh_is_debug_mode())
    {
      prot = elfsh_munprotect(file, (eresi_Addr) dst + sect_off, len);
      memcpy(dst + sect_off, src_buff, len);
      elfsh_mprotect((eresi_Addr) dst + sect_off, len, prot);
    }
  else
    memcpy(dst + sect_off, src_buff, len);

  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (len));
}



/**
 * Perform a raw read on the object cache data 
 * @param file
 * @param foffset
 * @param dest_buff
 * @param len
 * @return
 */
int		elfsh_raw_read(elfshobj_t *file, unsigned int foffset, void *dest_buff, int len)
{
  elfshsect_t	*sect;
  void		*src;
  int		sect_off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(KERNSH)
  if (kernsh_raw_read(file, foffset, dest_buff, len) == len)
    {
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, len);
    }
#endif

  sect = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid virtual address", -1);

  sect_off = foffset - sect->shdr->sh_offset;
  if (sect_off + len > sect->shdr->sh_size)
    len -= (sect_off + len - sect->shdr->sh_size);
  
  src = elfsh_get_anonymous_section(file, sect);
  if (src == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  memcpy(dest_buff, src + sect_off, len);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (len));
}


/**
 * Retreive the file offset from the virtual address 
 * @param file
 * @param vaddr
 * @return
 */
int		elfsh_get_foffset_from_vaddr(elfshobj_t *file, eresi_Addr vaddr)
{
  elfshsect_t	*actual;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!vaddr)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  for (actual = file->sectlist; actual; actual = actual->next)
    if (INTERVAL(actual->shdr->sh_addr, vaddr, 
		 actual->shdr->sh_addr + actual->shdr->sh_size))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (actual->shdr->sh_offset + 
			  (vaddr - actual->shdr->sh_addr)));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Retreive the virtual address given the file offset 
 * @param file
 * @param foffset
 * @return
 */
int		elfsh_get_vaddr_from_foffset(elfshobj_t *file, unsigned int foffset)
{
  elfshsect_t	*root;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  root = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (root)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (root->shdr->sh_addr + (foffset - root->shdr->sh_offset)));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
