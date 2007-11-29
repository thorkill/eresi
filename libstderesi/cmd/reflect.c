/*
** match.c for librevm in ERESI
**
** Handle the match construct
**
** Start on Wed May 23 13:55:45 2007 jfv
**
** $Id: reflect.c,v 1.1 2007-11-29 14:01:56 may Exp $
*/
#include "libstderesi.h"


/* Reflect command disassemble the block cache of the parameter and create a list of instr exprs */
int		cmd_reflect()
{
  container_t	*container;
  container_t	*instrcontainer;
  mjrblock_t	*curblock;
  asm_instr	*cur;
  elfsh_Half	machine;
  char		logbuf[BUFSIZ];
  elfsh_Addr	addr;
  int		off;
  int		ret;
  //aspectype_t	*contype;
  aspectype_t	*curtype;
  //aspectype_t	*exprtype;
  void		*blocdata;
  int		fileoff;
  revmobj_t	*last;
  list_t	*instrlist;
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  curtype  = aspect_type_get_by_name("instr");

  //contype  = aspect_type_get_by_name("container");
  //exprtype = aspect_type_get_by_id(ASPECT_TYPE_EXPR);

  if (!curtype) // || !contype || !exprtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed reflection : unknown type instruction", -1);

  /* Analyse the binary if not already done */
  if (!world.mjr_session.cur->analysed)
    {
      ret = mjr_analyse(&world.mjr_session, 0);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed analyzing current object", -1);
    }

  /* Init proc */			  
  if (!world.curjob->proc) 
    {
      switch (machine = elfsh_get_arch(world.curjob->current->hdr))
	{
	case EM_386:
	  world.curjob->proc = &world.proc;
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
  container = mjr_block_get_by_vaddr(world.mjr_session.cur, addr, MJR_BLOCK_GET_STRICT);
  if (!container)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to find bloc at this virtual address", -1);
  curblock = (mjrblock_t *) container->data;

  /* Load the data from the bloc */
  blocdata = alloca(curblock->size);
  fileoff = elfsh_get_foffset_from_vaddr(world.curjob->current, curblock->vaddr);
  if (elfsh_raw_read(world.curjob->current, fileoff, 
		     blocdata, curblock->size) != curblock->size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to read data from bloc", -1);

  /* Create the new list of instructions in expression form */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, instrlist, sizeof(list_t), -1);
  snprintf(logbuf, sizeof(logbuf), "%08X", curblock->vaddr);
  list_init(instrlist, strdup(logbuf), curtype->type);

  /* Reflection all instructions of the basic bloc in the list */
  for (off = 0; off < curblock->size; off += ret)
    {

      /* Fetch the current instruction */
      XALLOC(__FILE__, __FUNCTION__, __LINE__, cur, sizeof(asm_instr), -1);

      ret = asm_read_instr(cur, (u_char *) blocdata + off, 
			   curblock->size - off + 10, world.curjob->proc);
      if (ret < 0)
	{
	  list_destroy(instrlist);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to fetch code for basic bloc", -1);
	}

      /* Bind the instruction on a special variable */
      /*
      snprintf(logbuf, sizeof (logbuf), "$instr_%08X", curblock->vaddr + off); 
      expr = revm_inform_type_addr(curtype->name, strdup(logbuf),
				   (elfsh_Addr) &cur, NULL, 0, 1);
      if (!expr)
	{
	  fprintf(stderr, "Unable to reflect instr at addr %08X (off = %u, typename = %s) \n", 
		  curblock->vaddr + off, off, curtype->name);
	  
	  list_destroy(instrlist);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to reflect instruction in bloc", -1);
	}
      */

      /* Also add the instruction to the current reflected list for this block */
      instrcontainer = container_create(curtype->type, cur, NULL, NULL);
      snprintf(logbuf, sizeof (logbuf), "%08X", curblock->vaddr + off); 
      list_add(instrlist, strdup(logbuf), instrcontainer);
    }

  /* Inverse the list and add it */
  instrlist = list_reverse(instrlist);
  hash_add(&instrlists_hash, strdup(logbuf), instrlist);

  /* Printing message if we are not in quiet mode */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, sizeof(logbuf), 
	       " [*] Basic bloc at address %08X reflected succesfully \n\n", 
	       curblock->vaddr);
      revm_output(logbuf);
    }

  /* Put the current bloc in the last result variable */
  expr		      = revm_expr_get(REVM_VAR_RESULT);
  last                = expr->value;
  last->immed_val.ent = (elfsh_Addr) container;
  last->immed         = 1;
  last->otype         = aspect_type_get_by_id(container->type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
