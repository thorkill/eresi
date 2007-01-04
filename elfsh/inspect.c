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
  elfshobj_t		*obj;
  elfshsect_t		*sect;
  elfsh_Sym		*sym;
  mjrblock_t		*blk;
  mjrcaller_t		*cal;
  char			*name;
  char			*pname;
  elfsh_SAddr		off;
  u_int			vaddr;
  char			buflog[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Preliminary checks */
  obj = world.curjob->current;

  if (!world.mjr_session.cur->analysed)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
     "Control flow section not found : use analyse command", -1);

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
      pname = elfsh_reverse_metasym(world.curjob->current, cal->vaddr, &off);
      switch (cal->type)
	{
	case CALLER_JUMP:
	  snprintf(buflog, sizeof(buflog), 
		   "  -> jumped from " XFMT " : %s + " DFMT "\n",
		   cal->vaddr, pname, off);
	  break;
	case CALLER_CALL:
	  snprintf(buflog, sizeof(buflog),
		   "  -> called from " XFMT " : %s + " DFMT "\n", 
		   cal->vaddr, pname, off);
	  break;
	case CALLER_CONT:
	  snprintf(buflog, sizeof(buflog),
		   "  -> continued from " XFMT " : %s + " DFMT "\n", 
		   cal->vaddr, pname, off);
	  break;
	default:
	  snprintf(buflog, sizeof(buflog),
		   "  -> UNKNOW from " XFMT " : %s + " DFMT "\n", 
		   cal->vaddr, pname, off);
	}
        vm_output(buflog);
    }
  
  /* Dump ASM */
  vm_output(" -- block disassembly --\n");
  sect = elfsh_get_parent_section(obj, blk->vaddr, NULL);
  off  = elfsh_get_foffset_from_vaddr(obj, blk->vaddr);
  vm_display_object(sect, sym, blk->size, 0, off, blk->vaddr, 
		    name, ELFSH_DISASM_VIEW);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  
