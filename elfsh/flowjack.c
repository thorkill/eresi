/*
** flowjack.c in elfsh
** 
** Author  : <sk at devhell dot org>
**
** December 10 2006 : merged from modflow to the ELFsh vm -may
** December 31 2006 : factored and cleaned code -may
*/
#include "elfsh.h"


/* Perform hijack of basic blocks */
int			cmd_flowjack(void)
{
  mjrblock_t		*to_hijack;
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
  unsigned int		size;
  unsigned int		foff;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  sect = elfsh_get_section_by_name(world.curjob->current, 
				   ELFSH_SECTION_NAME_CONTROL, 
				   0, 0, 0);
  if (!sect || !sect->altdata)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section found. Use flow command", -1);

  /* Lookup parameters */
  file  = world.curjob->current;
  param = world.curjob->curcmd->param[0];
  sym = elfsh_get_metasym_by_name(file, param);
  if (sym)
    addr = sym->st_value;
  else
    addr = strtoul(world.curjob->curcmd->param[0], 0, 16);
  if ((sym = elfsh_get_metasym_by_name(world.curjob->current, 
				       world.curjob->curcmd->param[1])))
    new_addr = sym->st_value;
  else
    new_addr = strtoul(world.curjob->curcmd->param[0], 0, 16);

  /* Errors cases */
  if (!addr)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to resolve first parameter", -1);
  if (!new_addr)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to resolve second parameter", -1);
  
  /* Retreive bloc to be hijacked */
  printf(" * patch blocks calling %08x\n", (int) addr);
  to_hijack = mjr_block_get_by_vaddr(world.mjr_session.cur, addr, 0);
  if (!to_hijack)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to match hijacked block", -1);

  /* For each caller, modify the location beeing called */
  for (buffer = 0, cal = to_hijack->caller; cal; cal = cal->next)
    {
      name = elfsh_reverse_metasym(world.curjob->current, addr, &off);
      printf(" * patching block %s + " DFMT "\n", name, off);
      to_hijack = mjr_block_get_by_vaddr(world.mjr_session.cur, cal->vaddr, 1);
      size = to_hijack->size - (cal->vaddr - to_hijack->vaddr);
      foff = elfsh_get_foffset_from_vaddr(world.curjob->current, cal->vaddr);
      buffer = elfsh_realloc(buffer, size);
      elfsh_raw_read(world.curjob->current, foff, buffer, size);
      asm_read_instr(&ins, (u_char *) buffer, size, &world.proc);
      puts(" * would patch -> ");
      name = elfsh_reverse_metasym(world.curjob->current, cal->vaddr, &off);
      index = cal->vaddr - to_hijack->vaddr;      
      vm_display_instr(-1, 0, cal->vaddr, 0, size, name, off, buffer);
      
      /* Patching instruction operand */
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
      vm_display_instr(-1, 0, cal->vaddr, 0, size, name, off, buffer);
      elfsh_raw_write(world.curjob->current, foff, buffer, size);
    }

  return (0);
}







