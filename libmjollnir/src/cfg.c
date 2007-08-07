/*
** (C) 2006-2007 Devhell Labs / Asgard Labs 
**  - sk, jfv, thorolf, strauss
**
** @file libmjollnir/types.c
** 
** @brief Functions that use the typed instructions information in libasm
**
** $Id: cfg.c,v 1.7 2007-08-07 07:13:27 may Exp $
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
					  elfshobj_t    *obj, 
					  asm_instr     *curins, 
					  elfsh_Addr	curvaddr)
{
  int			ilen;
  container_t	*fun;
  elfsh_Addr		dstaddr;
  u_int			addend;

  /* Initialize stuffs */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  context->obj = obj;
  mjr_history_write(context, curins, curvaddr, MJR_HISTORY_CUR);
  mjr_history_write(context, curins, curvaddr, 0);
  ilen = asm_instr_len(curins);  
  fun = mjr_function_get_by_vaddr(context, curvaddr);
  if (fun)
    context->curfunc = fun;
  else 
    mjr_asm_check_function_start(context);
  fun = NULL;

  /* Switch on instruction types provided by libasm */
  if (curins->type == ASM_TYPE_CONDBRANCH)
    {
      dstaddr = mjr_get_jmp_destaddr(context);
      
#if __DEBUG_FLOW__
    fprintf(D_DESC,
	    "[D] %s: " XFMT " ASM_TYPE_CONDBRANCH T:" XFMT
	    " F:" XFMT"\n", __FUNCTION__, curvaddr, dstaddr, curvaddr + ilen);
#endif

    if (dstaddr != -1)
      mjr_link_block_jump(context, curvaddr, dstaddr, curvaddr + ilen);

    }
  else if (curins->type == ASM_TYPE_IMPBRANCH)
    {
      dstaddr = mjr_get_jmp_destaddr(context);
      
#if __DEBUG_FLOW__
      fprintf(D_DESC,
	      "[D] mjr_asm_flow: " XFMT " ASM_TYPE_IMPBRANCH  T:" XFMT 
	      " F: NULL \n", curvaddr, dstaddr);
#endif

      if (dstaddr != (elfsh_Addr) -1)
	mjr_link_block_jump(context, curvaddr, dstaddr, 0);

    }
  else if (curins->type & ASM_TYPE_CALLPROC)
    {
      dstaddr = mjr_get_call_destaddr(context);
      
#if __DEBUG_FLOW__
      fprintf(D_DESC,
	      "[D] %s: " XFMT " ASM_TYPE_CALLPROC  T:" XFMT
	      " F:" XFMT "\n", __FUNCTION__, curvaddr, dstaddr, curvaddr + ilen);
#endif
      
      context->calls_seen++;

      /* For delay slot */
      addend = (context->proc.type == ASM_PROC_SPARC ? 4 : 0);

      /* 20070102
       * FIXME: we should be able to resolve CALL 0x0 (dstaddr == 0), 
       * Possible libasm or mjollnir bug.
       */
      if (dstaddr && dstaddr != (elfsh_Addr) -1)
    	{
	  /* Link block layer */
	  mjr_link_block_call(context, curvaddr, dstaddr, curvaddr + ilen + addend);

	  /* Link function layer */
	  mjr_link_func_call(context, curvaddr, dstaddr, curvaddr + ilen + addend);
    	}
    }
  else if (curins->type == ASM_TYPE_RETPROC)
    {
      
#if __DEBUG_FLOW__
      fprintf(D_DESC,"[D] %s: " XFMT " ASM_TYPE_RETPROC\n",
	      __FUNCTION__, curvaddr);
#endif
    }
  else
    {
#if __DEBUG_FLOW__
      fprintf(D_DESC,"[D] %s: CUR: %x DEFAULT %d\n", 
	      __FUNCTION__, curvaddr, curins->type);
#endif
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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
elfsh_Addr	mjr_compute_fctptr(mjrcontext_t	*context)
{
  char		tmp[255];
  char		*ret;
  elfsh_Addr	dest;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  printf(" [*] Found function pointer called from 0x%08lx\n", 
	 (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr);
  snprintf(tmp, sizeof(tmp), "%lx", 
	   (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr);

  /* We deal with a constructed address */

  /* Specific pattern (simple one) */
  /* As we dont do any real dataflow analysis yet, we dont handle automatically
     complex case where addresses are  constructed in a complex way */

  if (context->hist[MJR_HISTORY_CUR].instr.instr   == ASM_CALL &&
      context->hist[MJR_HISTORY_PREV].instr.instr  == ASM_MOV  &&
      context->hist[MJR_HISTORY_PPREV].instr.instr == ASM_MOV)
    {
      dest = context->hist[MJR_HISTORY_PPREV].instr.op2.imm;

      if (dest < elfsh_get_entrypoint(context->obj->hdr))
	{
	  printf(" [*] FAILED to resolve function pointer called from 0x%08lx \n", 
		 (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr);

	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid target vaddr for function pointer",
		       (elfsh_Addr) -1);
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
     and make the graph continuous even with complex address construction */

  /* Happens when an address was manually inserted in the routing table */
  /* This allow to avoid the control flow graph to be broken if elfsh
     is not capable to recompute the target address */
  /* So the keys for this hash tables are the vaddr of the instruction that does
     the complex function pointer call */
  
  ret = (char *) hash_get(&goto_hash, tmp);
  if (!ret) 
    {
      printf(" [*] UNABLE to resolve function pointer called from 0x%08lx\n", 
	     (unsigned long) context->hist[MJR_HISTORY_CUR].vaddr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to compute function pointer target", (elfsh_Addr) -1);
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
int		mjr_get_call_destaddr(mjrcontext_t *context)
{
  int		ilen;
  elfsh_Addr	dest;
  asm_instr	*ins;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = 0;
  ins  = &context->hist[MJR_HISTORY_CUR].instr;
  
  if (context->proc.type == ASM_PROC_IA32)
  {
  	/* The target block is called directly */
    if ((ins->op1.content & ASM_OP_VALUE) && !(ins->op1.content & ASM_OP_REFERENCE)) 
    {    
    	ilen = asm_instr_len(ins);
    	asm_operand_get_immediate(ins, 1, 0, &dest);
    	dest += ilen + context->hist[MJR_HISTORY_CUR].vaddr;
    }
    /* The target block is called indirectly : if we find a pattern that correspond 
       to an easy to predict function pointer, then we compute it */
    else if (ins->op1.content & ASM_OP_BASE)
      dest = mjr_compute_fctptr(context);
    else
      dest = -1;
  }
  else if (context->proc.type == ASM_PROC_SPARC)
  {
    if (ins->instr == ASM_SP_CALL)
    {
    	if (ins->op1.type == ASM_SP_OTYPE_DISP30)
   	  {
  	    dest = (ins->op1.imm * 4) + context->hist[MJR_HISTORY_CUR].vaddr;
   	  }
    	else /* Indirect call (special case of JMPL) */
    	  dest = -1;
    }
  }
  else
    dest = -1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}

/**
 * @brief Resolve the destination address of current jmp instruction
 * @param context mjollnir context structure
 */
int		mjr_get_jmp_destaddr(mjrcontext_t *context)
{
  int		ilen;
  elfsh_Addr	dest;
  asm_instr	*ins;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = 0;
  ins  = &context->hist[MJR_HISTORY_CUR].instr;
  
  if (context->proc.type == ASM_PROC_IA32)
  {
  	/* The target block is called directly */
    if ((ins->op1.content & ASM_OP_VALUE) && !(ins->op1.content & ASM_OP_REFERENCE)) 
    {    
    	ilen = asm_instr_len(ins);
    	asm_operand_get_immediate(ins, 1, 0, &dest);
    	dest += ilen + context->hist[MJR_HISTORY_CUR].vaddr;
    }
    
    /* The target block is called indirectly : if we find a pattern that correspond 
       to an easy to predict function pointer, then we compute it */
    else if (ins->op1.content & ASM_OP_BASE)
      dest = mjr_compute_fctptr(context);
    else
      dest = -1;
  }
  else if (context->proc.type == ASM_PROC_SPARC)
  {
    if (ins->instr == ASM_SP_JMPL) /* Indirect jump */
      {
    	dest = -1;
      }
    else if (ins->type == ASM_TYPE_CONDBRANCH)
      {
    	dest = (ins->op1.imm * 4) + context->hist[MJR_HISTORY_CUR].vaddr;
      }
  }
  else
    dest = -1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}

/**
 * @brief Check if we missed some function start
 * @param ctxt mjollnir context structure
 */
int			mjr_asm_check_function_start(mjrcontext_t *ctxt)
{
  char			*tmpstr;
  u_int			tmpaddr;
  container_t	*fun;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* check function prologue */
  if (ctxt->proc.type == ASM_PROC_IA32)
  {
    if (ctxt->hist[MJR_HISTORY_CUR].instr.instr   == ASM_SUB &&
    	  ctxt->hist[MJR_HISTORY_PREV].instr.instr  == ASM_MOV &&
    	  ctxt->hist[MJR_HISTORY_PPREV].instr.instr == ASM_PUSH)
      {
	tmpstr = _vaddr2str(ctxt->hist[MJR_HISTORY_PPREV].vaddr);
	tmpaddr = ctxt->hist[MJR_HISTORY_PPREV].vaddr;
	
#if __DEBUG_FLOW__
	fprintf(D_DESC,"[D] %s: function start found at %x for %x\n",
		__FUNCTION__, ctxt->hist[MJR_HISTORY_CUR].vaddr, tmpaddr);
#endif
	fun = mjr_create_function_container(ctxt, tmpaddr, 0, tmpstr, NULL, NULL);
	mjr_function_register(ctxt, tmpaddr, fun);
	ctxt->curfunc = mjr_function_get_by_vaddr(ctxt, tmpaddr);
      }
  }
  else if (ctxt->proc.type == ASM_PROC_SPARC)
  {
    if (ctxt->hist[MJR_HISTORY_CUR].instr.instr == ASM_SP_SAVE &&
        ctxt->hist[MJR_HISTORY_CUR].instr.op1.baser == ASM_REG_O6 &&
        ctxt->hist[MJR_HISTORY_CUR].instr.op2.type == ASM_SP_OTYPE_IMMEDIATE &&
        ctxt->hist[MJR_HISTORY_CUR].instr.op3.baser == ASM_REG_O6)
  	{
  	  tmpstr = _vaddr2str(ctxt->hist[MJR_HISTORY_CUR].vaddr);
  	  tmpaddr = ctxt->hist[MJR_HISTORY_CUR].vaddr;

#if __DEBUG_FLOW__
  	  fprintf(D_DESC,"[D] %s: function start found at %x for %x\n",
        		  __FUNCTION__, ctxt->hist[MJR_HISTORY_CUR].vaddr, tmpaddr);
#endif

  	  fun = mjr_create_function_container(ctxt, tmpaddr, 0, tmpstr, NULL, NULL);
	  mjr_function_register(ctxt, tmpaddr, fun);
  	  ctxt->curfunc = mjr_function_get_by_vaddr(ctxt, tmpaddr);
  	}
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

