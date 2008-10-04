/**
 * @file op_push_iv.c
 * @ingroup handlers_ia32
 * $Id: op_push_iv.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * Handler for opcode 0x68 , instruction push_iv
  <instruction func="op_push_iv" opcode="0x68"/>
*/

int op_push_iv(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_IMMEDIATE,
                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_IMMEDIATE,
                                new);
#endif

  return (new->len);
}
