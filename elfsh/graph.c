/*
** graph.c in elfsh
**
** Dump graphviz output
**
** Started : Fri Mar  7 07:18:03 2003 mayhem
** Updated : Fri Dec 10 02:04:19 2006 mayhem
*/
#include "elfsh.h"


/* Add a link entry to the graph */
void		vm_write_graphent(elfshobj_t *file, int fd, elfshblk_t *cur)
{
  char		buf[BUFSIZ];
  char		*src_name;
  char		*dst_name;
  char		*col_arrow;
  elfsh_SAddr	src_offset;
  elfsh_SAddr	dst_offset;
  
  /* Some preliminary checks */
  if (cur->altype == CALLER_RET || cur->altype == CALLER_UNKN)
    return;
  src_name = elfsh_reverse_metasym(file, cur->vaddr, &src_offset);
  if (src_name == NULL)
    src_name = ELFSH_NULL_STRING;
  
  /* First process the contiguous block */
  if (cur->contig)
    {
      col_arrow = (cur->altype == CALLER_JUMP ? BLK_COLOR_FALSE : BLK_COLOR_CONT);
      dst_offset = 0;
      dst_name = (cur->contig != -1 ? 
		  elfsh_reverse_metasym(file, cur->contig, &dst_offset) : NULL);
      if (dst_name == NULL)
	snprintf(buf, sizeof(buf), "%s_" DFMT " -> unresolved "
		 "[color=%s];\n",
		 src_name, src_offset, col_arrow);
      else
	snprintf(buf, sizeof(buf), "%s_" DFMT " -> %s_" DFMT
		 "[color=%s];\n",
		 src_name, src_offset,
		 dst_name, dst_offset, col_arrow);
      write(fd, buf, strlen(buf));
    }

  /* Then the alternative block */
  if (cur->altern)
    {
      col_arrow = ((cur->altype == CALLER_JUMP) ? 
		   (cur->contig ? BLK_COLOR_TRUE : BLK_COLOR_JUMP) : 
		   BLK_COLOR_CALL);
      dst_offset = 0;
      dst_name = (cur->contig != -1 ? 
		  elfsh_reverse_metasym(file, cur->altern, &dst_offset) : NULL);
      if (dst_name == NULL)
	snprintf(buf, sizeof(buf), "%s_" DFMT " -> unresolved "
		 "[color=%s];\n",
		 src_name, src_offset, col_arrow);
      else
	snprintf(buf, sizeof(buf), "%s_" DFMT " -> %s_" DFMT
		 "[color=%s];\n",
		 src_name, src_offset,
		 dst_name, dst_offset, col_arrow);
      write(fd, buf, strlen(buf));
    }

}



/* Disassemble a block */
void		vm_disasm_block(int fd, elfshblk_t *blk)
{
  char		*buffer;
  char		*name;
  elfsh_SAddr	off;
  u_int		index = 0;
  int		vm_quiet;
  int		ret;
  u_int		foffset;

  vm_quiet = world.state.vm_quiet;
  world.state.vm_quiet = 0;
  buffer  = elfsh_malloc(blk->size);
  foffset = elfsh_get_foffset_from_vaddr(world.curjob->current, blk->vaddr);
  ret     = elfsh_raw_read(world.curjob->current, foffset, buffer, blk->size);
  if (ret > 0)
    {
      name = elfsh_reverse_metasym(world.curjob->current, blk->vaddr, &off);
      while (index < blk->size)
	{
	  index += vm_display_instr(fd, index, blk->vaddr, 0, blk->size,
				    name, index + off, buffer);
	  write(fd, "\\l", 2);
	}
    }
  XFREE(buffer);
  world.state.vm_quiet = vm_quiet;
}




