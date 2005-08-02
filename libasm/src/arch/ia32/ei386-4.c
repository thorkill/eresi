/*
** 
** i386_4x.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Sat Dec 28 03:37:02 2002
** Updated : Wed Apr 30 18:49:22 2003
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_cmovae" opcode="0x43"/>
*/


int i386_cmovae(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_CMOVAE;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <i386 func="i386_cmovne" opcode="0x45"/>
*/


int i386_cmovne(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_CMOVNE;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <i386 func="op_cmova" opcode="0x47"/>
 */

int	op_cmova(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_CMOVA;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
