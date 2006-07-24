/*
** got.c for elfsh
**
** Started on  Sun Jun 24 21:30:41 2001 mayhem
** Last update Thu May 15 04:39:15 2003 mayhem
*/
#include "libelfsh.h"




/* Shift GOT on ET_DYN */
int		elfsh_shift_got(elfshobj_t *file, u_int size)
{
  elfshsect_t	*got;
  int		nbr;
  u_int		idx;
  elfsh_Addr	*addr;


  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if	__DEBUG_ETRELintoETDYN__
  printf("[DEBUG_ETRELintoETDYN] Shifting GOT from %u bytes \n", size);
#endif

  got = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_GOT, 
				  NULL, NULL, &nbr);
  if (!got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Cannot retreive GOT in ET_DYN", -1);
  nbr = nbr / sizeof(elfsh_Addr);
  for (idx = 0; idx < nbr; idx++)
    {
      addr = elfsh_get_got_entry_by_index(got->data, idx);
      if (*addr)
	*addr += size;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}

/* Shift ALTGOT on ET_DYN */
int		elfsh_shift_altgot(elfshobj_t *file, u_int size)
{
  elfshsect_t	*got;
  int					nbr;
  u_int				idx;
  elfsh_Addr	*addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if	__DEBUG_ETRELintoETDYN__
  printf("[DEBUG_ETRELintoETDYN] Shifting ALTGOT from %u bytes \n", size);
#endif

  got = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTGOT, 
				  NULL, NULL, &nbr);
  if (!got)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
  nbr = nbr / sizeof(elfsh_Addr);
  for (idx = 0; idx < nbr; idx++)
    {
      addr = elfsh_get_got_entry_by_index(got->data, idx);
      if (*addr)
	*addr += size;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/* Change endianess of GOT */  
int		elfsh_endianize_got(elfshsect_t *new)
{
  elfsh_Addr	*cur;  
  int		idx;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
    cur = elfsh_get_raw(new);
    for (idx = 0; idx < new->shdr->sh_size / new->shdr->sh_entsize; idx++)
      cur[idx] = swaplong(cur[idx]);
  }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  


/* Read the global offset table in section .got */
elfsh_Addr     	*elfsh_get_got(elfshobj_t *file, int *num)
{
  elfshsect_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  else if (file->sectlist == NULL && elfsh_read_obj(file) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to read object", NULL);

  /* Read GOT */
  /* Now called .got.plt on new ld */
  new = file->secthash[ELFSH_SECTION_GOT];
  if (new == NULL)
    {
      new = elfsh_get_gotsct(file);
      if (new == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get GOT by name", NULL);
    }
 
  /* Load GOT data */
  if (new->data == NULL)
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (new->data == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load GOT", NULL);
      file->secthash[ELFSH_SECTION_GOT] = new;

      /* Arrange endianess if necessary */
      elfsh_endianize_got(new);
    }

  /* Final things */
  if (num != NULL)
    *num = new->shdr->sh_size / new->shdr->sh_entsize;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_raw(new)));
}
 


 
/* Return the real GOT section */
elfshsect_t	*elfsh_get_gotsct(elfshobj_t *file)
  {
    elfshsect_t *new;
   ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);    
    
    new = file->secthash[ELFSH_SECTION_GOT];
    if (new == NULL)
     {
       new = elfsh_get_section_by_name(file, 
				       ELFSH_SECTION_NAME_GOTPLT, 
				       NULL, NULL, NULL);
       if (new == NULL)
	 {
	   new = elfsh_get_section_by_name(file, 
					   ELFSH_SECTION_NAME_GOT, 
					   NULL, NULL, NULL);
	   if (new == NULL)
	     ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			       "Unable to get GOT by name", NULL);
	 }
     }
   ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
 }
 

