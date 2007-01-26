/*
** $Id: op_mov_rm_segr.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rm_segr" opcode="0x8c"/>
*/

int op_mov_rm_segr(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOV;
  
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  
  new->op2.type = ASM_OTYPE_SEGMENT;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_SREG;
  new->op2.base_reg = modrm->r;
  
  return (new->len);
}
