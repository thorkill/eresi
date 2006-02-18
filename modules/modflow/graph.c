/*
** 
** graph.c in modflow
** 
** Author  : <sk@devhell.org>
** Started : Fri Mar  7 07:18:03 2003 mayhem
** Updated : Fri Nov 28 02:04:19 2003
*/

#include "elfsh.h"
#include <libelfsh.h>
#include "modflow.h"


extern struct s_elfshstng mf_settings;

void	dump_assembly(int fd, elfshblk_t *blk);

/* Add a link entry to the graph */
void		vm_write_graphent(elfshobj_t *file, int fd, elfshblk_t *cur)
{
  char		buf[BUFSIZ];
  char		*src_name;
  elfsh_SAddr	src_offset;
  char		*dst_name;
  elfsh_SAddr	dst_offset;
  char		*col_arrow;

  if (cur->altype == CALLER_RET || cur->altype == CALLER_UNKN)
    return;
  
  src_name = elfsh_reverse_metasym(file, cur->vaddr, &src_offset);
  if (src_name == NULL)
    src_name = ELFSH_NULL_STRING;
  
  if (cur->contig) 
    {
      if (cur->altype == CALLER_JUMP)
	col_arrow = BLK_COLOR_FALSE;
      else
	col_arrow = BLK_COLOR_CONT;
      
      dst_offset = 0;

      if (cur->contig != -1)
	dst_name = elfsh_reverse_metasym(file, cur->contig, &dst_offset);
      else
	dst_name = NULL;
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
  
  if (cur->altern) 
    {
      if (cur->altype == CALLER_JUMP)
	{
	  if (cur->contig)
	    col_arrow = BLK_COLOR_TRUE;
	  else
	    col_arrow = BLK_COLOR_JUMP;
	}
      else
	col_arrow = BLK_COLOR_CALL;
      
      dst_offset = 0;

      if (cur->contig != -1)
	dst_name = elfsh_reverse_metasym(file, cur->altern, &dst_offset);
      else
	dst_name = NULL;
      
      dst_name = elfsh_reverse_metasym(file, cur->altern, &dst_offset);
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



/* Graph the binary */
int		cmd_graph(void)
{
  elfshblk_t	*blk;
  elfshsect_t	*sect;
  struct s_iblock	*blk_list;
  struct s_iblock	*iblk;
  struct s_caller	*cal;
  elfsh_Sym	*symtab;
  int		index;
  int		num;
  int		fd;
  char		buf[BUFSIZ];
  char		*name;
  elfsh_SAddr	offset;
  int		unresolved_pass;
  char		*blk_col;

  u_int min;
  char	*ptr;
  u_int max;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!(sect = elfsh_get_section_by_name(world.curjob->current, ".control", 0, 0, 0)))
    ELFSH_SETERROR(" [*] no \".control\" section found. Aborting\n", -1);
  
  if (!world.curjob->curcmd) {
   printf("[*] no \"world.curjob->curcmd\"\n");
   return (-1);
  }
  
  fd = open(world.curjob->curcmd->param[0], O_RDWR | O_CREAT, 0644);
  if (fd == -1)
    {
      printf("cannot open %s\n", world.curjob->curcmd->param[0]);
      return (-1);
    }

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
  
  printf("min = %8x max =%8x\n", min, max);
  
  
  /* For each executable section, get the block list */
  
  if (!(blk_list = sect->altdata))
    load_blocks(world.curjob->current, &blk_list);
      
  snprintf(buf, sizeof(buf),"digraph prof {\n"
	  "ratio = fill; node [style=filled];\n");
  write(fd, buf, strlen(buf));
      
  symtab = elfsh_get_symtab(world.curjob->current, &num);
  unresolved_pass = 0;
  for (index = 0; index < num; index++)
    {
      /* if symbol is not of type STT_BLOCK skip it */
      if ((elfsh_get_symbol_type(symtab + index) != STT_BLOCK))
	continue;
	  
      if (!(blk = (sect->data + (symtab + index)->st_value)))
	break;
      if (!(iblk = block_get_by_vaddr(blk_list ,  blk->vaddr, 0)))
	break;
      /*
	check if block is inside interval parameter
	parameter type:
	min:max
	min:+len
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
      // pouet:
      printf("min = %08x blk = %08x max =%08x\n", min, blk->vaddr, max);
	  
      name = elfsh_reverse_metasym(world.curjob->current, blk->vaddr, &offset);
      if ((name == NULL) && !unresolved_pass)
	{
	  unresolved_pass = 1;
	  snprintf(buf, sizeof (buf), "unresolved [");
	}
      else {
	  if (mf_settings.graph_verbose_level == 0) {
  	   snprintf(buf, sizeof (buf), 
		 "%s_" DFMT " [shape=\"box\" label=\"", name, offset);
	  } else if (mf_settings.graph_verbose_level == 1) {
	   snprintf(buf, sizeof (buf), 
		 "%s_" DFMT " [shape=\"box\" label=\"<%s+%x>:\\l", name, offset, name, offset);
	  } else {
	   snprintf(buf, sizeof (buf), 
		 "%s_" DFMT " [shape=\"box\" label=\"", name, offset);	   
	  }
	}
      write(fd, buf, strlen(buf));
      /* Write the block as a node */
      if (name != NULL)
	{
	  dump_assembly(fd, blk);
	  write(fd, "\"", 1);
	}
      if (blk->altype == CALLER_RET)
	blk_col = BLK_COLOR_RET;
      else
	{
	  if (block_is_called(iblk))
	    blk_col = BLK_COLOR_FUNC;
	  else
	    blk_col = BLK_COLOR_NORM;
	}
      snprintf(buf, sizeof(buf), " color=%s];\n", blk_col);
      write(fd, buf, strlen(buf));
      /* Write all graph links for the current block */
      if ((blk->vaddr < min) || (max <= blk->vaddr))
	   continue;

      vm_write_graphent(world.curjob->current, fd, blk);
    } /* !for */
      
  write(fd, "}\n", 2);
  close(fd);
  printf(" [*] Graph description dumped in %s \n\n", 
	 world.curjob->curcmd->param[0]);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



void		dump_assembly(int fd, elfshblk_t *blk)
{
  char		*buffer;
  char		*name;
  elfsh_SAddr	off;
  u_int		index = 0;
  int		vm_quiet;

  vm_quiet = world.state.vm_quiet;
  world.state.vm_quiet = 0;
  buffer = elfsh_malloc(blk->size);
  if ((elfsh_raw_read(world.curjob->current, 
		      elfsh_get_foffset_from_vaddr(world.curjob->current, 
						   blk->vaddr), 
		      buffer, blk->size)) > 0)
    {
      name = elfsh_reverse_metasym(world.curjob->current, blk->vaddr, &off);
      // write(fd, "{", 1);
      while (index < blk->size)
	{
	  index += display_instr(fd, index, blk->vaddr, 0, blk->size, 
				 name, index + off, buffer, NULL, NULL);
	  //lseek(fd, -1, SEEK_CUR);
	  write(fd, "\\l", 2);
	}
      // Chawrite(fd, "}", 1);
    }
  XFREE(buffer);
  world.state.vm_quiet = vm_quiet;
}

