/**
 * @file dynamic.c
 * @ingroup libelfsh
** dynamic.c for libelfsh
** 
** Started on  Tue Feb 27 04:36:53 2001 jfv
** 
**
** $Id: dynamic.c,v 1.11 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * Change endianess of .dynamic 
 * @param newent
 * @return
 */
int		elfsh_endianize_dynamic(elfshsect_t *newent)
{	  
  elfsh_Dyn	*dyn;
  unsigned int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity check */
  if (!newent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);

#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (newent->parent->hdr->e_ident[EI_DATA] == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (newent->parent->hdr->e_ident[EI_DATA] == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    dyn = (elfsh_Dyn *) elfsh_get_raw(newent);
    for (idx = 0; idx < newent->shdr->sh_size / sizeof(elfsh_Dyn); idx++)
      {
	dyn[idx].d_tag      = swaplong(dyn[idx].d_tag);
	dyn[idx].d_un.d_val = swaplong(dyn[idx].d_un.d_val);
      }
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Return a ptr on the dynamic section 
 * @param file
 * @param num
 * @return
 */
elfsh_Dyn	*elfsh_get_dynamic(elfshobj_t *file, unsigned int *num)
{
  elfshsect_t	*newent = NULL; /* to shut gcc up with -Wall */
  int		nbr;
  elfsh_Dyn	*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DYNAMIC] == NULL)
    {
      newent = elfsh_get_section_by_type(file, SHT_DYNAMIC, NULL, NULL, &nbr, 0);
      if (newent == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get .dynamic by type", NULL);
      file->secthash[ELFSH_SECTION_DYNAMIC] = newent;
    }    
  
  newent = file->secthash[ELFSH_SECTION_DYNAMIC];
  nbr = file->secthash[ELFSH_SECTION_DYNAMIC]->shdr->sh_size / 
    sizeof(elfsh_Dyn);
  if (num != NULL)
    *num = nbr;

  if (newent->data == NULL)
    {
      newent->data = elfsh_load_section(file, newent->shdr);
      if (newent->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load .dynamic", NULL);
      elfsh_endianize_dynamic(newent);
    }

  ret = (elfsh_Dyn *) elfsh_get_raw(newent);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/**
 * Return the tag field of the dynamic entry 
 * @param d
 * @return
 */
elfsh_Sword	elfsh_get_dynentry_type(elfsh_Dyn *d)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (d->d_tag));
}

/**
 * Change the tag field of the dynamic entry 
 * @param d
 * @param tag
 * @return
 */
int		elfsh_set_dynentry_type(elfsh_Dyn *d, eresi_Addr tag)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  d->d_tag = (elfsh_Sword) tag;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the tag field of the dynamic entry 
 * @param d
 * @return
 */
elfsh_Word	elfsh_get_dynentry_val(elfsh_Dyn *d)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", 0xFFFFFFFF);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (d->d_un.d_val));
}

/**
 * Change the val field of the dynamic entry 
 * @param d
 * @param val
 * @return
 */
int		elfsh_set_dynentry_val(elfsh_Dyn *d, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  d->d_un.d_val = (elfsh_Word) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Retreive the information giving the entry 
 * @param file elfsh object.
 * @param ent
 * @return
 */
char		*elfsh_get_dynentry_string(elfshobj_t *file, elfsh_Dyn *ent)
{
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      elfsh_get_dynsymtab(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNSYM", NULL);

  if (ent->d_un.d_val > file->secthash[ELFSH_SECTION_DYNSTR]->shdr->sh_size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid .dynstr offset", NULL);

  data = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSTR]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char *) data + ent->d_un.d_val));
}


/**
 * Get .dynamic entries by type 
 * @param file
 * @param type
 * @return
 */
elfsh_Dyn	*elfsh_get_dynamic_entry_by_type(elfshobj_t *file, 
						 elfsh_Word type)
{
  unsigned int		index;
  unsigned int		size;
  elfsh_Dyn	*table;
  elfsh_Sword	res;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_get_dynamic(file, &size);
  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNAMIC", NULL);

  for (index = 0; index < size; index++)
    {
      res = elfsh_get_dynentry_type(table + index);
      if (res != -1 && res == (elfsh_Sword) type)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (table + index));
    }
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No dynamic entry with that type", NULL);
}

/**
 * Return a elfsh_Dyn entry by its index 
 * @param dynamic
 * @param index
 * @return
 */
elfsh_Dyn	*elfsh_get_dynamic_entry_by_index(elfsh_Dyn *dynamic, 
						  eresi_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (dynamic + index));
}

/**
 * Say if the entry is a pointer 
 * @param ent
 * @return
 */
int		elfsh_shiftable_dynent(elfsh_Dyn *ent)
{
  switch (elfsh_get_dynentry_type(ent))
    {
    case DT_INIT:
    case DT_FINI:
    case DT_HASH:
    case DT_STRTAB:
    case DT_SYMTAB:
    case DT_JMPREL:
    case DT_REL:
    case DT_VERNEED:
    case DT_VERSYM:
    case DT_VERDEF:
    case DT_PLTGOT:
      return (1);
    default:
      return (0);
    }
  return (0);
}



/**
 * Shift the .dynamic section in ET_DYN files 
 *
 * @param file
 * @param size
 * @return
 */
int		elfsh_shift_dynamic(elfshobj_t *file, unsigned int size)
{
  elfsh_Dyn	*dyn;
  unsigned int		nbr;
  unsigned int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  dyn = elfsh_get_dynamic(file, &nbr);
  if (dyn == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot find .dynamic in ET_DYN", -1);
  for (idx = 0; idx < nbr; idx++)
    if (elfsh_shiftable_dynent(dyn + idx))
      elfsh_set_dynentry_val(dyn + idx,
			     elfsh_get_dynentry_val(dyn + idx) + size);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/**
 * FIXME: TEST?
 * Just a test 
 * @param robj
 * @param data
 * @param real_index
 * @return
 */
int		elfsh_get_dynent_by_type(elfshobj_t	*robj, 
					 elfsh_Dyn	*data,
					 elfsh_Word	real_index)
{
  elfsh_Dyn	*ent;
  int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ent = elfsh_get_dynamic_entry_by_type(robj, real_index);
  idx = (int) ((char *) ent - (char *) data);
  idx = idx / sizeof(elfsh_Dyn);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
}

