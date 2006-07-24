/*
** 
** hijack.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Wed Oct  1 21:01:35 2003
** Updated : Fri Nov 28 01:58:59 2003
*/

#include "modflow.h"

int			cmd_flowjack(void)
{
  struct s_iblock	**new_blk_list;
  struct s_iblock	*blk_list;
  struct s_iblock	*to_hijack;
  struct s_caller	*cal;
  unsigned long		addr;
  elfshsect_t		*sect;
  elfsh_Sym		*sym;
  char			*buffer;
  char			*name;
  int			index;
  asm_instr		ins;
  elfsh_SAddr		off;
  unsigned long		new_addr;
  u_int			value;

  elfshobj_t		*file;
  char			*param;

  sect = elfsh_get_section_by_name(world.curjob->current, ".control", 0, 0, 0);
  if (!sect)
    {
      printf("no \".control\" section. use the flow command\n");
      return (-1);
    }
  else
    puts(" * loading blocks");

  if (!sect->altdata)
  {
    printf(" * .control section unloaded ... Loading\n");
    new_blk_list = elfsh_malloc(sizeof (struct s_iblock *));
    *new_blk_list = 0;
    load_blocks(world.curjob->current, new_blk_list);
    sect->altdata = &new_blk_list;
    puts(" * loaded");
  } 
  else
    puts(" * .control section already loaded\n");
  blk_list = sect->altdata;

  file  = world.curjob->current;
  param = world.curjob->curcmd->param[0];
  sym = elfsh_get_metasym_by_name(file, param);
  if (sym)
    addr = sym->st_value;
  else
    addr = strtoul(world.curjob->curcmd->param[0], 0, 16);
  
  printf(" * patch blocks calling %08x\n", (int) addr);
  
  if ((sym = elfsh_get_metasym_by_name(world.curjob->current, world.curjob->curcmd->param[1])))
    new_addr = sym->st_value;
  else
    new_addr = strtoul(world.curjob->curcmd->param[0], 0, 16);

  if (!addr)
    {
      printf(" ! %s: cannot resolve symbol or incorrect address\n", 
	     world.curjob->curcmd->param[0]);
      free_blocks(blk_list);
      return (-1);
    }
  if (!new_addr)
    {
      printf(" ! %s: cannot resolve symbol or incorrect address\n", 
	     world.curjob->curcmd->param[1]);
      free_blocks(blk_list);
      return (-1);
    }
  
  if (!(to_hijack = block_get_by_vaddr(blk_list, addr, 0)))
    {
      printf(" ! no block found at %08x\n", (int) addr);
      free_blocks(blk_list);
      return (-1);
    }

  buffer = 0;
  for (cal = to_hijack->caller; cal; cal = cal->next)
    {
      name = elfsh_reverse_metasym(world.curjob->current, addr, &off);
      printf(" * patching block %s + " DFMT "\n", name, off);
      to_hijack = block_get_by_vaddr(blk_list, cal->vaddr, 1);
      buffer = elfsh_realloc(buffer, to_hijack->size - (cal->vaddr - to_hijack->vaddr));
      elfsh_raw_read(world.curjob->current, 
		     elfsh_get_foffset_from_vaddr(world.curjob->current, cal->vaddr),
		     buffer, to_hijack->size - (cal->vaddr - to_hijack->vaddr));
      asm_read_instr(&ins, (u_char *)buffer, 
		     to_hijack->size - (cal->vaddr - to_hijack->vaddr),
		     &world.proc);
      puts(" * would patch -> ");
      //fflush(stdout);
      name = elfsh_reverse_metasym(world.curjob->current, cal->vaddr, &off);
      index = cal->vaddr - to_hijack->vaddr;
      
      display_instr(-1, 0, cal->vaddr, 0, 
		    to_hijack->size - (cal->vaddr - to_hijack->vaddr),
		    name, off, buffer, NULL, NULL);
      
      /* patching instruction operand */
      asm_operand_get_immediate(&ins, 1, 0, &value);
      if (ins.op1.type == ASM_OTYPE_JUMP)
	{	
	  value = cal->vaddr + asm_instr_len(&ins);
	  value = new_addr - value;
	  asm_operand_set_immediate(&ins, 1, 0, &value);
	} 
      else
	{
	  puts(" ! operand type not supported");
	  continue;
	}
      /* display for debug */
      puts(" * patched ->");
      display_instr(-1, 0, cal->vaddr, 0,
		    to_hijack->size - (cal->vaddr - to_hijack->vaddr),
		    name, off, buffer, NULL, NULL);
      
      elfsh_raw_write(world.curjob->current, 
		      elfsh_get_foffset_from_vaddr(world.curjob->current, 
						   cal->vaddr),
		      buffer, to_hijack->size - (cal->vaddr - to_hijack->vaddr));
    }

  return (0);
}







