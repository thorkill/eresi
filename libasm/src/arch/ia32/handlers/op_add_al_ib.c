/*
** $Id: op_add_al_ib.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x04
  Instruction :         ADD
*/

int op_add_al_ib(asm_instr *new, u_char *opcode, u_int len,
                      asm_processor *proc) {
  new->instr = ASM_ADD;
  new->type = ASM_TYPE_ARITH;
  new->ptr_instr = opcode;
  new->len += 1;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_AL;
  new->op1.regset = ASM_REGSET_R8;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_IMMEDIATEBYTE, new);
  #else

  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
  
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.base_reg = ASM_REG_AL;
  new->op1.regset = ASM_REGSET_R8;
  
  new->op2.content = ASM_OP_VALUE;
  new->len += 1;
  new->op2.ptr = opcode + 1;
  new->op2.len = 1;
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, 1);
  #endif
  return (new->len);
}
