/*
** $Id: op_in_al_dx.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_al_dx" opcode="0xec"/>
 */

int     op_in_al_dx(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
  new->type = ASM_TYPE_LOAD;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);

  new->ptr_instr = opcode;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.baser = ASM_REG_AL;

  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, new);

  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R16;
  new->op2.baser = ASM_REG_DX;

  return (new->len);
}
