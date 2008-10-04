/**
 * @file op_leave.c
 * @ingroup handlers_ia32
 * $Id: op_leave.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the leave instruction opcode 0xc9
 * @param new Pointer to the instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
*/

int op_leave(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = 4;
  new->instr = ASM_LEAVE;
  return (new->len);
}
