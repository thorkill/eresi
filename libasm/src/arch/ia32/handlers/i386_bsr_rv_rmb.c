/*
** $Id: i386_bsr_rv_rmb.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction bsr rv, rms, opcode 0x0f 0xbd
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int i386_bsr_rv_rmb(asm_instr *new, u_char *opcode, u_int len, 
		    asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_BSR;
    
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, 
				new);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
    
  operand_rv_rmb(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
