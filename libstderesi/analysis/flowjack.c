/*
** @file flowjack.c
** @ingroup libstderesi
** Author  : <sk at devhell dot org>
**
** December 10 2006 : merged from modflow to the ELFsh vm -may
** December 31 2006 : factored and cleaned code -may
**
** $Id$
**
*/
#include "libstderesi.h"


/* Perform hijack of basic blocks */
int			cmd_flowjack(void)
{
  container_t		*cntnr_to_hijack;
  mjrblock_t		*to_hijack, *cal;
  mjrlink_t		*caller;
  unsigned long		addr;
  elfshsect_t		*sect;
  elfsh_Sym		*sym;
  char			*buffer;
  char			*name;
  asm_instr		ins;
  elfsh_SAddr		off;
  unsigned long		new_addr;
  u_int			value;
  elfshobj_t		*file;
  char			*param;
  unsigned int		foff;
  list_t		*linklist;
  listent_t		*listent;
  int			len;
  char			logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  sect = elfsh_get_section_by_name(world.curjob->curfile, 
				   ELFSH_SECTION_NAME_EDFMT_BLOCKS, 
				   0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Control flow section not found"
		 " : use analyse command", -1);
  revm_proc_init();

  /* Lookup parameters */
  file  = world.curjob->curfile;
  param = world.curjob->curcmd->param[0];
  sym = elfsh_get_metasym_by_name(file, param);
  if (sym)
    addr = sym->st_value;
  else
    addr = strtoul(world.curjob->curcmd->param[0], 0, 16);

  if ((sym = elfsh_get_metasym_by_name(world.curjob->curfile, 
				       world.curjob->curcmd->param[1])))
    new_addr = sym->st_value;
  else
    new_addr = strtoul(world.curjob->curcmd->param[0], 0, 16);

  /* Errors cases */
  if (!addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to resolve first parameter", -1);
  if (!new_addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to resolve second parameter", -1);
  
  /* Retreive bloc to be hijacked */
  printf(" [*] Patching blocks calling " XFMT " in %s \n", (int) addr, world.curjob->curfile->name);
  cntnr_to_hijack = mjr_block_get_by_vaddr(world.mjr_session.cur, addr, 0);
  if (!cntnr_to_hijack)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to match hijacked block", -1);

  /* For each caller, modify the location beeing called */
  linklist = cntnr_to_hijack->inlinks;
  for (buffer = 0, listent = linklist->head; listent; listent = listent->next)
    {
      caller = (mjrlink_t *) listent->data;
      name = elfsh_reverse_metasym(world.curjob->curfile, addr, &off);
      printf(" [*] patching block %s + " DFMT "\n", name, off);

      cal = mjr_lookup_container(world.mjr_session.cur, caller->id)->data;

      cntnr_to_hijack = mjr_block_get_by_vaddr(world.mjr_session.cur, cal->vaddr, 1);
      to_hijack = cntnr_to_hijack->data;

      //size = to_hijack->size - (cal->vaddr - to_hijack->vaddr);

      foff = elfsh_get_foffset_from_vaddr(world.curjob->curfile, cal->vaddr);
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, buffer, buffer, cal->size, -1);
      elfsh_readmemf(world.curjob->curfile, foff, buffer, cal->size);
      
      puts(" [*] would patch block at -> ");
      name = elfsh_reverse_metasym(world.curjob->curfile, cal->vaddr, &off);
      revm_instr_display(-1, cal->vaddr, 0, cal->size, name, off, buffer);
      
      /* Understand what is the calling instruction */
      switch (caller->type)
	{
	case MJR_LINK_FUNC_CALL:
	  len = (world.curjob->proc->type == ASM_PROC_IA32 ? 5 : 4);
	  asm_read_instr(&ins, (u_char *) buffer + cal->size - len, len, world.curjob->proc);
	  break;
	case MJR_LINK_BLOCK_COND_TRUE:
	case MJR_LINK_BLOCK_COND_FALSE:
	case MJR_LINK_BLOCK_COND_ALWAYS:
	  len = asm_read_instr(&ins, (u_char *) buffer + cal->size - 2, 2, world.curjob->proc);
	  if (len == 2 && (ins.type == ASM_TYPE_IMPBRANCH || ins.type == ASM_TYPE_CONDBRANCH))
	    break;
	  len = asm_read_instr(&ins, (u_char *) buffer + cal->size - 3, 3, world.curjob->proc);
	  if (len == 3 && (ins.type == ASM_TYPE_IMPBRANCH || ins.type == ASM_TYPE_CONDBRANCH))
	    break;
	  len = asm_read_instr(&ins, (u_char *) buffer + cal->size - 4, 4, world.curjob->proc);
	  if (len == 4 && (ins.type == ASM_TYPE_IMPBRANCH || ins.type == ASM_TYPE_CONDBRANCH))
	    break;
	  len = asm_read_instr(&ins, (u_char *) buffer + cal->size - 5, 5, world.curjob->proc);
	  if (len == 5 && (ins.type == ASM_TYPE_IMPBRANCH || ins.type == ASM_TYPE_CONDBRANCH))
	    break;
	default:
	  revm_output(" [D] Unable to patch flow for non-immediate CALL/JMP transfers \n");
	  continue;
	}

      /* Patch the immediate operand */
      asm_operand_get_immediate(&ins, 1, 0, &value);
      if (ins.op[0].content == ASM_CONTENT_JUMP)
	{	
	  value = cal->vaddr + cal->size;
	  value = new_addr - value;
	  asm_operand_set_immediate(&ins, 1, 0, &value);
	} 
      else
	{
	  fprintf(stderr, " ! operand type %u not supported \n", ins.op[0].type);
	  continue;
	}

      /* display for debug */
      puts(" * patched ->");
      revm_instr_display(-1, cal->vaddr, 0, cal->size, name, off, buffer);
      elfsh_writememf(world.curjob->curfile, foff, buffer, cal->size);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}

