/*
** pht.c for libelfsh
** 
** Started on  Mon Feb 26 04:07:33 2001 mayhem
** Last update Thu May 15 03:34:32 2003 mayhem
*/
#include "libelfsh.h"



/* 3 tiny functions to retreive the rights for a segment */
int	elfsh_segment_is_readable(Elf32_Phdr *p)
{
  return ((p->p_flags & PF_R));
}

int	elfsh_segment_is_writable(Elf32_Phdr *p)
{
  return ((p->p_flags & PF_W));
}

int	elfsh_segment_is_executable(Elf32_Phdr *p)
{
  return ((p->p_flags & PF_X));
}

/* Tiny write access functions for program headers */
int	elfsh_set_segment_flags(Elf32_Phdr *p, Elf32_Word flags)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_flags = flags;
  return (0);
}

Elf32_Word	elfsh_get_segment_flags(Elf32_Phdr *p)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  return (p->p_flags);
}

int	elfsh_set_segment_align(Elf32_Phdr *p, Elf32_Word align)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_align = align;
  return (0);
}

Elf32_Word	elfsh_get_segment_align(Elf32_Phdr *p)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  return (p->p_align);
}

int	elfsh_set_segment_memsz(Elf32_Phdr *p, Elf32_Word memsz)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_memsz = memsz;
  return (0);
}

Elf32_Word	elfsh_get_segment_memsz(Elf32_Phdr *p)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  return (p->p_memsz);
}

int	elfsh_set_segment_filesz(Elf32_Phdr *p, Elf32_Word filesz)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_filesz = filesz;
  return (0);
}

Elf32_Word	elfsh_get_segment_filesz(Elf32_Phdr *p)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  return (p->p_filesz);
}

int	elfsh_set_segment_paddr(Elf32_Phdr *p, Elf32_Addr paddr)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_paddr = paddr;
  return (0);
}

Elf32_Addr	elfsh_get_segment_paddr(Elf32_Phdr *p)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  return (p->p_paddr);
}

int	elfsh_set_segment_vaddr(Elf32_Phdr *p, Elf32_Addr vaddr)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_vaddr = vaddr;
  return (0);
}

Elf32_Addr	elfsh_get_segment_vaddr(Elf32_Phdr *p)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  return (p->p_vaddr);
}

int		elfsh_set_segment_type(Elf32_Phdr *p, Elf32_Word type)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_type = type;
  return (0);
}

Elf32_Word	elfsh_get_segment_type(Elf32_Phdr *p)
{
 if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
 return (p->p_type);
}

int		elfsh_set_segment_offset(Elf32_Phdr *p, Elf32_Off offset)
{
  if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
  p->p_offset = offset;
  return (0);
}

Elf32_Off	elfsh_get_segment_offset(Elf32_Phdr *p)
{
 if (!p)
    ELFSH_SETERROR("[libelfsh] Invalid NULL argument", -1);
 return (p->p_offset);
}



/* Retreive the 'range'th segment header of type 'type' */
Elf32_Phdr	*elfsh_get_segment_by_type(elfshobj_t *file, int type, int range)
{
  Elf32_Phdr	*pht;
  int		index;
  int		max;
  int		cnt;

  pht = elfsh_get_pht(file, &max);
  if (NULL == pht)
    return (NULL);
  
  for (cnt = index = 0; index < max; index++)
    if (elfsh_get_segment_type(pht + index) == type)
      {
	if (cnt == range)
	  return (&pht[index]);
	cnt++;
      }

  return (NULL);
}

/* Return an array of program (segment) header */
int		elfsh_load_pht(elfshobj_t *file)
{
  int		size;
  Elf32_Phdr	*p;

  if (NULL == file)
    ELFSH_SETERROR("[libelfsh:load_pht] Invalid NULL parameter\n", -1);
  else if (file->pht != NULL)
    return (0);
  else if (!file->hdr->e_phoff)
    ELFSH_SETERROR("[libelfsh:load_pht] No PHT\n", -1);

  size = file->hdr->e_phentsize * file->hdr->e_phnum;
  XSEEK(file->fd, file->hdr->e_phoff, SEEK_SET, -1);
  XALLOC(file->pht, size, -1);
  XREAD(file->fd, file->pht, size, -1);

  /* Deal with cross-endianess binaries */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (file->hdr->e_ident[EI_DATA] == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (file->hdr->e_ident[EI_DATA] == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    p = file->pht;
    p->p_type = swap32(p->p_type);
    p->p_offset = swap32(p->p_offset);
    p->p_vaddr = swap32(p->p_vaddr);
    p->p_paddr = swap32(p->p_paddr);
    p->p_filesz = swap32(p->p_filesz);
    p->p_memsz = swap32(p->p_memsz);
    p->p_flags = swap32(p->p_flags);
    p->p_align = swap32(p->p_align);
  }
  return (0);
}


/* Find the parent segment for this section */
Elf32_Phdr	*elfsh_get_parent_segment(elfshobj_t *file, elfshsect_t *new)
{
  Elf32_Phdr	*actual;
  int		index;

  /* Sanity checks */
  if (NULL == new || NULL == new->parent || NULL == file ||
      (NULL == new->parent->pht && elfsh_load_pht(new->parent)) < 0)
    ELFSH_SETERROR("[libelfsh:get_parent_segment] Invalid NULL parameter\n",
		   NULL);

  /* Find the parent segment */
  for (index = 0, actual = new->parent->pht; index < new->parent->hdr->e_phnum; index++)
    if (INTERVAL(actual[index].p_vaddr, new->shdr->sh_addr, 
		 actual[index].p_vaddr + actual[index].p_memsz))
      return (actual + index);
  return (NULL);
}


/* Tell if the section belongs to the segment or not */
int		elfsh_segment_is_parent(elfshsect_t *new, Elf32_Phdr *p)
{
  if (INTERVAL(p->p_offset, new->shdr->sh_offset, p->p_offset + p->p_filesz))
    return (1);
  return (0);
}


/* Get the base virtual address for an object */
Elf32_Addr	elfsh_get_object_baseaddr(elfshobj_t *file)
{
  u_int		nbr;
  u_int		index;
  u_int		vaddr;

  if (file == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  if (elfsh_get_pht(file, &nbr) == NULL)
    ELFSH_SETERROR("[libelfsh] Cannot read PHT\n", -1);

  vaddr = (u_int) -1;
  for (index = 0; index < nbr; index++)
    if (file->pht[index].p_type == PT_LOAD && file->pht[index].p_vaddr < vaddr)
      vaddr = file->pht[index].p_vaddr;
  return (vaddr);
}


/* Return a ptr on the program header table */
void	*elfsh_get_pht(elfshobj_t *file, int *num)
{
  if (file->pht == NULL && elfsh_load_pht(file) < 0)
    return (NULL);
  if (num != NULL)
    *num = file->hdr->e_phnum;
  return (file->pht);
}


/* Return a entry giving its parent and its index */
Elf32_Phdr	*elfsh_get_pht_entry_by_index(Elf32_Phdr *pht, u_int index)
{
  return (pht + index);
}













