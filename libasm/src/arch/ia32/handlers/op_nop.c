/**
 * @file op_nop.c
 * $Id: op_nop.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the nop instruction, opcode 0x90
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of the instruction.
 */

int op_nop(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_NOP;
  new->type = ASM_TYPE_NOP;
  return (new->len);
}
