/*
** $Id: op_pop_es.c,v 1.5 2007-06-27 11:25:12 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x07
  Instruction :         POP
*/

int op_pop_es(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  
  new->instr = ASM_POP;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = 4;
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.baser = ASM_REG_ES;
#else  
  new->ptr_instr = opcode;
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.baser = ASM_REG_ES;
#endif
  return (new->len);
}
