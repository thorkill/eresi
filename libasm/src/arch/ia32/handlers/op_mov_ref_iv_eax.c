/**
 * @file op_mov_ref_iv_eax.c
 * $Id: op_mov_ref_iv_eax.c,v 1.5 2007-05-29 00:40:27 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_ref_iv_eax" opcode="0xa3"/>
*/

int op_mov_ref_iv_eax(asm_instr *new, u_char *opcode, u_int len, 
		      asm_processor *proc) 
{
  new->instr = ASM_MOV;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->ptr_instr = opcode;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_OFFSET, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, 
				new);
#else
  new->op1.type = ASM_OTYPE_OFFSET;
  new->op2.type = ASM_OTYPE_FIXED;
    
  new->op1.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  memcpy(&new->op1.imm, opcode + 1, 4);

  new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op2.len = 0;
  new->op2.base_reg = ASM_REG_EAX;
  new->op2.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->len += 4;
#endif
  return (new->len);
}
