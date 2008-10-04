/**
 * @file op_nop.c
 * @ingroup handlers_ia32
 * $Id: op_nop.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
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

int op_nop(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_NOP;
  new->type = ASM_TYPE_NOP;
  return (new->len);
}
