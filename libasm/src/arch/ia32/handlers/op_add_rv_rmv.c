/*
** $Id: op_add_rv_rmv.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the add rv, rmv instruction, opcode 0x03
 * @param new Pointer to the instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of the data to disassemble.
 * @param proc Pointer to the processor structure.
 * @return Length of the disassembled instruction.
 */

int op_add_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_ADD;
  new->type = ASM_TYPE_ARITH;
  new->len++;
  new->ptr_instr = opcode;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, proc);
#else  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
