/*
** $Id: op_pop_ss.c,v 1.5 2007-06-27 11:25:12 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for pop es instruction. Opcode = 0x17
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 */

int     op_pop_ss(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc)
{
  new->instr = ASM_POP;
  new->len += 1;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = 4;
  new->ptr_instr = opcode;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.baser = ASM_REG_SS;

#else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.baser = ASM_REG_SS;
  
#endif
  return (new->len);
}
