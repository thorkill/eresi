/*
** $Id: op_cmp_al_ib.c,v 1.7 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_al_ib" opcode="0x3c"/>
*/

int op_cmp_al_ib(asm_instr *new, u_char *opcode, u_int len, 
                 asm_processor *proc) 
{
  new->instr = ASM_CMP;
  new->len += 1;

  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_PF | ASM_FLAG_AF |
                        ASM_FLAG_SF | ASM_FLAG_OF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.baser = ASM_REG_AL;
  new->len += asm_operand_fetch(&new->op2, opcode + 1,
                                ASM_OTYPE_IMMEDIATEBYTE, new);

  return (new->len);
}
