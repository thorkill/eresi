/*
** $Id: op_popa.c,v 1.3 2007-05-11 16:40:58 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the popa instruction. Opcode = 0x61
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of buffer to disassemble.
 * @param proc Pointer to processor structure.
*/

int op_popa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_POPA;
  new->spdiff = 8 * 4;
  new->type = ASM_TYPE_TOUCHSP;
  return (new->len);
}
