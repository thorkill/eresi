/*
** $Id: op_or_eax_iv.c,v 1.5 2007-06-27 11:25:12 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0d
  Instruction :         OR
  */

int op_or_eax_iv(asm_instr *new, u_char *opcode, u_int len,
                 asm_processor *proc) 
{
  new->instr = ASM_OR;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.baser = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_IMMEDIATE, new);
#else  
  new->op1.type = ASM_OP_FIXED;
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.baser = ASM_REG_EAX;
  
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = asm_proc_vector_size(proc);
  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  new->len += asm_proc_vector_len(proc);
#endif  
  
  return (new->len);
}
