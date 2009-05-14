/*
** (C) 2006-2008 The ERESI team
**
** @file libmjollnir/types.c
** 
** @brief Top-level functions for control-flow analysis
**
*/
#include "libmjollnir.h"



/** 
 * @brief This function trace execution flow and creates block depending on instruction.
 *
 * If instruction break execution flow, block is considerated finished and added 
 * to linked list of blocks (that is the content of the .edfmt.blocks section)
 *
 *  Depending on instruction type -based on IA32 instruction set-
 *
 *  ASM_TYPE_CONDBRANCH: jcc, loop, MAY NOT break execution flow
 *  ASM_TYPE_CALLPROC: calls break instruction flow but CAN restore it
 *  ASM_TYPE_IMPBRANCH, ASM_TYPE_RETPROC: jmp and ret break execution flow
 *
 *  The last two types reset g_prevaddr as execution flow won't be restored
 *  to following instruction.
 *
 * FIXME : this function must be tested on other architectures
 *
 */
int			mjr_trace_control(mjrcontext_t *context,
					  container_t	*curblock,
					  elfshobj_t    *obj, 
					  asm_instr     *curins, 
					  eresi_Addr	curvaddr,
					  eresi_Addr	*dstaddr,
					  eresi_Addr	*retaddr)
{
  int			ilen;
  container_t		*bloc;
  u_int			addend;
  elfshsect_t		*sect;

  /* Initialize stuffs */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  context->obj = obj;
  mjr_history_write(context, curins, curvaddr, MJR_HISTORY_CUR);
  mjr_history_write(context, curins, curvaddr, 0);
  ilen = asm_instr_len(curins);  

  /* It might be that we are reaching the beginning of an existing bloc */
  bloc = mjr_block_get_by_vaddr(context, curvaddr, MJR_BLOCK_GET_STRICT);
  if (bloc && bloc->id != curblock->id)
    {
      *dstaddr = MJR_BLOCK_EXIST;
      *retaddr = MJR_BLOCK_INVALID;
      mjr_container_add_link(context, curblock, bloc->id, MJR_LINK_BLOCK_COND_ALWAYS, 
			     MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
      mjr_container_add_link(context, bloc, curblock->id, MJR_LINK_BLOCK_COND_ALWAYS, 
			     MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_IN);
      
#if __DEBUG_FLOW__
      fprintf(stderr, " [D] Found contiguous block ! stopping this recursion branch ******** \n");
#endif
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  //mjr_asm_check_function_start(context);

  /* Switch on instruction types provided by libasm */
  if (curins->type & ASM_TYPE_CONDBRANCH)
    {
      /* MIPS use delay slots for jump instructions too */
      addend = (context->proc.type == ASM_PROC_MIPS || 
		context->proc.type == ASM_PROC_SPARC ? 4 : 0);
      
      *dstaddr = mjr_get_jmp_destaddr(context);
      
#if __DEBUG_FLOW__
      fprintf(D_DESC,
	      "[D] %s:%d " XFMT " ASM_TYPE_CONDBRANCH T:" XFMT
	      " F:" XFMT"\n", __FUNCTION__, __LINE__, 
	      curvaddr, *dstaddr, curvaddr + ilen + addend);
#endif

      *retaddr = curvaddr + ilen + addend;      
      mjr_link_block_jump(context, curvaddr, *dstaddr, *retaddr);
    }
  else if (curins->type & ASM_TYPE_IMPBRANCH)
    {
      *dstaddr = mjr_get_jmp_destaddr(context);
      
#if __DEBUG_FLOW__
      fprintf(D_DESC,
	      "[D] mjr_asm_flow: " XFMT " ASM_TYPE_IMPBRANCH  T:" XFMT 
	      " F: NULL \n", curvaddr, *dstaddr);
#endif
      
      if (*dstaddr != MJR_BLOCK_INVALID)
	mjr_link_block_jump(context, curvaddr, *dstaddr, MJR_BLOCK_INVALID);
    }
  
  else if (curins->type & ASM_TYPE_CALLPROC)
    {
      *dstaddr = mjr_get_call_destaddr(context);
      
#if __DEBUG_FLOW__
      fprintf(D_DESC,
	      "[D] %s:%d " XFMT " ASM_TYPE_CALLPROC  T:" XFMT
	      " F:" XFMT "\n", __FUNCTION__, __LINE__, 
	      curvaddr, *dstaddr, curvaddr + ilen);
#endif
      
      context->calls_seen++;
      
      /* SPARC and MIPS use delay slots */
      addend = (context->proc.type == ASM_PROC_SPARC || context->proc.type == ASM_PROC_MIPS ? 4 : 0);
      
      /* If call occured at the end of a section */
      sect = elfsh_get_parent_section(context->obj, curvaddr + ilen + addend, NULL);
      if (!sect)
	{

#if __DEBUG_FLOW__
	  fprintf(D_DESC,"[W] *** %s: unusual retaddr found - expected ret: "
		  XFMT" section end is below this address. ***\n",
		  __FUNCTION__, curvaddr + ilen + addend);
#endif
	  
	  *retaddr = MJR_BLOCK_INVALID;
	}
      else
	*retaddr = curvaddr + ilen + addend;

      /* Link block layer */
      mjr_link_block_call(context, curvaddr, *dstaddr, *retaddr);

      /* Link function layer */ 
      if (*dstaddr != MJR_BLOCK_INVALID)
	{
	  mjr_link_func_call(context, curvaddr, *dstaddr, *retaddr);
	  context->calls_found++;
	}
    }
  else if (curins->type & ASM_TYPE_RETPROC)
    {

      if (context->func_stack->elmnbr > 1)
	{
	  context->curfunc = elist_pop(context->func_stack);

#if __DEBUG_FLOW__
      fprintf(D_DESC, " *********** CURFUNC RET @ " XFMT " : to %s ******** \n",
	      curvaddr, (context->curfunc && context->curfunc->data ? 
			 ((mjrfunc_t *) context->curfunc->data)->name : "NULL"));
#endif

	}

#if __DEBUG_FLOW__
      else
	fprintf(D_DESC, " *********** RET @ " XFMT " : NO RET FROM %s ******** \n",
		curvaddr, (context->curfunc && context->curfunc->data ? 
			   ((mjrfunc_t *) context->curfunc->data)->name : "NULL"));
#endif

    }

#if __DEBUG_FLOW__
  else
    {
      fprintf(D_DESC,"[D] %s: CUR: %x DEFAULT %d\n", 
	      __FUNCTION__, curvaddr, curins->type);
    }
#endif


  /* Return the delay slot size if any */
  addend = (context->proc.type == ASM_PROC_MIPS || 
	    context->proc.type == ASM_PROC_SPARC ? 4 : 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, addend);
}






/**
 * @brief Support function pointers computations
 *
 * Handle something like this : call *%reg
 *
 *  -> Check goto table and fixup destination
 *
 *  - start gdb, set breakpoint on a known pointer function, run the prog
 *  - check register value at breakpoint
 *  - start elfsh
 *  - addgoto <vaddr of call *%reg> <destaddr value of *%reg> 
 *
 * TODO:
 *  history of mov to reg when second operand was an address ;-)
 *
 *  804933c:       89 d1                   mov    %edx,%ecx
 *  804933e:       89 15 c0 e2 04 08       mov    %edx,0x804e2c0
 *  ...
 *  804936c:       ff d2                   call   *%edx
 *
 * FIXME : this function must be ported to the SPARC architecture
 */
eresi_Addr	mjr_compute_fctptr(mjrcontext_t	*context)
{
  char		tmp[255];
  char		*ret;
  eresi_Addr	dest;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  printf(" [*] Found function pointer called from " XFMT "\n", 
	 context->hist[MJR_HISTORY_CUR].vaddr);
  snprintf(tmp, sizeof(tmp), AFMT, context->hist[MJR_HISTORY_CUR].vaddr);

  /* We deal with a constructed address */

  /* Specific pattern (simple one) */
  /* As we dont do any real dataflow analysis yet, we dont handle automatically
     complex case where addresses are  constructed in a complex way */

  if (context->hist[MJR_HISTORY_CUR].instr.instr   == ASM_CALL &&
      context->hist[MJR_HISTORY_PREV].instr.instr  == ASM_MOV  &&
      context->hist[MJR_HISTORY_PPREV].instr.instr == ASM_MOV)
    {
      dest = context->hist[MJR_HISTORY_PPREV].instr.op[1].imm;

      if (dest < elfsh_get_entrypoint(context->obj->hdr))
	{
	  printf(" [*] FAILED to resolve function pointer called from " XFMT "\n", 
		 context->hist[MJR_HISTORY_CUR].vaddr);

	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid target vaddr for function pointer",
		       MJR_BLOCK_INVALID);
	}

#if __DEBUG_MJOLLNIR__
      printf(" [*] 0x%lx Detected possible FUNCPTR at [%lx/%ld] \n",
	     (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr,
	     (unsigned long) dest, (unsigned long) dest);
#endif
      
      printf(" [*] RESOLVED function pointer called from 0x%08lx -> 0x%08lx \n", 
	     (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr, 
	     (unsigned long) dest);
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
    }

  /* Instead we have a hash table that we can add element to using the 'addgoto'
     command of elfsh. That allow to tell which is the target address manually
     and connect two graphs even with complex address construction */
  /* Happens when an address was manually inserted in the routing table */
  /* This allow to avoid the control flow graph to be broken if elfsh
     is not capable to recompute the target address */
  /* So the keys for this hash tables are the vaddr of the instruction that does
     the complex function pointer call */
  
  ret = (char *) hash_get(&context->goto_hash, tmp);
  if (!ret) 
    {
      printf(" [*] UNABLE to resolve function pointer called from 0x%08lx\n", 
	     (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to compute function pointer target", MJR_BLOCK_INVALID);
    }

  dest = strtol(ret, (char **) NULL, 16);
  if (dest)
    {
#if __DEBUG_MJOLLNIR__
      printf(" [*] Extended routing table found 0x%lx -> 0x%lx\n",
	     (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr,
	     (unsigned long) dest);
#endif
    }

  printf(" [*] RESOLVED function pointer called from 0x%08lx -> 0x%08lx \n", 
	 (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr, 
	 (unsigned long) dest);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}

/**
 * @brief Resolve the destination address of current call 
 * @param context mjorllnir context strucutre
 */
eresi_Addr		 mjr_get_call_destaddr(mjrcontext_t *context)
{
  int			 ilen;
  eresi_Addr		 dest;
  asm_instr		 *ins;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = 0;
  ins  = &context->hist[MJR_HISTORY_CUR].instr;

  switch (context->proc.type)
    {
      
      /* INTEL architecture */
    case ASM_PROC_IA32:
      
      /* The target block is called directly */
      if ((ins->op[0].content & ASM_OP_VALUE) && !(ins->op[0].content & ASM_OP_REFERENCE)) 
	{    
	  ilen = asm_instr_len(ins);
	  asm_operand_get_immediate(ins, 1, 0, &dest);
	  dest = asm_dest_resolve(&context->proc, context->hist[MJR_HISTORY_CUR].vaddr, (dest + ilen));
	}
      /* The target block is called indirectly : if we find a pattern that correspond 
	 to an easy to predict function pointer, then we compute it */
      else if (ins->op[0].content & ASM_OP_BASE)
	dest = mjr_compute_fctptr(context);
      else
	dest = MJR_BLOCK_INVALID;
      break;

      /* SPARC architecture */
    case ASM_PROC_SPARC:
      if (ins->instr == ASM_SP_CALL)
	{
	  if (ins->op[0].content & ASM_SP_OTYPE_DISP30)
	    dest = (ins->op[0].imm * 4) + context->hist[MJR_HISTORY_CUR].vaddr;
	  else /* Indirect call (special case of JMPL) */
	    dest = MJR_BLOCK_INVALID;
	}
      break;

      /* MIPS architecture */
    case ASM_PROC_MIPS:
      switch (ins->instr)
	{
	case ASM_MIPS_JAL:
	  dest = (ins->op[0].imm << 2) | 
	    ((((context->hist[MJR_HISTORY_CUR].vaddr + 8) >> 28) & 0xF) << 28);
	  break;
	case ASM_MIPS_JALR:
	  dest = MJR_BLOCK_INVALID;
	  break;
	case ASM_MIPS_BAL:
	  dest = (context->hist[MJR_HISTORY_CUR].vaddr + (((short) ins->op[0].imm + 1) * 4));
	  break;
	default:
	  dest = (context->hist[MJR_HISTORY_CUR].vaddr + (((short) ins->op[1].imm + 1) * 4));
	  break;
	}
      break;

      /* ARM architecture */
    case ASM_PROC_ARM:
      switch(ins->instr)
        {
        case ASM_ARM_BL:
        case ASM_ARM_BLX1:
          dest = asm_dest_resolve_arm(context->hist[MJR_HISTORY_CUR].vaddr, ins->op[0].imm,
                                      (ins->op[0].content & ASM_ARM_OTYPE_DISP_HALF) ? 1 : 0);
          break;
        default:
          dest = MJR_BLOCK_INVALID;
          break;
        }
      break;
      
      /* Unknown architecture */
    default:
      dest = MJR_BLOCK_INVALID;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}


/**
 * @brief Resolve the destination address of current jmp instruction
 * @param context mjollnir context structure
 */
eresi_Addr	mjr_get_jmp_destaddr(mjrcontext_t *context)
{
  int		ilen;
  eresi_Addr	dest;
  asm_instr	*ins;
  unsigned int	i;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = 0;
  ins  = &context->hist[MJR_HISTORY_CUR].instr;
  
  switch (context->proc.type)
    {

      /* INTEL architecture */
    case  ASM_PROC_IA32:

      /* The target block is called directly */
      if ((ins->op[0].content & ASM_OP_VALUE) && !(ins->op[0].content & ASM_OP_REFERENCE)) 
	{    
	  ilen = asm_instr_len(ins);
	  asm_operand_get_immediate(ins, 1, 0, &dest);
	  dest = asm_dest_resolve(&context->proc, context->hist[MJR_HISTORY_CUR].vaddr, (dest + ilen));
	}
      
      /* The target block is called indirectly : if we find a pattern that correspond 
       to an easy to predict function pointer, then we compute it */
      else if (ins->op[0].content & ASM_OP_BASE)
	dest = mjr_compute_fctptr(context);
      else
	dest = MJR_BLOCK_INVALID;
      break;

      /* SPARC architecture */
    case ASM_PROC_SPARC:
      /*
	if (ins->instr & ASM_SP_JMPL)  //Indirect jump
	{
	printf("INDIRECT BRANCH TYPE FOR SPARC at addr "XFMT" ! \n", 
	context->hist[MJR_HISTORY_CUR].vaddr);
	dest = MJR_BLOCK_INVALID;
	}
      */
      if (ins->type & ASM_TYPE_CONDBRANCH || ins->type & ASM_TYPE_IMPBRANCH)
	dest = (ins->op[0].imm * 4) + context->hist[MJR_HISTORY_CUR].vaddr;
      else
	{
	  fprintf(stderr, " [D] UNKNOWN BRANCH FOR SPARC at addr "XFMT" ! \n", 
		  context->hist[MJR_HISTORY_CUR].vaddr);
	  dest = MJR_BLOCK_INVALID;
	}
      break;
  
      /* MIPS architecture */
    case ASM_PROC_MIPS:
      i = 0;
      if ( (ins->instr & ASM_MIPS_BEQ) || (ins->instr & ASM_MIPS_BEQL) 
	   || (ins->instr & ASM_MIPS_BNE) || (ins->instr & ASM_MIPS_BNEL) )
	i = 2;
      else if ( (ins->instr & ASM_MIPS_BGEZAL) || (ins->instr & ASM_MIPS_BGEZALL)
		|| (ins->instr & ASM_MIPS_BGEZ) || (ins->instr & ASM_MIPS_BGEZL)
		|| (ins->instr & ASM_MIPS_BGTZ) || (ins->instr & ASM_MIPS_BGTZL)
		|| (ins->instr & ASM_MIPS_BLEZ) || (ins->instr & ASM_MIPS_BLEZL)
		|| (ins->instr & ASM_MIPS_BLTZAL) || (ins->instr & ASM_MIPS_BLTZALL)
		|| (ins->instr & ASM_MIPS_BLTZ) || (ins->instr & ASM_MIPS_BLTZL) )
        i = 1;
      if (ins->op[i].type & ASM_MIPS_OTYPE_BRANCH)
        dest = (context->hist[MJR_HISTORY_CUR].vaddr + (((short)ins->op[i].imm+1) * 4));
      else if (ins->op[i].type & ASM_MIPS_OTYPE_JUMP)
        dest = (ins->op[i].imm << 2) | 
	  ((((context->hist[MJR_HISTORY_CUR].vaddr + 8) >> 28) & 0xF) << 28);
      else
        dest = MJR_BLOCK_INVALID; /* Jump to register - not yet */
      break;

      /* ARM architecture */
    case ASM_PROC_ARM:
      switch(ins->instr)
        {
        case ASM_ARM_B:
          dest = asm_dest_resolve_arm(context->hist[MJR_HISTORY_CUR].vaddr, ins->op[0].imm,
                                      (ins->op[0].content & ASM_ARM_OTYPE_DISP_HALF) ? 1 : 0);
          break;
        default:
          dest = MJR_BLOCK_INVALID;
          break;
        }
      break;

      /* Unknown architecture */
    default:
      dest = MJR_BLOCK_INVALID;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}



/**
 * @brief Check if we missed some function start
 * @param ctxt mjollnir context structure
 * @todo to remove ?
 */
/*
int			mjr_asm_check_function_start(mjrcontext_t *ctxt)
{
  char			*tmpstr;
  eresi_Addr		tmpaddr;
  container_t		*fun;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //check function prologue
  //FIXME: does not support function without frame pointer or without local variables,
  //- check func-without-fp signature
  //- check if history contains a call to us (to detect small functions)
  switch (ctxt->proc.type)
    {
      
      //IA32 architecture
    case ASM_PROC_IA32:
      if (ctxt->hist[MJR_HISTORY_CUR].instr.instr   == ASM_SUB &&
	  ctxt->hist[MJR_HISTORY_PREV].instr.instr  == ASM_MOV &&
	  ctxt->hist[MJR_HISTORY_PPREV].instr.instr == ASM_PUSH)
	{
	  tmpstr = _vaddr2str(ctxt->hist[MJR_HISTORY_PPREV].vaddr);
	  tmpaddr = ctxt->hist[MJR_HISTORY_PPREV].vaddr;
	  goto finish;
	}
      break;
  
      //SPARC architecture
    case ASM_PROC_SPARC:
      if (ctxt->hist[MJR_HISTORY_CUR].instr.instr == ASM_SP_SAVE &&
	  ctxt->hist[MJR_HISTORY_CUR].instr.op[0].baser == ASM_REG_O6 &&
	  ctxt->hist[MJR_HISTORY_CUR].instr.op[1].type == ASM_SP_OTYPE_IMMEDIATE &&
	  ctxt->hist[MJR_HISTORY_CUR].instr.op[2].baser == ASM_REG_O6)
  	{
  	  tmpstr = _vaddr2str(ctxt->hist[MJR_HISTORY_CUR].vaddr);
  	  tmpaddr = ctxt->hist[MJR_HISTORY_CUR].vaddr;
	  goto finish;
  	}
      break;
  
      //MIPS architecture
    case ASM_PROC_MIPS:
      if (ctxt->hist[MJR_HISTORY_CUR].instr.instr   == ASM_MIPS_SD &&
	  ctxt->hist[MJR_HISTORY_PREV].instr.instr == ASM_MIPS_ADDIU &&
	  ctxt->hist[MJR_HISTORY_PREV].instr.op[0].baser == ASM_MIPS_REG_SP)
	{
	  tmpstr = _vaddr2str(ctxt->hist[MJR_HISTORY_PREV].vaddr);
	  tmpaddr = ctxt->hist[MJR_HISTORY_PREV].vaddr;
	  //goto finish;
	  
	}
      break;
      
    default:
      PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
		   "Unsupported architecture in libmjollnir", -1);
    }
  
  //Nothing to declare
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  //We found a prolog
 finish:

#if __DEBUG_FLOW__
  fprintf(D_DESC," [D] FUNCTION START FOUND at "XFMT" for "XFMT" \n",
	  ctxt->hist[MJR_HISTORY_CUR].vaddr, tmpaddr);
#endif
  
  fun = mjr_function_get_by_vaddr(ctxt, tmpaddr);
  if (!fun)
    {
      fun = mjr_create_function_container(ctxt, tmpaddr, 0, tmpstr, NULL, NULL);
      mjr_function_register(ctxt, tmpaddr, fun);
      mjr_function_symbol(ctxt, fun);
      mjr_container_add_link(ctxt, ctxt->curfunc, fun->id,
			     MJR_LINK_FUNC_SLIDE, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
      elist_push(ctxt->func_stack, fun);
      ctxt->curfunc = fun;
      fprintf(stderr, " ******* ALLOCATED STACKFRAME IN MIDDLE OF FUNC at "XFMT" \n", tmpaddr);
      fprintf(stderr, " **** -> NOW NEW CURFUNC @ %s \n", ((mjrfunc_t *) fun->data)->name);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
*/
