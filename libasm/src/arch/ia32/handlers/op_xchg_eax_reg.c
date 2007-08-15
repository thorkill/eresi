/**
 * @file op_xchg_eax_reg.c
 * @ingroup handlers_ia32
** $Id: op_xchg_eax_reg.c,v 1.5 2007-08-15 21:30:21 strauss Exp $
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

int op_xchg_eax_reg(asm_instr *new, u_char *opcode, u_int len, 
                    asm_processor *proc)
{
  new->instr = ASM_XCHG;
  new->ptr_instr = opcode;
  new->len += 1;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_FIXED, 
                                new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.len = 0;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_OPMOD, new);

  return (new->len);
}
