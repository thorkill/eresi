/*
** raw.c for libelfsh
** 
** Started on  Wed Feb 27 19:41:45 2002 mayhem
** Last update Thu Mar 20 05:47:12 2003 mayhem
*/
#include "libelfsh.h"



/* Perform a raw write on the object cache data */
int		elfsh_raw_write(elfshobj_t	*file, 
				u_int		foffset, 
				void		*src_buff, 
				int		len)
{
  elfshsect_t	*sect;
  int		sect_off;
  void		*dst;

  sect = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (sect == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid virtual address\n", -1);

  sect_off = foffset - sect->shdr->sh_offset;
  if (sect_off + len > sect->shdr->sh_size)
    ELFSH_SETERROR("[libelfsh] Section too short\n", -1);

  dst = elfsh_get_anonymous_section(file, sect);
  if (dst == NULL)
    return (0);
  
  memcpy(dst + sect_off, src_buff, len);
  return (len);
}



/* Perform a raw read on the object cache data */
int		elfsh_raw_read(elfshobj_t *file, u_int foffset, void *dest_buff, int len)
{
  elfshsect_t	*sect;
  void		*src;
  int		sect_off;

  sect = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (sect == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid virtual address\n", -1);

  sect_off = foffset - sect->shdr->sh_offset;

  if (sect_off + len > sect->shdr->sh_size)
    len -= (sect_off + len - sect->shdr->sh_size);
  
  src = elfsh_get_anonymous_section(file, sect);
  if (src == NULL)
    return (0);

  memcpy(dest_buff, src + sect_off, len);
  return (len);

}


/* Retreive the file offset from the virtual address */
int		elfsh_get_foffset_from_vaddr(elfshobj_t *file, u_int vaddr)
{
  elfshsect_t	*actual;
  
  if (!vaddr)
    return (0);
  for (actual = file->sectlist; actual; actual = actual->next)
    if (INTERVAL(actual->shdr->sh_addr, vaddr, actual->shdr->sh_addr + actual->shdr->sh_size))
      return (actual->shdr->sh_offset + (vaddr - actual->shdr->sh_addr));
  return (0);
}


/* Retreive the virtual address given the file offset */
int		elfsh_get_vaddr_from_foffset(elfshobj_t *file, u_int foffset)
{
  elfshsect_t	*root;

  root = elfsh_get_parent_section_by_foffset(file, foffset, NULL);
  if (root)
    return (root->shdr->sh_addr + (foffset - root->shdr->sh_offset));
  return (0);
}
