/*
** @file inspect.c
** @ingroup libstderesi
** @brief Inspect the content of a basic block
**
** Original author  : <sk at devhell dot org>
**
** December 10 2006 : Merged from modflow to the ELFsh vm -may
**
** $Id: inspect.c,v 1.2 2008-02-16 12:32:27 thor Exp $
*/
#include "libstderesi.h"



/** 
 * Some API that needs to be moved in another file ! 
 * @param id
 */
eresi_Addr	revm_get_block_vaddr_by_id(int id)
{
  container_t *container;
  mjrblock_t	 *block;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  container = mjr_lookup_container(world.mjr_session.cur,id);
  block     = (mjrblock_t *) container->data;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, block->vaddr);
}


/** 
 * Inspect command for basic blocks 
 */
int			cmd_inspect() 
{
  elfshobj_t		*obj;
  elfshsect_t		*sect;
  elfsh_Sym		*sym;
  mjrblock_t		*blk;
  container_t		*cntnr;
  mjrlink_t		*cal;
  char			*name;
  char			*pname;
  elfsh_SAddr		off;
  unsigned int			vaddr;
  char			buflog[BUFSIZ];
  listent_t		*curent;
  mjrblock_t		*tmpblock;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Preliminary checks */
  cntnr = NULL;
  obj = world.curjob->curfile;
  if (!world.mjr_session.cur->analysed)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Control flow section not found"
		 " : use analyse command", -1);
  
  /* Try to find block by symbol or address */
  if ((sym = elfsh_get_metasym_by_name(world.curjob->curfile, 
				       world.curjob->curcmd->param[0])))
    vaddr = sym->st_value;
  else
    vaddr = strtoul(world.curjob->curcmd->param[0], 0, 16);  

  /* Try to lookup by variable name */
  if (!vaddr)
    {
      if (!strncmp(world.curjob->curcmd->param[0], "$bloc", 5))
	cntnr = hash_get(&exprs_hash, world.curjob->curcmd->param[0]);
      if (!cntnr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find block symbol or address", -1);
      name = elfsh_reverse_metasym(world.curjob->curfile, *(eresi_Addr *) cntnr->data, &off);
    }
  else
    {
      name = elfsh_reverse_metasym(world.curjob->curfile, vaddr, &off);
      cntnr = mjr_block_get_by_vaddr(world.mjr_session.cur, vaddr, 1);
      if (!cntnr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find block", -1); 
    }

  blk = cntnr->data;

  /* Dump calling information */
  snprintf(buflog, sizeof(buflog), " [*] Found block " XFMT " : %s + " DFMT "\n",
	   blk->vaddr, name, off);
  revm_output(buflog);

  /* Iterate on the list of links */
  for (curent = cntnr->inlinks->head; curent; curent = curent->next)
    {
      cal      = curent->data;
      tmpblock = mjr_lookup_container(world.mjr_session.cur,cal->id)->data;
      pname    = elfsh_reverse_metasym(world.curjob->curfile, tmpblock->vaddr, &off);
      
      /* FIXME: we have to work out the lack of the 'type' field for blocks */
      switch (cal->type)
	{
	case MJR_LINK_BLOCK_COND_TRUE:
	  snprintf(buflog, sizeof(buflog), 
		   "  -> true jumped from " XFMT " : %s + " DFMT "\n",
		   revm_get_block_vaddr_by_id(cal->id), pname, off);
	  break;
	case MJR_LINK_BLOCK_COND_FALSE:
	  snprintf(buflog, sizeof(buflog), 
		   "  -> false from " XFMT " : %s + " DFMT "\n",
		   revm_get_block_vaddr_by_id(cal->id), pname, off);
	  break;
	case MJR_LINK_FUNC_CALL:
	  snprintf(buflog, sizeof(buflog),
		   "  -> called from " XFMT " : %s + " DFMT "\n", 
		   revm_get_block_vaddr_by_id(cal->id), pname, off);
	  break;
	case MJR_LINK_FUNC_RET:
	  snprintf(buflog, sizeof(buflog),
		   "  -> returned from " XFMT " : %s + " DFMT "\n", 
		   revm_get_block_vaddr_by_id(cal->id), pname, off);
	  break;
	default:
	  snprintf(buflog, sizeof(buflog),
		   "  -> UNKNOW from " XFMT " : %s + " DFMT "\n", 
		   revm_get_block_vaddr_by_id(cal->id), pname, off);
	}
      revm_output(buflog);      
    }

  /* Dump ASM */
  revm_output(" -- block disassembly --\n");
  sect = elfsh_get_parent_section(obj, blk->vaddr, NULL);
  off  = elfsh_get_foffset_from_vaddr(obj, blk->vaddr);
  revm_object_display(sect, sym, blk->size, 0, off, blk->vaddr, 
		    name, REVM_VIEW_DISASM);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
