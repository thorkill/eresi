/*
** $Id: op_mov_segr_rm.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_segr_rm" opcode="0x8e"/>
*/

int op_mov_segr_rm(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_SEGMENT, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, proc);
  #else
  new->op1.type = ASM_OTYPE_SEGMENT;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_SREG;
  new->op1.base_reg = modrm->r;
  
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv(&new->op2, opcode + 1, len - 1, proc);
  new->len += new->op2.len;
  //new->op2.content = ASM_OP_BASE;
  //new->op2.regset = ASM_REGSET_R32;
  //new->op2.base_reg = modrm->m;
  #endif
  return (new->len);
}
