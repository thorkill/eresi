/**
 * @file sanitize.c
 * @ingroup libelfsh
** sanitize.c for elfsh
** 
** Started on  Sun Mar 30 04:13:03 2003 jfv
** 
**
** $Id: sanitize.c,v 1.7 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * @brief Update the sh_name field in the section header table entry
 * @param file
 * @param offset
 * @param len
 * @return
 */
void		elfsh_update_nameidx(elfshobj_t *file, int offset, int len)
{
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < file->hdr->e_shnum; index++)
    if (file->sht[index].sh_name > offset)
      file->sht[index].sh_name -= len;
  
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Update the sh_link for all sections linked to a specific shifted one
 * @param file
 * @param low_index
 * @param diff
 * @return
 */
void		elfsh_update_linkidx(elfshobj_t *file, int low_index, int diff)
{
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < file->hdr->e_shnum; index++)
    if (file->sht[index].sh_link && file->sht[index].sh_link >= low_index)
      file->sht[index].sh_link += diff;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Update the sh_link for all section linked to a specific one
 * @param file
 * @param idx
 * @param diff
 * @return
 */
void		elfsh_update_linkidx_equ(elfshobj_t *file, int idx, int diff)
{
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_shstrndx == idx)
    file->hdr->e_shstrndx += diff;
  for (index = 0; index < file->hdr->e_shnum; index++)
    if (file->sht[index].sh_link && file->sht[index].sh_link == idx)
      file->sht[index].sh_link += diff;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Update the sh_link for all section linked to a specific one 
 * @param file
 * @param idx
 * @param diff
 * @return
 */
void		elfsh_update_symlinkidx_equ(elfshobj_t *file, int idx, int diff)
{
  elfshsect_t	*s;
  int		index;
  unsigned int		size;
  elfsh_Sym	*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Update sh_link fields for the given section 
 * @param file
 * @param tab
 * @param idx
 * @param diff
 * @return
 */
void		elfsh_update_shidx(elfshobj_t *file, elfshsect_t *tab, int idx, int diff)
{
  elfshsect_t	*s;
  int		index;
  unsigned int		size;
  elfsh_Sym	*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If there is no symtab, no need to fix st_shndx fields */
  s = tab;
  if (s != NULL)
    {
      sym = s->data;
      size = s->shdr->sh_size / ELFSH_SYMTAB_ENTRY_SIZE;
      for (index = 0; index < size; index++)
	if (sym[index].st_shndx && sym[index].st_shndx > idx)
	  sym[index].st_shndx += diff;
    }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Update the sh_link for all section linked to a specific one 
 * @param file
 * @param idx
 * @param diff
 * @return
 */
void		elfsh_update_dynsym_shidx(elfshobj_t *file, int idx, int diff)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If there is no symtab, no need to fix st_shndx fields */
  s = file->secthash[ELFSH_SECTION_DYNSYM];
  if (s != NULL)
    elfsh_update_shidx(file, s, idx, diff);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Update the sh_link for all section linked to a specific one 
 * @param file
 * @param idx
 * @param diff
 */
void		elfsh_update_symtab_shidx(elfshobj_t *file, int idx, int diff)
{
  elfshsect_t	*s;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If there is no symtab, no need to fix st_shndx fields */
  s = file->secthash[ELFSH_SECTION_SYMTAB];
  if (s != NULL)
    elfsh_update_shidx(file, s, idx, diff);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

