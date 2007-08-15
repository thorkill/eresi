/*
** $Id: op_in_eax_dx.c,v 1.6 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_dx" opcode="0xed"/>
 */

int op_in_eax_dx(asm_instr *new, u_char *opcode, u_int len,
                 asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.baser = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, new);
  new->op2.regset = ASM_REGSET_R16;
  new->op2.baser = ASM_REG_DX;  
  new->op2.content = ASM_OP_BASE | ASM_OP_REFERENCE;

  return (new->len);
}
