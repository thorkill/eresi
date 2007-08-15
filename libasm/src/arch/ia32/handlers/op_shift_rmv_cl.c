/*
** $Id: op_shift_rmv_cl.c,v 1.5 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmv_cl" opcode="0xd3"/>
*/

int op_shift_rmv_cl(asm_instr *new, u_char *opcode, u_int len, 
                    asm_processor *proc)
{
  struct s_modrm        *modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF;

  switch(modrm->r) {
  case 0:
    new->instr = ASM_ROL;
    break;
  case 5:
    new->instr = ASM_SHR;
    new->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
    break;
  case 4:
    new->instr = ASM_SHL;
    new->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
    break;
  case 7:
    new->instr = ASM_SAR;
    new->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
    break;
  default:
    new->instr = ASM_BAD;
  }

  new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                ASM_OTYPE_ENCODED, new);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.baser = ASM_REG_CL;
  new->op2.len = 0;
  new->op2.ptr = 0;

  return (new->len);
}
