/*
** 
** inspect.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Fri Jun  6 18:52:31 2003
** Updated : Tue Jun 17 23:33:06 2003
*/
#include "modflow.h"



int	inspect_cmd() {
  elfshsect_t	*sect;
  Elf32_Sym	*symtab;
  elfshblk_t	*blk;
  int		index;
  int		num;
  u_int		vaddr;
  char		*buffer;
  asm_instr	ins;
  char		*str;

  if (!(sect = elfsh_get_section_by_name(world.current, ".control", 0, 0, 0)))
    puts(" [*] no \".control\" section found. Aborting\n");
  else {
    vaddr = strtoul(world.args.param[0], 0, 16);
    symtab = elfsh_get_symtab(world.current, &num);

    /* browse symtab and search block */
    for (index = 0; index < num; index++) 
      {
	if ((elfsh_get_symbol_type(symtab + index) != STT_BLOCK))
	  continue;
	blk = (sect->data + (symtab + index)->st_value);
    
	if ((blk->vaddr <= vaddr) && (vaddr < blk->vaddr + blk->size)) 
	  {
	    buffer = malloc(blk->size);
	    elfsh_raw_read(world.current, 
			   elfsh_get_foffset_from_vaddr(world.current, 
							blk->vaddr), 
			   buffer, blk->size);
	    for (index = 0; index < blk->size; index += num) 
	      {
		num = asm_read_instr(&ins, buffer + index, 
				     blk->size - index, &world.proc);
		str = asm_display_instr_att(&ins, blk->vaddr + index);
		printf("%8x:\t%s\n", blk->vaddr + index, str);
	      }
	    free(buffer);  
	    return (0);
	  }
	
      }
    printf(" [*] %8x doesn't belong to any stored block\n\n", vaddr);
  }
  return (-1);
}
  
