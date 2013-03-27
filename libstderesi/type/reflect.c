/**
 * @file libstderesi/type/reflect.c
 * @ingroup type
 * @brief Handle the match construct.
 *
 * Start on Wed May 23 13:55:45 2007 jfv
 *
 * $Id$
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
  eresi_Addr	daddr;
  u_int		off;
  int		ret;
  aspectype_t	*curtype;
  void		*blocdata;
  int		fileoff;
  list_t	*instrlist;
  revmexpr_t	*expr;
  u_int		insnbr;
  u_int		reqnbr;
  u_int		readsize;
  revmexpr_t	*immed;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  curtype  = aspect_type_get_by_name("instr");
  if (!curtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Failed reflection : unknown type : instr", -1);

  /* Validate parameters */
  if (world.curjob->curcmd->argc != 1 && world.curjob->curcmd->argc != 2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Command expect one or two parameters \n", -1);
  
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
	case EM_ARM:
          world.curjob->proc = &world.proc_arm;
	  break;
	case EM_MIPS:
	case EM_MIPS_RS3_LE:
          world.curjob->proc = &world.proc_mips;
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
  immed = revm_lookup_param(world.curjob->curcmd->param[0], 1);
  if (!immed)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Failed to lookup parameter address expr", -1);
  if (revm_convert_object(immed, ASPECT_TYPE_CADDR) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid address parameter", 0);
  addr = (immed->value->immed ? immed->value->immed_val.ent : 
	  immed->value->get_obj(immed->value->parent));

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

  /* One more param enables lightweight mode: lookup data from a binary file section - no CFG needed */
  if (world.curjob->curcmd->argc == 2)
    {
      immed = revm_lookup_param(world.curjob->curcmd->param[1], 1);
      if (revm_convert_object(immed, ASPECT_TYPE_INT) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid size parameter", 0);
      reqnbr = (immed->value->immed ? (u_int) immed->value->immed_val.ent : 
		(u_int) immed->value->get_obj(immed->value->parent));
      if (reqnbr <= 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid instruction number to reflect", -1);
      readsize = 16 * reqnbr;
      blocdata = alloca(readsize);
    }

  /* Only one param (an addr/symbol) --> Lookup data from a basic block on the CFG */
  else
    {
      container = mjr_block_get_by_vaddr(world.mjr_session.cur, addr, MJR_BLOCK_GET_STRICT);
      if (!container)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to find bloc at this virtual address", -1);
      curblock = (mjrblock_t *) container->data;
      readsize = curblock->size;
      blocdata = alloca(readsize);
      reqnbr = 0;
    }

  /* Read data -- could be imported from a remote process, so needs to copy buffer */
  fileoff = elfsh_get_foffset_from_vaddr(world.curjob->curfile, addr);
  if (elfsh_readmemf(world.curjob->curfile, fileoff, blocdata, readsize) != readsize)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to read data to reflect", -1);

  /* Create the new list of instructions in expression form */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, instrlist, sizeof(list_t), -1);
  snprintf(logbuf2, sizeof(logbuf2), AFMT, addr);
  elist_init(instrlist, strdup(logbuf2), ASPECT_TYPE_EXPR);

  /* Reflection all instructions of the basic bloc in the list */
  for (insnbr = off = 0; off < readsize; off += ret, insnbr++)      
    {

      /* If reached the number of requested instruction, stop now even without reaching buffer end */
      if (reqnbr && insnbr == reqnbr)			
	break;

      /* Fetch the current instruction */
      ret = asm_read_instr(&cur, (u_char *) blocdata + off,
                           readsize - off + 10, world.curjob->proc);
      if (ret < 0)
        {
          elist_destroy(instrlist);
          PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                       "Unable to fetch code for basic bloc", -1);
        }

      /* Also add the instruction to the current reflected list for this block */
      instrcontainer = container_create(curtype->type, (void *) &cur, NULL, NULL, 0);
      snprintf(logbuf, sizeof (logbuf), "$instr-"XFMT, addr + off);
      daddr = (eresi_Addr) instrcontainer;
      expr = revm_inform_type_addr(curtype->name, strdup(logbuf), daddr, NULL, 0, 1);
      elist_add(instrlist, strdup(logbuf), expr);
    }

  /* Reverse instrlist and add it to the hash of lists */
  instrlist = elist_reverse(instrlist);
  hash_add(&instrlists_hash, strdup(logbuf2), instrlist);

  /* Printing message if we are not in quiet mode */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, sizeof(logbuf),
               " [*] Code at address " AFMT " reflected succesfully (%u instrs) \n\n",
               addr, insnbr);
      revm_output(logbuf);
    }

  /* Put the current bloc in the last result variable */
  /*
  revm_expr_destroy_by_name(REVM_VAR_RESULT);
  revm_expr_copy(expr, REVM_VAR_RESULT);
  */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
