/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: destCall.c,v 1.5 2006-12-11 13:57:39 may Exp $
 *
 */
#include "libmjollnir.h"



int	mjr_get_call_dst(mjrsession_t *sess, int *dest) 
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


/* Debug printing */
int	asm_debug_operand(asm_operand * op)
{
  fprintf(D_DESC, "[*] Operand Dump\n[*] len: %d type: %d size: %d content: %d\n",
	  op->len, op->type, op->size, op->content);
  
  fprintf(D_DESC, "[*] Content: %s %s %s %s %s %s %s\n",
	  (op->content & ASM_OP_VALUE) ? "ASM_OP_VALUE" : ".",
	  (op->content & ASM_OP_BASE) ? "ASM_OP_BASE" : ".",
	  (op->content & ASM_OP_INDEX) ? "ASM_OP_INDEX" : ".",
	  (op->content & ASM_OP_SCALE) ? "ASM_OP_SCALE" : ".",
	  (op->content & ASM_OP_FIXED) ? "ASM_OP_FIXED" : ".",
	  (op->content & ASM_OP_REFERENCE) ? "ASM_OP_REFERENCE" : ".",
	  (op->content & ASM_OP_ADDRESS) ? "ASM_OP_ADDRESS" : ".");
  
  return 0;
}
