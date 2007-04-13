/*
** $Id: op_outsb.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_outsb" opcode="0x6e"/>
*/

int op_outsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_OUTSB;
  new->ptr_instr = opcode;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_EDX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_XSRC, proc);
  #else
  
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.regset = ASM_REGSET_R16;
  new->op1.base_reg = ASM_REG_EDX;
  
  new->op2.type = ASM_OTYPE_XSRC;
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.prefix = ASM_PREFIX_DS;
  new->op2.base_reg = ASM_REG_ESI;
  new->op2.regset = asm_proc_addsize(proc) ? ASM_REGSET_R16 :
    ASM_REGSET_R32;
#endif
  return (new->len);
}
