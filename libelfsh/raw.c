/**
 * @file raw.c
** raw.c for libelfsh
** 
** Started on  Wed Feb 27 19:41:45 2002 jfv
** Last update Thu Mar 20 05:47:12 2003 jfv
**
** $Id: raw.c,v 1.5 2007-07-31 03:28:46 may Exp $
**
*/
#include "libelfsh.h"



/**
 * Perform a raw write on the object cache data 
 */
int		elfsh_raw_write(elfshobj_t	*file, 
				u_int		foffset, 
				void		*src_buff, 
				int		len)
{
  elfshsect_t	*sect;
  int		sect_off;
  void		*dst;
  //int		prot;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
      //prot = elfsh_munprotect(dst + sect_off, len);
      memcpy(dst + sect_off, src_buff, len);
      //elfsh_mprotect(dst + sect_off, len, prot);
    }
  else
    memcpy(dst + sect_off, src_buff, len);

  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (len));
}



/**
 * Perform a raw read on the object cache data 
 */
int		elfsh_raw_read(elfshobj_t *file, u_int foffset, void *dest_buff, int len)
{
  elfshsect_t	*sect;
  void		*src;
  int		sect_off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
 */
int		elfsh_get_foffset_from_vaddr(elfshobj_t *file, elfsh_Addr vaddr)
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
 */
int		elfsh_get_vaddr_from_foffset(elfshobj_t *file, u_int foffset)
{
  elfshsect_t	*root;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  root = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (root)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (root->shdr->sh_addr + (foffset - root->shdr->sh_offset)));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
