/*
** inspect.c in elfsh
** 
** Author  : <sk at devhell dot org>
**
** December 10 2006 : Merged from modflow to the ELFsh vm -may
*/
#include "elfsh.h"


/* Inspect command for basic blocks */
int			cmd_inspect() 
{
  elfshsect_t		*sect;
  elfsh_Sym		*sym;
  elfshiblock_t		**new_blk_list;
  elfshiblock_t		*blk_list;
  elfshiblock_t		*blk;
  elfshcaller_t		*cal;
  char			*name;
  elfsh_SAddr		off;
  int			index;
  int			num;
  u_int			vaddr;
  char			*buffer;
  asm_instr		ins;
  char			*str;
  char			buflog[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  sect = elfsh_get_section_by_name(world.curjob->current, 
				   ELFSH_SECTION_NAME_CONTROL, 
				   0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section found", -1);

  if (!sect->altdata)
    {
      snprintf(buflog, sizeof(buflog), 
	       " [*] %s section found but unloaded\n",
	       ELFSH_SECTION_NAME_CONTROL);
      new_blk_list  = elfsh_malloc(sizeof(elfshiblock_t *));
      *new_blk_list = 0;
      num           = mjr_load_blocks(world.curjob->current, new_blk_list);
      sect->altdata = *new_blk_list;
      printf(" [*] Loaded %i blocks\n", num);
    } 
  else
    blk_list = sect->altdata;

  blk_list = (elfshiblock_t *) sect->altdata;
  
  if ((sym = elfsh_get_metasym_by_name(world.curjob->current, 
				       world.curjob->curcmd->param[0])))
    vaddr = sym->st_value;
  else
    vaddr = strtoul(world.curjob->curcmd->param[0], 0, 16);
  
  if (!vaddr)
    ELFSH_SETERROR(" * cannot find symbol/address null\n", -1);
  
  name = elfsh_reverse_metasym(world.curjob->current, vaddr, &off);
  blk = mjr_block_get_by_vaddr(blk_list, vaddr, 1);
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
  
  /* Dump ASM */
  puts(" -- block disassembly --");
  buffer = elfsh_malloc(blk->size);
  elfsh_raw_read(world.curjob->current, 
		 elfsh_get_foffset_from_vaddr(world.curjob->current, 
					      blk->vaddr), 
		 buffer, blk->size);
  for (index = 0; index < blk->size; index += num) 
    {
      
      
      num = asm_read_instr(&ins, (u_char *)buffer + index, 
			   blk->size - index, &world.proc);
      str = asm_display_instr_att(&ins, blk->vaddr + index);
      printf("%8x:\t%s\n", blk->vaddr + index, str);
      
    }
  XFREE(buffer);
  puts(" * done\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  
