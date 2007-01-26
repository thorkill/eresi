/*
** $Id: op_esc6.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc6" opcode="0xde"/>
 */

int op_esc6(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  if (modrm->mod <= 2) {
    switch(modrm->r) {
      case 0:
        new->instr = ASM_FIADD;
        break;
      case 1:
        new->instr = ASM_FIMUL;
        break;
      case 2:
        new->instr = ASM_FICOM;
        break;
      case 3:
        new->instr = ASM_FICOMP;
        break;
      case 4:
        new->instr = ASM_FISUB;
        break;
      case 5:
        new->instr = ASM_FISUBR;
        break;
      case 6:
        new->instr = ASM_FIDIV;
        
        break;
      case 7:
        new->instr = ASM_FIDIVR;
        break;
      } 
    } else {
      switch(modrm->r) {
      case 0: new->instr = ASM_FADDP; break;
      case 1: 
        new->instr = ASM_FMULP;
        break;
      case 2: 
        new->instr = ASM_FCOMPS;
        break;
      case 3: new->instr = ASM_FCOMPP;
        break;
      case 4: new->instr = ASM_FSUBP;
        break;
      case 5: new->instr = ASM_FSUBRP;
        break;
      case 6: 
        switch(modrm->m) {
        case 3: case 5: case 2: case 4: case 6: case 1: new->instr = ASM_FDIVP; break;
        default: case 0: new->instr = ASM_FDIVP; break;
        }
        break;
      case 7: 
        switch(modrm->m) {
        case 3: new->instr = ASM_FDIVR; break;
        default: new->instr = ASM_FDIV; break;
        }       
        break;
      }
    }
      
    if (!(*(opcode + 1) == 0xd9)) {
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op1.len = 1;
      new->op1.scale = modrm->m;
      new->op2.type = ASM_OTYPE_FIXED;
      new->op2.content = ASM_OP_FPU | ASM_OP_BASE;
      new->op2.len = 0;
    } else
      new->len++;
    if (new->op1.type)
      new->len += new->op1.len;
  return (new->len);
}
