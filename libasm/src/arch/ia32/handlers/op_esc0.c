/*
** $Id: op_esc0.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc0" opcode="0xd8"/>
*/

int op_esc0(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_FADD;

    break;
  case 1:
    new->instr = ASM_FMUL;
    break;
  case 2:
    new->instr = ASM_FCOM;
    break;
  case 3:
    new->instr = ASM_FCOMP;
    break;
  case 4:
    new->instr = ASM_FSUB;
    break;
  case 5:
    new->instr = ASM_FSUBR;
    break;
  case 6:
    new->instr = ASM_FDIV;
    break;
  case 7:
    new->instr = ASM_FDIVR;
    break;
  }
  if (modrm->mod < 3){
    new->op1.type = ASM_OTYPE_FIXED;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
  } else {
    new->len += 1;
    switch(modrm->r) {
    case 6: case 0: case 5: case 1: case 4: case 7:

      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_BASE;
      new->op2.type = ASM_OTYPE_FIXED;
      new->op2.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op2.scale = modrm->m;
      break;
    
    case 2: case 3:
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.scale = modrm->m;
      break;
    }
  }
  return (new->len);
}
