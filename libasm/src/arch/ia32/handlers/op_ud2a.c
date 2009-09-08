/**
 * @file op_ud2a.c
ngroup ia-instrs/g
 * @ingroup instrs
 * @brief Handler for instruction ud2a opcode 0x??
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction ud2a opcode 0x??
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int     op_ud2a(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_UD2A;
  return (new->len);
}
