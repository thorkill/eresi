/*
** dynamic.c for libelfsh
** 
** Started on  Tue Feb 27 04:36:53 2001 mayhem
**
*/
#include "libelfsh.h"


/* Change endianess of .dynamic */
int		elfsh_endianize_dynamic(elfshsect_t *new)
{	  
  elfsh_Dyn	*dyn;
  u_int		idx;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity check */
  if (!new)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);

#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (new->parent->hdr->e_ident[EI_DATA] == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (new->parent->hdr->e_ident[EI_DATA] == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    dyn = elfsh_get_raw(new);
    for (idx = 0; idx < new->shdr->sh_size / sizeof(elfsh_Dyn); idx++)
      {
	dyn[idx].d_tag      = swaplong(dyn[idx].d_tag);
	dyn[idx].d_un.d_val = swaplong(dyn[idx].d_un.d_val);
      }
  }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Return a ptr on the dynamic section */
elfsh_Dyn	*elfsh_get_dynamic(elfshobj_t *file, int *num)
{
  elfshsect_t	*new = NULL; /* to shut gcc up with -Wall */
  int		nbr;
  elfsh_Dyn	*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DYNAMIC] == NULL)
    {
      new = elfsh_get_section_by_type(file, SHT_DYNAMIC, NULL, NULL, &nbr, 0);
      if (new == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get .dynamic by type", NULL);
      file->secthash[ELFSH_SECTION_DYNAMIC] = new;
    }    
  
  new = file->secthash[ELFSH_SECTION_DYNAMIC];
  nbr = file->secthash[ELFSH_SECTION_DYNAMIC]->shdr->sh_size / 
    sizeof(elfsh_Dyn);
  if (num != NULL)
    *num = nbr;

  if (new->data == NULL)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load .dynamic", NULL);
      elfsh_endianize_dynamic(new);
    }

  ret = elfsh_get_raw(new);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/* Return the tag field of the dynamic entry */
elfsh_Sword	elfsh_get_dynentry_type(elfsh_Dyn *d)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (d->d_tag));
}

/* Change the tag field of the dynamic entry */
int		elfsh_set_dynentry_type(elfsh_Dyn *d, elfsh_Addr tag)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  d->d_tag = (elfsh_Sword) tag;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return the tag field of the dynamic entry */
elfsh_Word	elfsh_get_dynentry_val(elfsh_Dyn *d)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (d->d_un.d_val));
}

/* Change the val field of the dynamic entry */
int		elfsh_set_dynentry_val(elfsh_Dyn *d, elfsh_Addr val)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (d == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  d->d_un.d_val = (elfsh_Word) val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Retreive the information giving the entry */
char		*elfsh_get_dynentry_string(elfshobj_t *file, elfsh_Dyn *ent)
{
  void		*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  if (file->secthash[ELFSH_SECTION_DYNSYM] == NULL && 
      elfsh_get_dynsymtab(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNSYM", NULL);

  if (ent->d_un.d_val > file->secthash[ELFSH_SECTION_DYNSTR]->shdr->sh_size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid .dynstr offset", NULL);

  data = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSTR]);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((char *) data + ent->d_un.d_val));
}


/* Get .dynamic entries by type */
elfsh_Dyn	*elfsh_get_dynamic_entry_by_type(elfshobj_t *file, 
						 elfsh_Word type)
{
  int		index;
  int		size;
  elfsh_Dyn	*table;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  table = elfsh_get_dynamic(file, &size);
  if (!table)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DYNAMIC", NULL);

  for (index = 0; index < size; index++)
    if (elfsh_get_dynentry_type(table + index) == type)
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (table + index));
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "No dynamic entry with that type", NULL);
}


/* Return a elfsh_Dyn entry by its index */
elfsh_Dyn	*elfsh_get_dynamic_entry_by_index(elfsh_Dyn *dynamic, 
						  elfsh_Addr index)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (dynamic + index));
}


/* Say if the entry is a pointer */
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



/* Shift the .dynamic section in ET_DYN files */
int		elfsh_shift_dynamic(elfshobj_t *file, u_int size)
{
  elfsh_Dyn	*dyn;
  u_int		nbr;
  u_int		idx;
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  dyn = elfsh_get_dynamic(file, &nbr);
  if (dyn == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot find .dynamic in ET_DYN", -1);
  for (idx = 0; idx < nbr; idx++)
    if (elfsh_shiftable_dynent(dyn + idx))
      elfsh_set_dynentry_val(dyn + idx,
			     elfsh_get_dynentry_val(dyn + idx) + size);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}








