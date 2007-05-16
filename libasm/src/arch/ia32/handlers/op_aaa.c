/*
** $Id: op_aaa.c,v 1.3 2007-05-16 19:21:58 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the aaa instruction, opcode 0x37
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of disassembled instruction.
*/

int     op_aaa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_AAA;
  new->type = ASM_TYPE_CONTROL | ASM_TYPE_ARITH;
  new->flags = ASM_FLAG_AF | ASM_FLAG_CF;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
