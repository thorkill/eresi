/*
** $Id: op_push_ss.c,v 1.7 2007-08-20 07:21:04 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_ss" opcode="0x16"/>
*/

int     op_push_ss(asm_instr *new, u_char *opcode, u_int len, 
                   asm_processor *proc)
{
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.baser = ASM_REG_SS;

  return (new->len);
}
