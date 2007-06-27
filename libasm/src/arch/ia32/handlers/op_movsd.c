/*
** $Id: op_movsd.c,v 1.4 2007-06-27 11:25:11 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_movsd" opcode="0xa5"/>
*/

int op_movsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  if (asm_proc_opsize(proc))
    new->instr = ASM_MOVSW;
  else
    new->instr = ASM_MOVSD;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_XSRC, new);
#else
  new->op1.type = ASM_OTYPE_YDEST;
  new->op2.type = ASM_OTYPE_XSRC;
  
  new->op1.prefix = ASM_PREFIX_ES;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op1.baser = ASM_REG_EDI;
  new->op1.len = 0;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.len = 0;
  new->op2.prefix = ASM_PREFIX_DS;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.baser = ASM_REG_ESI;
#endif
  return (new->len);
}
