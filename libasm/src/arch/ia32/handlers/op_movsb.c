/*
** $Id: op_movsb.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_movsb" opcode="0xa4"/>
 */

int op_movsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_MOVSB;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_STORE | ASM_TYPE_ASSIGN;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_XSRC, new);

  return (new->len);
}
