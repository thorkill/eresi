/**
 * @file op_lea_rv_m
 * $Id: op_lea_rv_m.c,v 1.3 2007-05-09 16:26:23 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
  <instruction func="op_lea_rv_m" opcode="0x8d"/>
*/

int op_lea_rv_m(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_LEA;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + new->op1.len, ASM_OTYPE_MEMORY, proc);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_MEMORY;
  
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
