/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Mon Feb 17 11:26:17 2003
** Updated : Wed May  7 01:34:50 2003
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode:	0xff 0x00
  <i386 func="op_group6" opcode="0x00"/>
*/

int	op_group6(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  new->len += 1;
  modrm = (struct s_modrm *) opcode + 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_SLDT;
    break;
  case 1:
    new->instr = ASM_STR;
    break;
  case 2:
    new->instr = ASM_LLDT;
    break;
  case 3:
    new->instr = ASM_LTR;
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    break;
  }
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len;
  new->op1.regset = ASM_REGSET_R16;
  return (new->len);
}

/*
  <i386 func="op_group7" opcode="0x01"/>
*/


int	op_group7(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm	*modrm;
  
  new->len += 1;
  modrm = (struct s_modrm *) opcode + 1;
  
    switch(modrm->r) {
    case 0:
      new->instr = ASM_SGDT;
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

/*
  <i386 func="op_ud2a" opcode="0x0b"/>
 */

int	op_ud2a(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_UD2A;
  return (new->len);
}





