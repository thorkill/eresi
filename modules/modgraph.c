/*
** modgraph.c for elfsh
** 
** Started on  Fri Mar  7 07:18:03 2003 mayhem
** Updated on  Sat Jun  2 21:34:03 2005 mayhem
*/
#include "elfsh.h"
#include <libelfsh.h>
#include "modflow/modflow.h"


void	dump_assembly(int fd, elfshblk_t *blk);

/* Add a link entry to the graph */
void		vm_write_graphent(elfshobj_t *file, int fd, elfshblk_t *cur)
{
  char		buf[BUFSIZ];
  char		*src_name;
  int		src_offset;
  char		*dst_name;
  int		dst_offset;
  
  if (cur->altype == CALLER_RET || cur->altype == CALLER_UNKN)
    return;

  src_name = elfsh_reverse_metasym(file, cur->vaddr, &src_offset);
  if (src_name == NULL)
    src_name = ELFSH_NULL_STRING;
  
  if (cur->contig) 
    {
      dst_offset = 0;
      dst_name = elfsh_reverse_metasym(file, cur->contig, &dst_offset);
      if (dst_name == NULL)
	snprintf(buf, sizeof(buf), "%s_%i -> unresolved "
	        "[color=\"0.650 0.700 0.000\"];\n",
	       src_name, src_offset);
	else
	snprintf(buf, sizeof(buf), "%s_%i -> %s_%i "
	       "[color=\"0.650 0.700 0.000\"];\n",
	       src_name, src_offset,
	       dst_name, dst_offset);
      write(fd, buf, strlen(buf));
    }

  if (cur->altern) 
    {
      dst_offset = 0;
      dst_name = elfsh_reverse_metasym(file, cur->altern, &dst_offset);
      if (dst_name == NULL)
      snprintf(buf, sizeof(buf), "%s_%i -> unresolved "
	       "[color=\"0.000 0.700 0.650\"];\n",
	       src_name, src_offset);
	else
      snprintf(buf, sizeof(buf), "%s_%i -> %s_%i "
	       "[color=\"0.000 0.700 0.650\"];\n",
	       src_name, src_offset,
	       dst_name, dst_offset);
      write(fd, buf, strlen(buf));
    }
  
}



/* Graph the binary */
int		cmd_graph()
{
  elfshblk_t	*blk;
  elfshsect_t	*sect;
  elfsh_Sym	*symtab;
  int		index;
  int		num;
  int		fd;
  char		buf[BUFSIZ];
  char		*name;
  int		offset;
  int		unresolved_pass;
  char		*blk_col;
  void		*data;

  /* For each executable section, get the block list */
  
  if (!(sect = elfsh_get_section_by_name(world.current, ".control", 0, 0, 0)))
    puts(" [*] no \".control\" section found. Aborting\n");
  else 
    {
      XOPEN(fd, world.args.param[0], O_RDWR | O_CREAT, 0644, -1);
      sprintf(buf, "digraph prof {\n"
	      "size=\"6,4\"; ratio = fill; node [style=filled];\n");
      write(fd, buf, strlen(buf));
      
      symtab = elfsh_get_symtab(world.current, &num);
      unresolved_pass = 0;

      for (index = 0; index < num; index++)
	{
	  if ((elfsh_get_symbol_type(symtab + index) != STT_BLOCK))
	    continue;

	  data = elfsh_get_raw(sect);
	  if (!(blk = (data + (symtab + index)->st_value)))
	    break;
	  name = elfsh_reverse_metasym(world.current, blk->vaddr, &offset);
	  if ((name == NULL) && !unresolved_pass)
	    {
	      unresolved_pass = 1;
	      snprintf(buf, sizeof (buf), "unresolved [");
	    }
	  else
	    snprintf(buf, sizeof (buf), 
		     "%s_%i [fontname=\"Courier\" shape=\"box\" label=\"", name, offset);
		     
	  write(fd, buf, strlen(buf));
	  /* Write the block as a node */
	  dump_assembly(fd, blk);
	  if (blk->altype == CALLER_RET)
	    blk_col = BLK_COLOR_RED;
	  else
	    {
	      /*
	      if (block_is_called(block_get_by_vaddr((struct s_iblock *) 
						     sect->altdata, blk->vaddr, 0)))
		blk_col = BLK_COLOR_GREEN;
	      else
	      */
		blk_col = BLK_COLOR_CYAN;
	    }
	  snprintf(buf, sizeof(buf), "\" color=%s];\n", blk_col);
	  write(fd, buf, strlen(buf));
	  /* Write all graph links for the current block */
	  vm_write_graphent(world.current, fd, blk);
	}
	  
      write(fd, "}\n", 2);
      XCLOSE(fd, -1);
      printf(" [*] Graph description dumped in %s \n\n", 
	     world.args.param[0]);
    }
  return (0);
}


void	elfsh_init()
{
  puts(" [*] ELFsh modgraph loaded \n");
  puts("     Added commands:\n\tgraph <file> : dump graphviz file to file\n");
  vm_addcmd(CMD_GRAPH, cmd_graph, vm_getoption, 1);
}


void	elfsh_fini()
{
  vm_delcmd(CMD_GRAPH);
  puts(" [*] ELFsh modgraph unloaded \n");
}


void	dump_assembly(int fd, elfshblk_t *blk)
{
  char	*buffer;
  char	*name;
  int	off;
  u_int	index = 0;
  int	vm_quiet;
  
  vm_quiet = world.state.vm_quiet;
  world.state.vm_quiet = 1;
  buffer = elfsh_malloc(blk->size);
  off = elfsh_get_foffset_from_vaddr(world.current, blk->vaddr);
  if (off > 0) 
    {
    if ((elfsh_raw_read(world.current, off, buffer, blk->size)) > 0)
      {
	// write(fd, "{", 1);
	while (index < blk->size)
	  {
	    if (index != 0)
	      write(fd, "\\n", 2);
	    index += display_instr(fd, index, blk->vaddr, 0, blk->size, 
				   name, index + off, buffer);
	    
	  }
	// Chawrite(fd, "}", 1);
      }
    }
  else
    {
      name = elfsh_reverse_metasym(world.current, blk->vaddr, &off);
      dprintf(fd, "%s_%08x", name, off);

      }
  XFREE(buffer);
  world.state.vm_quiet = vm_quiet;
}


