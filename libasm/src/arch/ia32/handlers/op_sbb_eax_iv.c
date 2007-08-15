/*
** $Id: op_sbb_eax_iv.c,v 1.5 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_eax_iv" opcode="0x1d"/>
*/

int op_sbb_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SBB;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG | ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.size = new->op2.size = ASM_OSIZE_VECTOR;
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ? ASM_REGSET_R32 :
    ASM_REGSET_R16;
  new->len += asm_operand_fetch(&new->op2, opcode + 1,
                                ASM_OTYPE_IMMEDIATE, new);

  return (new->len);
}
