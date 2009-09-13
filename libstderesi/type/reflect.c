/*
* @file libstderesi/type/reflect.c
** @ingroup type
** @brief Handle the match construct.
**
** Start on Wed May 23 13:55:45 2007 jfv
**
** $Id$
*/
#include "libstderesi.h"


/**
 * Reflect command disassemble the block cache of the parameter and create a list of instr exprs 
 */
int		cmd_reflect()
{
  container_t	*container;
  container_t	*instrcontainer;
  mjrblock_t	*curblock;
  asm_instr	cur;
  elfsh_Half	machine;
  char		logbuf[BUFSIZ];
  char		logbuf2[BUFSIZ];
  eresi_Addr	addr;
  u_int		off;
  int		ret;
  aspectype_t	*curtype;
  void		*blocdata;
  int		fileoff;
  list_t	*instrlist;
  revmexpr_t	*expr;
  int		insnbr;
  int		maxdepth;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  curtype  = aspect_type_get_by_name("instr");
  if (!curtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed reflection : unknown type instruction", -1);

  /* Init proc */			  
  if (!world.curjob->proc) 
    {
      switch (machine = elfsh_get_arch(world.curjob->curfile->hdr))
	{
	case EM_386:
	  world.curjob->proc = &world.proc_ia32;
	  break;
	case EM_SPARC:
	case EM_SPARC32PLUS:
	case EM_SPARCV9:
	  world.curjob->proc = &world.proc_sparc;
	  break;
	default:
	  snprintf(logbuf, sizeof (logbuf), 
		   "Architecture %s not supported. No disassembly available\n",
		   elfsh_get_machine_string(machine));
	  revm_output(logbuf);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
    }
  
  /* Now lookup the block by its addr or symbol */
  addr = revm_lookup_addr(world.curjob->curcmd->param[0]);
  if (!addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to lookup parameter address", -1);

  /* Analyse the binary if not already done */
  /*
  if (!world.mjr_session.cur->analysed)
    {
      maxdepth = (int) config_get_data(CONFIG_CFGDEPTH);
      ret = mjr_analyse(&world.mjr_session, 
			world.curjob->curfile->hdr->e_entry, 
			maxdepth, 0);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed analyzing current object", -1);
    }
  */

  container = mjr_block_get_by_vaddr(world.mjr_session.cur, addr, MJR_BLOCK_GET_STRICT);
  if (!container)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to find bloc at this virtual address", -1);
  curblock = (mjrblock_t *) container->data;

  /* Load the data from the bloc */
  blocdata = alloca(curblock->size);
  fileoff = elfsh_get_foffset_from_vaddr(world.curjob->curfile, curblock->vaddr);
  if (elfsh_readmemf(world.curjob->curfile, fileoff, 
		     blocdata, curblock->size) != (int) curblock->size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to read data from bloc", -1);

  /* Create the new list of instructions in expression form */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, instrlist, sizeof(list_t), -1);
  snprintf(logbuf2, sizeof(logbuf2), AFMT, curblock->vaddr);

  elist_init(instrlist, strdup(logbuf2), ASPECT_TYPE_EXPR);

  /* Reflection all instructions of the basic bloc in the list */
  for (insnbr = off = 0; off < curblock->size; off += ret, insnbr++)
    {
      
      /* Fetch the current instruction */
      ret = asm_read_instr(&cur, (u_char *) blocdata + off, 
			   curblock->size - off + 10, world.curjob->proc);
      if (ret < 0)
	{
	  elist_destroy(instrlist);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to fetch code for basic bloc", -1);
	}

      /* Also add the instruction to the current reflected list for this block */
      instrcontainer = container_create(curtype->type, (void *) &cur, NULL, NULL, 0);
      snprintf(logbuf, sizeof (logbuf), "$instr-"XFMT, curblock->vaddr + off); 
      addr = (eresi_Addr) instrcontainer;
      expr = revm_inform_type_addr(curtype->name, strdup(logbuf), addr, NULL, 0, 1);
      elist_add(instrlist, strdup(logbuf), expr);
    }

  /* Reverse instrlist and add it to the hash of lists */
  instrlist = elist_reverse(instrlist);
  hash_add(&instrlists_hash, strdup(logbuf2), instrlist);
  
  /* Printing message if we are not in quiet mode */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, sizeof(logbuf), 
	       " [*] Basic bloc at address " AFMT " reflected succesfully (%u instrs) \n\n", 
	       curblock->vaddr, insnbr);
      revm_output(logbuf);
    }

  /* Put the current bloc in the last result variable */
  /*
  revm_expr_destroy_by_name(REVM_VAR_RESULT);
  revm_expr_copy(expr, REVM_VAR_RESULT);
  */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
