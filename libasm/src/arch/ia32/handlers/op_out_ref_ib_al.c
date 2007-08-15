/*
** $Id: op_out_ref_ib_al.c,v 1.5 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_ref_ib_al" opcode="0xe6"/>
*/


int op_out_ref_ib_al(asm_instr *new, u_char *opcode, u_int len, 
                     asm_processor *proc)
{
  new->instr = ASM_OUT;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_IO | ASM_TYPE_STORE;

  new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                ASM_OTYPE_IMMEDIATEBYTE, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.baser = ASM_REG_AL;

  return (new->len);
}
