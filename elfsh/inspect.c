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
  
  /* Preliminary checks */
  sect = elfsh_get_section_by_name(world.curjob->current, 
				   ELFSH_SECTION_NAME_CONTROL, 
				   0, 0, 0);
  if (!sect || !sect->altdata)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No usable control section : run analyse first", -1);
  
  /* Try to find block by symbol or address */
  if ((sym = elfsh_get_metasym_by_name(world.curjob->current, 
				       world.curjob->curcmd->param[0])))
    vaddr = sym->st_value;
  else
    vaddr = strtoul(world.curjob->curcmd->param[0], 0, 16);  
  if (!vaddr)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find block symbol or address", -1);
  name = elfsh_reverse_metasym(world.curjob->current, vaddr, &off);
  blk = mjr_block_get_by_vaddr(world.mjr_session.cur, vaddr, 1);
  if (!blk)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find block", -1);
  
  /* Dump calling information */
  snprintf(buflog, sizeof(buflog),
	   " [*] Found block " XFMT " : %s + " DFMT "\n", 
	   blk->vaddr, name, off);
  vm_output(buflog);
  for (cal = blk->caller; cal ; cal = cal->next)
    {
      name = elfsh_reverse_metasym(world.curjob->current, cal->vaddr, &off);
      switch (cal->type)
	{
	case CALLER_JUMP:
	  snprintf(buflog, sizeof(buflog), 
		   "  -> jumped from " XFMT " : %s + " DFMT "\n",
		   cal->vaddr, name, off);
	  break;
	case CALLER_CALL:
	  snprintf(buflog, sizeof(buflog),
		   "  -> called from " XFMT " : %s + " DFMT "\n", 
		   cal->vaddr, name, off);
	  break;
	case CALLER_CONT:
	  snprintf(buflog, sizeof(buflog),
		   "  -> continued from " XFMT " : %s + " DFMT "\n", 
		   cal->vaddr, name, off);
	  break;
	default:
	  snprintf(buflog, sizeof(buflog),
		   "  -> UNKNOW from " XFMT " : %s + " DFMT "\n", 
		   cal->vaddr, name, off);
	}
        vm_output(buflog);
    }
  
  /* Dump ASM */
  /* XXX: use colored assembly functions from vm/disasm.c */
  vm_output(" -- block disassembly --\n");
  buffer = elfsh_malloc(blk->size);
  elfsh_raw_read(world.curjob->current, 
		 elfsh_get_foffset_from_vaddr(world.curjob->current, blk->vaddr), 
		 buffer, blk->size);
  for (index = 0; index < blk->size; index += num) 
    {
      num = asm_read_instr(&ins, (u_char *) buffer + index, 
			   blk->size - index, &world.proc);
      str = asm_display_instr_att(&ins, blk->vaddr + index);
      printf("%8x:\t%s\n", blk->vaddr + index, str);
      
    }
  XFREE(buffer);
  puts(" * done\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  