/* Graph the binary */
int		cmd_graph(void)
{
  elfshblk_t	*blk;
  elfshsect_t	*sect;
  elfshiblock_t	*blk_list;
  elfshiblock_t	*iblk;
  elfshcaller_t	*cal;
  elfsh_Sym	*symtab;
  int		index;
  int		num;
  int		fd;
  char		buf[BUFSIZ];
  char		*name;
  elfsh_SAddr	offset;
  int		unresolved_pass;
  char		*blk_col;
  u_int		min;
  u_int		max;
  char		*ptr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Some preliminary checks */
  sect = elfsh_get_section_by_name(world.curjob->current,
				   ELFSH_SECTION_NAME_CONTROL, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Control flow section not found (use flow command)", -1);

  fd = open(world.curjob->curcmd->param[0], O_RDWR | O_CREAT, 0644);
  if (fd == -1)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot create graphviz file", -1);
  
  /* Gxoet parameters */
  if ((symtab = elfsh_get_metasym_by_name(world.curjob->current,
					  world.curjob->curcmd->param[1])))
    min = symtab->st_value;
  else
    min = strtoul(world.curjob->curcmd->param[1], 0, 16);
  ptr = world.curjob->curcmd->param[2];
  if (*ptr == '+')
    max = min + atoi(++ptr);
  else
    max = strtoul(ptr, 0, 16);

#if defined(__DEBUG_GRAPH__)
  printf(" [D] Graph parameters : min = %8x max =%8x\n", min, max);
#endif

  /* For each executable section, get the block list */
  if (!(blk_list = sect->altdata))
    mjr_load_blocks(world.curjob->current, &blk_list);

  if (blk_list == NULL)
    printf("blk_list is NULL ! \n");

  snprintf(buf, sizeof(buf),"digraph prof {\n"
	  "ratio = fill; node [style=filled];\n");
  write(fd, buf, strlen(buf));
  symtab = elfsh_get_symtab(world.curjob->current, &num);
  unresolved_pass = 0;

  /* Parse the symbol table */
  for (index = 0; index < num; index++)
    {

      /* if symbol is not of type STT_BLOCK skip it */
      if (elfsh_get_symbol_type(symtab + index) != STT_BLOCK)
	continue;
      blk = sect->data + symtab[index].st_value;
      if (!blk)
	{
	  printf("No BLOCK found at offset %08X \n", blk);
	  continue;
	}
      iblk = mjr_block_get_by_vaddr(blk_list, blk->vaddr, 0);
      if (!iblk)
	{
	  //printf("No IBLOCK found at addr %08X \n", blk->vaddr);
	  putchar('.');
	  continue;
	}
      else
	printf("\nIBLOCK FOUND at %08X \n", blk->vaddr);

      /* Check if block is inside interval parameter
      ** Parameter type: min:max | min:+len
      */
      if ((blk->vaddr < min) || (max <= blk->vaddr))
	{
	  /* block doesn't belong to interval provided
	   * check if it is called by a block belonging
	   * to another block
	   */
	  for (cal = iblk->caller; cal; cal = cal->next)
	    if ((cal->vaddr >= min) && (max > cal->vaddr))
	      min = min;
	  continue;
	}

#if defined(__DEBUG_GRAPH__)
      printf("min = %08x blk = %08x max =%08x\n", 
	     min, blk->vaddr, max);
#endif

      /* Get symbol name from block address */
      name = elfsh_reverse_metasym(world.curjob->current, blk->vaddr, &offset);
      if (!name && !unresolved_pass)
	{
	  unresolved_pass = 1;
	  snprintf(buf, sizeof (buf), "unresolved [");
	}

      /* Select verbosity level */
      else if (world.state.vm_gvl == 1)
	snprintf(buf, sizeof (buf),
		 "%s_" DFMT " [shape=\"box\" label=\"<%s+%x>:\\l", 
		 name, offset, name, offset);
      else
	snprintf(buf, sizeof (buf),
		 "%s_" DFMT " [shape=\"box\" label=\"", 
		 name, offset);

      /* Filter out the unrequested blocks */
      if ((blk->vaddr < min) || (max <= blk->vaddr))
	continue;

      /* Write the block as a node */
      write(fd, buf, strlen(buf));
      if (name != NULL)
	{
	  vm_disasm_block(fd, blk);
	  write(fd, "\"", 1);
	}
      blk_col = ((blk->altype == CALLER_RET) ? BLK_COLOR_RET : 
		 (mjr_block_is_funcstart(iblk) ? BLK_COLOR_FUNC : BLK_COLOR_NORM));
      snprintf(buf, sizeof(buf), " color=%s];\n", blk_col);
      write(fd, buf, strlen(buf));
      vm_write_graphent(world.curjob->current, fd, blk);
    } 

  write(fd, "}\n", 2);
  close(fd);
  printf(" [*] Graph description dumped in %s \n\n",
	 world.curjob->curcmd->param[0]);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
