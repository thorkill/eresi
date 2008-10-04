/*
** $Id: op_popa.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
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

int op_popa(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_POPA;
  new->spdiff = 8 * 4;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_LOAD;
  return (new->len);
}
