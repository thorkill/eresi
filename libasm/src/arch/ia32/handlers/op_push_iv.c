/**
 * @file op_push_iv.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * Handler for opcode 0x68 , instruction push_iv
  <instruction func="op_push_iv" opcode="0x68"/>
*/

int op_push_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_IMMEDIATE, new);

  return (new->len);
}
