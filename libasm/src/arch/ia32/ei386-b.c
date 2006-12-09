/*
** private - do not distribute
** 
** Author  : <sk at devhell dot org>
** Started : Wed Jul 24 19:52:01 2002
** Updated : Tue May  6 22:10:25 2003
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_cmp_xchg" opcode="0xb1"/>
*/

int	op_cmp_xchg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_CMPXCHG;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <i386 func="op_lss_rv_rmv" opcode="0xb2"/>
*/

int	op_lss_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_LSS;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}


/*
  <i386 func="op_btr_rmv_rv" opcode="0xb3"/>
*/

int	op_btr_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BTR;
  
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);

  return (new->len);
}

/*
  <i386 func="i386_movzbl_rv_rmb" opcode="0xb6"/>
*/

int i386_movzbl_rv_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
    if (asm_proc_oplen(proc))
      new->instr = ASM_MOVZBW;
    else
      new->instr = ASM_MOVZBL;
    new->op1.type = ASM_OTYPE_GENERAL;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_ENCODED;
    new->op2.size = ASM_OSIZE_BYTE;
    operand_rv_rmb(new, opcode + 1, len - 1, proc);
    return (new->len);
}

/*
  <i386 func="i386_movzwl_rv_rm2" opcode="0xb7"/>
*/

int i386_movzwl_rv_rm2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_MOVZWL;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rm2(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
 <i386 func="i386_bsf" opcode"0xbc"/>
*/

int i386_bsf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_BSF;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rm2(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <i386 func="i386_bsr_rv_rmb" opcode="0xbd"/>
 */

int i386_bsr_rv_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_BSR;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
    operand_rv_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <i386 func="i386_movsbl_rv_rmb" opcode="0xbe"/>
*/

int i386_movsbl_rv_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

  if (asm_proc_oplen(proc))
    new->instr = ASM_MOVSBW;
  else
    new->instr = ASM_MOVSBL;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmb(new, opcode + 1, len - 1, proc);
  return (new->len);
}

/*
  <i386 func="i386_movswl_rv_rm2" opcode="0xbf"/>
*/


int i386_movswl_rv_rm2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (asm_proc_oplen(proc))
    new->instr = ASM_MOVSBW;
  else
    new->instr = ASM_MOVSWL;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rm2(new, opcode + 1, len - 1, proc);
  return (new->len);
}

