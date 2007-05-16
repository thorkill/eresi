/*
** $Id: i386_bsf.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction bsf, opcode 0x0f 0xbc
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 <i386 func="i386_bsf" opcode"0xbc"/>
*/

int i386_bsf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BSF;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_GENERAL, proc);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_ENCODED, proc);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rm2(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
