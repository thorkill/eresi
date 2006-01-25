/*
** 
** inspect.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Fri Jun  6 18:52:31 2003
** Updated : Fri Nov 28 01:33:51 2003
*/
#include "modflow.h"



int	inspect_cmd() {
  elfshsect_t	*sect;
  elfsh_Sym	*sym;
  struct s_iblock	**new_blk_list;
  struct s_iblock	*blk_list;
  struct s_iblock	*blk;
  struct s_caller	*cal;
  char			*name;
  elfsh_SAddr		off;
  int		index;
  int		num;
  u_int		vaddr;
  char		*buffer;
  asm_instr	ins;
  char		*str;


  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sect = elfsh_get_section_by_name(world.curjob->current, ".control", 0, 0, 0);
  if (!sect)
    {
      puts(" [*] no \".control\" section found. Aborting\n");
      return (-1);
    }
  else
    if (!sect->altdata)
      {
	puts(" [*] \".control\" section found but unloaded\n");
	new_blk_list = elfsh_malloc(sizeof (struct s_iblock *));
	*new_blk_list = 0;
	num = load_blocks(world.curjob->current, new_blk_list);
	sect->altdata = *new_blk_list;
	printf(" * loaded %i blocks\n", num);
      } 
    else
      blk_list = sect->altdata;
  blk_list = (struct s_iblock *) sect->altdata;
  
  if ((sym = elfsh_get_metasym_by_name(world.curjob->current, 
				       world.curjob->curcmd->param[0])))
    vaddr = sym->st_value;
  else
    vaddr = strtoul(world.curjob->curcmd->param[0], 0, 16);
  
  if (!vaddr)
    ELFSH_SETERROR(" * cannot find symbol/address null\n", -1);
  
  name = elfsh_reverse_metasym(world.curjob->current, vaddr, &off);
  blk = block_get_by_vaddr(blk_list, vaddr, 1);
  if (!blk)
    {
      printf(" * cannot find block at %08x\n", vaddr);
      return (-1);
    }
  else
    printf(" * found block %08x : %s + " DFMT "\n", blk->vaddr, name, off);
  for (cal = blk->caller; cal ; cal = cal->next)
    {
      name = elfsh_reverse_metasym(world.curjob->current, cal->vaddr, &off);
      switch(cal->type)
	{
	case CALLER_JUMP:
	  printf("  -> jump from %08x : %s + " DFMT "\n", cal->vaddr, name, off);
	  break;
	case CALLER_CALL:
	  printf("  -> call from %08x : %s + " DFMT "\n", cal->vaddr, name, off);
	  break;
	case CALLER_CONT:
	  printf("  -> continued from %08x : %s + " DFMT "\n", cal->vaddr, name, off);
	  break;
	default:
	  printf("  -> ??? from %08x : %s + " DFMT "\n", cal->vaddr, name, off);
	}
    }
  /*	dump assembly */
  puts(" -- block disassembly --");
  buffer = elfsh_malloc(blk->size);
  elfsh_raw_read(world.curjob->current, 
		 elfsh_get_foffset_from_vaddr(world.curjob->current, 
					      blk->vaddr), 
		 buffer, blk->size);
  for (index = 0; index < blk->size; index += num) 
    {
      
      
      num = asm_read_instr(&ins, buffer + index, 
			   blk->size - index, &world.proc);
      str = asm_display_instr_att(&ins, blk->vaddr + index);
      printf("%8x:\t%s\n", blk->vaddr + index, str);
      
    }
  XFREE(buffer);
  puts(" * done\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  
