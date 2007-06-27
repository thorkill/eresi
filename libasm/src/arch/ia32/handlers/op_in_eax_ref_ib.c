/*
** $Id: op_in_eax_ref_ib.c,v 1.4 2007-06-27 11:25:11 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_ref_ib" opcode="0xe5"/>
*/

int op_in_eax_ref_ib(asm_instr *new, u_char *opcode, u_int len, 
		     asm_processor *proc) 
{
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->len += 1;
  
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_IMMEDIATEBYTE, new);
  new->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE;

  #else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.content = ASM_OP_REFERENCE | ASM_OP_VALUE;
  #endif
  return (new->len);
}
