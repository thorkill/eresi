/*
** $Id: op_popa.c,v 1.4 2007-08-20 07:21:04 strauss Exp $
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
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_LOAD;
  return (new->len);
}
