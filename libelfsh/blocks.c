/*
** blocks.c for libelfsh
** 
** Started on  Sun Jul 22 05:02:47 2001 mayhem
** Last update Sun May 25 19:57:55 2003 mayhem
*/
#include "libelfsh.h"


/* 
** Parse the symbol table looking for non-zero-lenght-blocks symbol and add it 
** to the corresponding parent elfshsect_t object .
*/
int			elfsh_scan_blocks(elfshobj_t *file) 
{
  elfshblock_t		*fct;
  elfshblock_t		*prev_fct;
  Elf32_Sym		*sym;
  int			index;
  char			is_plt_entry;

  /* Some preliminary checks */
  if (file->scanned)
    return (0);
  if (NULL == elfsh_get_symtab(file, NULL) ||
      NULL == elfsh_get_plt(file, NULL))
    return (-1);

  /* Entering the symtab loop ... */
  for (prev_fct = fct = NULL, index = 0; 
       index < file->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / sizeof(Elf32_Sym);
       index++)
    {
      sym = (Elf32_Sym *) file->secthash[ELFSH_SECTION_SYMTAB]->data + index;
      if (elfsh_get_symbol_type(sym) == STT_FUNC)
	{
	  
	  /* Load the corresponding section if requested */
	  XALLOC(fct, sizeof(elfshblock_t), -1);
	  if ((fct->section = elfsh_get_parent_section(file, sym->st_value, NULL)) == NULL)
	    return (-1);
	  if (fct->section->data == NULL)
	    elfsh_get_anonymous_section(file, fct->section);

	  /* Create the elfshblock_t entry */
	  is_plt_entry = elfsh_is_pltentry(file, sym);
	  fct->sym = sym;
	  fct->offset = sym->st_value - fct->section->shdr->sh_addr;
	  fct->len = (is_plt_entry ? ELFSH_PLT_ENTRY_SIZE : fct->sym->st_size);
	  
	  /* Add the elfshblock_t at the end of the list for this section */
	  elfsh_insert_block(fct->section, fct, ELFSH_BLOCK_FIRST);
	}
    }

  /* Everything is okay ! yeah man $B> */
  if (elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]) < 0)
    return (-1);
  file->scanned = 1;
  return (0);
}    









#if 0
/*
** Sort the elfshblock_t array for each executable section in the file .
** -> Sort the section's block list by their address from lower to higher
**
** NOT USED ANYMORE AT THE MOMENT
*/
int			elfsh_sort_blocks(elfshobj_t *file)
{
  elfshblock_t		tmp;
  elfshblock_t		*blk;	
  elfshsect_t		*s;
  int			pass;

  /* Sanity checks */
  if (!file->scanned && elfsh_scan_blocks(file) < 0)
    return (-1);
  
  /* Sort block for each executable section, O(cn²) */
  for (s = file->sectlist; s; s = s->next)
    if (elfsh_get_section_execflag(s->shdr))
      for (pass = s->nbr; pass; pass--)
	for (blk = (void *) s->altdata; blk && blk->next; blk = blk->next)
	  if (blk->sym->st_value > blk->next->sym->st_value)
	    {
	      
	      tmp.sym = blk->sym;
	      tmp.section = blk->section;
	      tmp.offset = blk->offset;
	      tmp.len = blk->len;
	      
	      blk->sym = blk->next->sym;
	      blk->section = blk->next->section;
	      blk->offset = blk->next->offset;
	      blk->len = blk->next->len;
	      
	      blk->next->sym = tmp.sym;
	      blk->next->section = tmp.section;
	      blk->next->offset = tmp.offset;
	      blk->next->len = tmp.len;
	    }
  return (0);
}
#endif





/* Free all block entries for all executable files */
void		elfsh_free_blocks(elfshobj_t *file)
{
  elfshsect_t	*sct;
  elfshsect_t	*tmpsct;
  elfshblock_t	*blk;
  elfshblock_t	*tmpblk;

  /* to shut gcc up */
  tmpsct = NULL;
  tmpblk = NULL;

  for (sct = file->sectlist; sct; sct = tmpsct)
    if (elfsh_get_section_execflag(sct->shdr))
      {
	tmpsct = sct->next;
	for (blk = sct->altdata; blk; blk = tmpblk)
	  {
	    tmpblk = blk->next;
	    free(blk);
	  }
	free(sct);
      }
}
    






/* Return the elfshblock_t giving its address */
elfshblock_t	*elfsh_get_block_by_addr(elfshobj_t *file, int addr)
{
  elfshsect_t	*sct;
  elfshblock_t	*blk;
  
  if (!file->scanned && !elfsh_scan_blocks(file))
    return (NULL);

  for (sct = file->sectlist; sct; sct = sct->next)
    for (blk = sct->data; blk; blk = blk->next)
      if (INTERVAL(blk->sym->st_value, addr, blk->sym->st_value + blk->len))
	return (blk);

  ELFSH_SETERROR("[libelfsh] No block at this address", NULL);
}






/* Return the elfshblock_t whoose address match the 'name' symbol one */
elfshblock_t	*elfsh_get_block_by_name(elfshobj_t *file, char *name)
{
  Elf32_Sym	*sym;
  u_int		type;

  if (!(sym = elfsh_get_symbol_by_name(file, name)))
    ELFSH_SETERROR("[libelfsh] No block by that name", NULL);
  type = elfsh_get_symbol_type(sym);
  if (type == STT_FUNC)
    ELFSH_SETERROR("[libelfsh] symbol is not a block", NULL);
  return (elfsh_get_block_by_addr(file, sym->st_value));
}




/* Return the block lenght */
u_int		elfsh_get_block_len(elfshblock_t *blk)
{
  return (blk->len);
}




/* Return the block address */
Elf32_Addr	elfsh_get_block_addr(elfshblock_t *blk)
{
  return (blk->sym->st_value);
}





/* Insert a block to the section */
/* XXX: add a symbol table entry if there isnt one for it already */
int		elfsh_insert_block(elfshsect_t *sct, elfshblock_t *blk, int index)
{
  elfshblock_t	*tmp;
  char		*name;

  name = elfsh_get_symbol_name(sct->parent, blk->sym);
  if (name == NULL)
    name = ELFSH_NULL_STRING;

#if __DEBUG_BLOCKS__
  printf(" [DEBUG] Inserting block %s \n", name);
#endif

  if (index == ELFSH_BLOCK_FIRST)
    {
      blk->next = sct->altdata;
      sct->altdata = blk;
    }
  else if (index == ELFSH_BLOCK_LAST)
    {
      if (!sct->altdata)
	sct->altdata = blk;
      else
	{
	  tmp = sct->altdata;
	  while (tmp && tmp->next)
	    tmp = tmp->next;
	  tmp->next = blk;
	}
    }

#if __DEBUG_BLOCKS__
  elfsh_print_blocks(sct);
#endif

  return (0);
}




/* Print the block list for debug purposes */
int		elfsh_print_blocks(elfshsect_t *sect)
{
  elfshblock_t	*cur;
  u_int		index;
  char		*name;

  printf("\n [*] List of blocks for section %s \n", sect->name);
  for (index = 0, cur = sect->altdata; cur; cur = cur->next, index++)
    {

      name = elfsh_get_symbol_name(cur->section->parent, cur->sym);
      if (name == NULL)
	name = ELFSH_NULL_STRING;
      printf(" [%04u] %s [vaddr %08X , len %u bytes] \n", 
	     index, name, 
	     (u_int) cur->section->shdr->sh_addr + cur->offset, 
	     cur->len);
    }
  puts("");
  return (0);
}














