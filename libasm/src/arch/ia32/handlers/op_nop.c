/**
 * @file op_nop.c
 * @ingroup libasm/ia32/handlers
 * $Id$
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
