/*
** types.c in libmjollnir for elfsh
** 
** Functions that use the typed instructions information in libasm
**
** Made by mayhem, thorkill, sk
**
** Updated Thu Dec 29 16:14:39 2006 mayhem
*/
#include "libmjollnir.h"
  


/*
**  Depending on instruction type -based on IA32 instruction set-
** 
**  ASM_TYPE_CONDBRANCH: jcc, loop, MAY NOT break execution flow
**  ASM_TYPE_CALLPROC: calls break instruction flow but CAN restore it
**  ASM_TYPE_IMPBRANCH, ASM_TYPE_RETPROC: jmp and ret break execution flow
**  
**  The last two types reset g_prevaddr as execution flow won't be restored
**  to following instruction.
**
** FIXME : this function must be tested on other architectures
**
*/
int		mjr_asm_flow(mjrcontext_t *context)
{
  int		ilen;
  char		*md5;
  mjrfunction_t	*fun;
  asm_instr	*curins;
  elfsh_Addr	curvaddr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  curins   = &context->hist[MJR_HISTORY_CUR].instr;
  curvaddr = context->hist[MJR_HISTORY_CUR].vaddr;
  ilen     = asm_instr_len(curins);

  /* Switch on instruction types provided by libasm */
  switch (curins->type) 
    {
    case ASM_TYPE_CONDBRANCH:
      context->curblock->contig = curvaddr + ilen;
      context->curblock->altern = mjr_insert_destaddr(context);
      context->curblock->altype = CALLER_JUMP;
      context->curblock = 0;
      break;
    case ASM_TYPE_CALLPROC:
      context->curblock->contig = curvaddr + ilen;
      context->curblock->altern = mjr_insert_destaddr(context);
      context->curblock->altype = CALLER_CALL;
      context->curblock = 0;
      context->calls_seen++;
      if (context->curblock->altern)
	context->calls_found++;

      /* XXX: put this in a vector of fingerprinting techniques */
      fun = mjr_function_create(context->curblock->altern);
      md5 = mjr_fingerprint_function(context, 
				     context->curblock->altern, 
				     MJR_FNG_TYPE_MD5);
      if (md5)
	fun->md5 = elfsh_strdup(md5);
      break;

    case ASM_TYPE_IMPBRANCH:
      context->curblock->altern = mjr_insert_destaddr(context);
      context->curblock->altype = CALLER_JUMP;
      context->curblock         = 0;
      context->hist[MJR_HISTORY_PREV].vaddr = 0;
      break;
    case ASM_TYPE_RETPROC:
      context->curblock->contig = 0;
      context->curblock->altern = 0;
      context->curblock->altype = CALLER_RET;
      context->curblock         = 0;
      context->hist[MJR_HISTORY_PREV].vaddr = 0;
      break;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* 
** Support function pointers computations
**
** Handle something like this : call *%reg
**     
**  -> Check goto table and fixup destination
**
**  - start gdb, set breakpoint on a known pointer function, run the prog
**  - check register value at breakpoint
**  - start elfsh
**  - addgoto <vaddr of call *%reg> <destaddr value of *%reg>   
*/
elfsh_Addr		mjr_compute_fctptr(mjrcontext_t	*context)
{
  char		tmp[255];
  char		*ret;
  elfsh_Addr	dest;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  printf(" [*] Found function pointer at %x\n", 
	 context->hist[MJR_HISTORY_CUR].vaddr);
  snprintf(tmp, sizeof(tmp), "%x", context->hist[MJR_HISTORY_CUR].vaddr);

  /* We deal with a constructed address */
  if (context->hist[MJR_HISTORY_CUR].instr.instr   == ASM_CALL &&
      context->hist[MJR_HISTORY_PREV].instr.instr  == ASM_MOV  &&
      context->hist[MJR_HISTORY_PPREV].instr.instr == ASM_MOV) 
    {
      dest = context->hist[MJR_HISTORY_PPREV].instr.op2.imm;
      
      if (dest < elfsh_get_entrypoint(context->obj->hdr)) 
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid target vaddr for function pointer", 
			  (elfsh_Addr) -1);

      /* Resolve target block */
      mjr_block_point(&context->blklist, &context->hist[MJR_HISTORY_CUR].instr, 
		      context->hist[MJR_HISTORY_CUR].vaddr, dest);

#if defined(__DEBUG_MJOLLNIR__)
      printf(" [*] 0x%08x Detected possible FUNCPTR at [%x/%d] \n",
	     context->hist[MJR_HISTORY_CUR].vaddr, dest, dest);
#endif

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
    } 
  
  /* Happens when an address was manually inserted in the routing table */
  /* This allow to avoid the control flow graph to be broken if elfsh
     is not capable to recompute the target address */
  ret = (char *) hash_get(&goto_hash, tmp);
  if (!ret) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to compute function pointer target", 
		      (elfsh_Addr) -1);  
  dest = strtol(ret, (char **) NULL, 16);
  if (dest) 
    {

#if defined(__DEBUG_MJOLLNIR__)
      printf(" [*] Extended routing table found 0x%08x -> 0x%08x\n", 
	     context->hist[MJR_HISTORY_CUR].vaddr, dest);
#endif

      /* Resolve target block */
      mjr_block_point(&context->blklist, &context->hist[MJR_HISTORY_CUR].instr,
		      context->hist[MJR_HISTORY_CUR].vaddr, dest);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}





/*
** This function add a new element to the linked list of callers 
** of the current block.
** 
** It resolves operands of instruction which may modify the execution path.
** 
** If resolved to a virtual address, a new block is inserted.
**
** -> Return destination address inserted or -1 if unresolved
*/
int		mjr_insert_destaddr(mjrcontext_t *context)
{
  int		ilen;
  elfsh_Addr	dest;
  asm_instr	*ins;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = 0;
  ins  = &context->hist[MJR_HISTORY_CUR].instr;

  /* The target block is called directly */
  if ((ins->op1.content & ASM_OP_VALUE) && !(ins->op1.content & ASM_OP_REFERENCE)) 
    {    
      ilen    = asm_instr_len(ins);
      asm_operand_get_immediate(ins, 1, 0, &dest);
      dest   += ilen + context->hist[MJR_HISTORY_CUR].vaddr;
      mjr_block_point(&context->blklist, ins, 
		      context->hist[MJR_HISTORY_CUR].vaddr, dest);
    }

  /* The target block is called indirectly : if we find a pattern that correspond 
     to an easy to predict function pointer, then we compute it */
  else if (ins->op1.content & ASM_OP_BASE) 
    dest = mjr_compute_fctptr(context);
  else
    dest = -1;
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}






/* Old function of libmjollnir, maybe it does more than insert_destaddr, to make
   sure we can remove this */
/*
  int	mjr_get_calldest(mjrsession_t *sess, int *dest) 
  {
  int   ret = 0;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (sess->cur->ihist[0].instr != ASM_CALL)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(ret));
  
  #if __DEBUG_CALLS__
  fprintf(D_DESC, "[__DEBUG_CALLS__] mjrGetCallDst: start - 0x%08x %d\n",
  sess->cur->curVaddr, sess->cur->ihist[0].op1.content);
  #endif
  
  if ((sess->cur->ihist[0].op1.content & ASM_OP_VALUE) &&
  !(sess->cur->ihist[0].op1.content & ASM_OP_REFERENCE))
  {
  if (asm_operand_get_immediate(&sess->cur->ihist[0], 1, 0, dest) == -1) 
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
  "Failed to get immediate operand", -1);
  ret = 1;
  }
  
  else if ((sess->cur->ihist[0].op1.content & ASM_OP_ADDRESS) &&
  sess->cur->ihist[1].instr == ASM_MOV &&
  sess->cur->ihist[2].instr == ASM_MOV &&
  !(sess->cur->ihist[2].op1.content & ASM_OP_SCALE) &&
  !(sess->cur->ihist[2].op2.content & ASM_OP_REFERENCE) &&
  sess->cur->ihist[2].op2.imm != 0x0) 
  {
  
  #if __DEBUG_OPERAND__
  asm_debug_operand(&sess->cur->ihist[0].op1);
  asm_debug_operand(&sess->cur->ihist[0].op2);
  asm_debug_operand(&sess->cur->ihist[2].op1);
  asm_debug_operand(&sess->cur->ihist[2].op2);
  #endif
  
  dest = (int *)sess->cur->ihist[2].op2.imm;
  
  #if __DEBUG_PTR__
  fprintf(D_DESC, "[__DEBUG_PTR__] mjrGetCallDst: PTR FUNCTION ! %x %x\n",
  sess->cur->curVaddr, (int)dest);
  #endif
  ret = 1;
  } 
  else
  {
  
  #if __DEBUG_OPERAND__
  fprintf(D_DESC, " [DM] Operand not supported! [vaddr = 0x%08x] %02X\n",
  sess->cur->curVaddr, sess->cur->ihist[0].op1.content);
  #endif
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
  "Operand content not supported.", -1);
  }
  
  #if __DEBUG_CALLS__
  fprintf(D_DESC, "[__DEBUG_CALLS__] mjrGetCallDst: vaddr: %x dest: %x\n",
  sess->cur->curVaddr,
  *dest);
  #endif
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(ret));
  }
*/
