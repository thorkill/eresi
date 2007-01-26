/*
** $Id: i386_xadd.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
<i386 func="" opcode="0xc1"/>
*/

int i386_xadd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  modrm = (struct s_modrm *) opcode;
  new->instr = ASM_XADD;
  new->len += 1;
  /*
  new->op1.regset = modrm->m;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_R32;
  
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.content = ASM_OP_IMM
  memcpy(&new->op2.imm, opcode + 1, 4);
  */
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_GENERAL;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
