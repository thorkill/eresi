/*
** sanitize.c for elfsh
** 
** Started on  Sun Mar 30 04:13:03 2003 mayhem
** Last update Sun Mar 30 04:28:10 2003 mayhem
*/
#include "libelfsh.h"


/* Update the sh_name field in the section header table entry */
void		elfsh_update_nameidx(elfshobj_t *file, int offset, int len)
{
  int		index;

  for (index = 0; index < file->hdr->e_shnum; index++)
    if (file->sht[index].sh_name > offset)
      file->sht[index].sh_name -= len;
}

/* Update the sh_link for all sections linked to a specific shifted one */
void		elfsh_update_linkidx(elfshobj_t *file, int low_index, int diff)
{
  int		index;

  for (index = 0; index < file->hdr->e_shnum; index++)
    if (file->sht[index].sh_link && file->sht[index].sh_link >= low_index)
      file->sht[index].sh_link += diff;
}

/* Update the sh_link for all section linked to a specific one */
void		elfsh_update_linkidx_equ(elfshobj_t *file, int idx, int diff)
{
  int		index;

  if (file->hdr->e_shstrndx == idx)
    file->hdr->e_shstrndx += diff;
  for (index = 0; index < file->hdr->e_shnum; index++)
    if (file->sht[index].sh_link && file->sht[index].sh_link == idx)
      file->sht[index].sh_link += diff;
}

/* Update the sh_link for all section linked to a specific one */
void		elfsh_update_symlinkidx_equ(elfshobj_t *file, int idx, int diff)
{
  elfshsect_t	*s;
  int		index;
  u_int		size;
  Elf32_Sym	*sym;

  /* If there is no symtab, no need to fix st_shndx fields */
  s = file->secthash[ELFSH_SECTION_SYMTAB];
  if (s != NULL)
    {
      sym = s->data;
      size = s->shdr->sh_size / ELFSH_SYMTAB_ENTRY_SIZE;
      for (index = 0; index < size; index++)
	if (sym[index].st_shndx && sym[index].st_shndx == idx)
	  sym[index].st_shndx += diff;
    }
}
