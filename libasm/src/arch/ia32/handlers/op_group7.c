/*
** $Id: op_group7.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     op_group7(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{ 
  struct s_modrm        *modrm;
  
  new->len += 1;
  modrm = (struct s_modrm *) opcode + 1;

    switch(modrm->r) {
    case 0:
      new->instr = ASM_SGDT;
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
      break;
    case 1:
      new->instr = ASM_SIDT;
      break;
    case 2:
      new->instr = ASM_LGDT;
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
      break;
    case 3:
      new->instr = ASM_LIDT;
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
      break;
    case 4:
      break;
    case 5:
      new->instr = ASM_BAD;
      break;
    case 6:
      
      break;
    case 7:

      break;
    }
    return (new->len);
}
