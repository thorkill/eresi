/*
** $Id: op_xchg_eax_reg.c,v 1.2 2007-04-13 06:56:35 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_xchg_eax_reg" opcode="0x91"/>
  <instruction func="op_xchg_eax_reg" opcode="0x92"/>
  <instruction func="op_xchg_eax_reg" opcode="0x93"/>
  <instruction func="op_xchg_eax_reg" opcode="0x94"/>
  <instruction func="op_xchg_eax_reg" opcode="0x95"/>
  <instruction func="op_xchg_eax_reg" opcode="0x96"/>
  <instruction func="op_xchg_eax_reg" opcode="0x97"/>
*/

int op_xchg_eax_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  new->instr = ASM_XCHG;
  new->ptr_instr = opcode;
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_FIXED, proc);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_OPMOD, proc);
#else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;

  new->op2.type = ASM_OTYPE_OPMOD;
  new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op2.len = 0;
  new->op2.base_reg = modrm->m;
  new->op2.regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;
#endif
  return (new->len);
}