/* Return the 'range'th got - seems ok */
elfshsect_t     *elfsh_get_got_by_idx(elfshobj_t *file, 
				      elfsh_Addr range, 
				      u_int	 *nbr)
{
  elfshsect_t	*got;
  elfshsect_t	*cursect;
  u_int		idx;
  char		*name;
  elfsh_Addr	rank = 0;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == 0x0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No file selected", NULL);
  
  if (file->sectlist == NULL && elfsh_get_sht(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  for (idx = 0; idx < elfsh_get_shtnbr(file->hdr); idx++)
    {
      
      cursect = elfsh_get_section_by_index(file, idx, NULL, NULL);
      name = cursect->name;

      if (strstr(name, "got"))
	{
	  
	  got = elfsh_get_section_by_name(file, name,
					  NULL, NULL, NULL);
	  if (got == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to get GOT by name", NULL);	  
	  
	  /* Load GOT data */
	  if (got->data == NULL)
	    {

	      got->data = elfsh_load_section(file, got->shdr);
	     
	      if (got->data == NULL)
		ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
				  "Unable to load got", NULL);
	      
	      // file->secthash[ELFSH_SECTION_GOT] = got;
	      
	      /* Arrange endianess if necessary */
	      elfsh_endianize_got(got);
	    }
	  
	  if (rank == range)
	    {
	      if (nbr != NULL)
		*nbr =  got->shdr->sh_size / got->shdr->sh_entsize;
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (got));
	    }

	  rank++;	  
	}
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Cannot reach requested range", NULL);
}
  



/* Modify a GOT entry */
int		elfsh_set_got_entry_by_index(elfshobj_t *file, 
					     int i, 
					     elfsh_Addr n)
{
  int		nbr;
  elfsh_Addr	*got;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  got = elfsh_get_got(file, &nbr);
  if (NULL == got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get GOT", -1);
  if (i >= nbr)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GOT index too big", -1);
  got[i] = n;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Modify the got entry for the dynamic symbol 'n' */
int		elfsh_set_got_entry_by_name(elfshobj_t *f, 
					    char *n, 
					    elfsh_Addr a)
{
  elfsh_Addr	*got;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (f == NULL || n == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  got = elfsh_get_got_entry_by_name(f, n);
  if (got == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get GOT entry by name", -1);
  *got = a;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Return a entry giving its parent and its index */
elfsh_Addr     	*elfsh_get_got_entry_by_index(elfsh_Addr *got, 
					      elfsh_Addr index)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (got == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (got + index));
}



/* Return a entry giving its parent and its index */
elfsh_Addr     	*elfsh_get_got_entry_by_name(elfshobj_t *file, char *name)
{
  int		nbr;
  u_int		index;
  elfsh_Addr	*got;
  elfsh_Sym	*sym;
  u_int		sz;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || name == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  got = elfsh_get_got(file, &nbr);
  sym = elfsh_get_dynsymbol_by_name(file, name);
  if (got == NULL || sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find symbol", NULL);

  /* Get PLT info */
  if (!elfsh_is_pltentry(file, sym))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Symbol is not PLT entry", NULL);
  sz = elfsh_get_pltentsz(file);
  
  /* Find the entry */
  for (index = 0; index < nbr; index++)
    if (INTERVAL(sym->st_value, got[index], sym->st_value + sz))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (got + index));
    
  /* Not found */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "GOT entry not found", NULL);
}




/* Return a entry giving its parent and its index : used as INTERNAL hash handler */
int		elfsh_set_got_entry(elfsh_Addr *got, elfsh_Addr vaddr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  *got = vaddr;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Return a entry value giving its parent and its index : used as INTERNAL 
   hash handler */
elfsh_Addr		elfsh_get_got_entry(elfsh_Addr *got)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*got));
}



/* Return a GOT entry value */
elfsh_Addr		elfsh_get_got_val(elfsh_Addr *got)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*got));
}



/* Return a GOT entry address */
elfsh_Addr		elfsh_get_got_addr(elfsh_Addr *got)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*got));
}



/* Change a GOT entry val */
u_int			elfsh_set_got_val(elfsh_Addr *got, elfsh_Addr val)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  *got = val;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Change a GOT entry addr */
u_int			elfsh_set_got_addr(elfsh_Addr *got, elfsh_Addr val)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  // XXX Not yet implemented ... and not going to be
  printf("[DEBUG] elfsh_set_got_addr\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/*
** Only used on BeoS 
** Tested and developed by zadig@myrealbox.com
**
*/
int   elfsh_get_got_symbol_reloc(elfshobj_t	*file,
				 uint8		*name,
                                 elfsh_Rel	*rel_entry)
{
   elfshsect_t *got, *rel_got;
   u_int       index, got_rel_index;
   u_int       entsz;
   int         got_index;
   elfsh_Off   off;
   void		*data;
   
   ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
   
   /* Sanity checks */
   entsz = elfsh_get_pltentsz(file);
   if (entsz < 0)
     ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid PLT entry size", -1);

  if (NULL == elfsh_get_dynsymtab(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get DYNSYM", -1);

  if (NULL == elfsh_get_symtab(file, NULL))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get SYMTAB", -1);
  
  /* get got section index */
  got = elfsh_get_section_by_name(file,
				  ELFSH_SECTION_NAME_GOT,
				  &got_index, NULL, NULL);
  if (got == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get GOT", -1);
  
  /* search the got relocation section */
  rel_got = NULL;
  for (got_rel_index = 0; got_rel_index < file->hdr->e_shnum; got_rel_index++)
    {
      rel_got = elfsh_get_section_by_index(file, got_rel_index,
                                          NULL, NULL);
    if (rel_got->shdr->sh_info == got_index)
      break;
    }
  
  if ((rel_got == NULL) || (rel_got->shdr->sh_info != got_index))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get GOT relocations", -1);
  
  /* search for the requested symbol */
  off = entsz;
  data = elfsh_get_raw(rel_got);
  for (index = 0; 
       index < (rel_got->shdr->sh_size / rel_got->shdr->sh_entsize); 
       off += entsz, index ++)
    {
      elfsh_Rel *cur_rel;
      cur_rel = ((elfsh_Rel*) data) + index;
      if(strcmp(elfsh_get_symname_from_reloc(file, cur_rel), name) == 0)
	{
	  *rel_entry = *cur_rel;
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
    }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Symbol not found", -1);
}




