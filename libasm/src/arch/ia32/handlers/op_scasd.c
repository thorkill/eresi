/*
** $Id: op_scasd.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_scasd" opcode="0xaf"/>
*/

int op_scasd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  if (asm_proc_opsize(proc))
    new->instr = ASM_SCASW;
  else
    new->instr = ASM_SCASD;
  new->ptr_instr = opcode;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.baser = ASM_REG_EAX;
  
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_YDEST, new);
#else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_YDEST;
  
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R8;
  new->op1.baser = ASM_REG_EAX;
  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.prefix = ASM_PREFIX_ES;
  new->op2.baser = ASM_REG_EDI;
  new->instr = ASM_SCASD;
#endif
  return (new->len);
}
