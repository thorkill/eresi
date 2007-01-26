/*
** $Id: op_bound_gv_ma.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_bound_gv_ma" opcode="0x62"/>
*/

int     op_bound_gv_ma(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  new->instr = ASM_BOUND;
  new->len += 1;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_MEMORY;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.base_reg = modrm->r;
  new->len += new->op1.len;
  return (new->len);
}
