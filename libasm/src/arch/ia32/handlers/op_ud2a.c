/**
 * @file op_ud2a.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction ud2a opcode 0x??
 * $Id: op_ud2a.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
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

int     op_ud2a(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_UD2A;
  return (new->len);
}
