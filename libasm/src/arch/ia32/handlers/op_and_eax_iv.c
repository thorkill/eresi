/*
** $Id: op_and_eax_iv.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_eax_iv" opcode="0x25"/>
*/

int op_and_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) 
{
  new->instr = ASM_AND;
  new->type = ASM_TYPE_ARITH;
  new->len += 1;
  new->ptr_instr = opcode;
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATE, 
				new);
#else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_EAX;
  new->op1.regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;

  new->len +=  asm_proc_vector_len(proc);
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
#endif
  return (new->len);
}
