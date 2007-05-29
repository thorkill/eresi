/*
** $Id: op_outsw.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_outsw" opcode="0x6f"/>
  <instruction func="op_outsd" opcode="0x6f"/>
*/

int op_outsw(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  
  if (!asm_proc_opsize(proc))
    new->instr = ASM_OUTSW;
  else
    new->instr = ASM_OUTSD;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_XSRC, new);
  #else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_DX;
  
  new->op2.type = ASM_OTYPE_XSRC;
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.base_reg = ASM_REG_ESI;
  new->op2.regset = asm_proc_addsize(proc) ? ASM_REGSET_R16 :
    ASM_REGSET_R32;
#endif
  return (new->len);
}
