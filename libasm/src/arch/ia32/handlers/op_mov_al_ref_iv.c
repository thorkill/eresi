/*
** $Id: op_mov_al_ref_iv.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_al_ref_iv" opcode="0xa0"/>
*/

int op_mov_al_ref_iv(asm_instr *new, u_char *opcode, u_int len, 
                     asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ASSIGN;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE;
  new->op1.baser = ASM_REG_AL;
  new->op1.regset = ASM_REGSET_R8;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_OFFSET, new);

  return (new->len);
}
