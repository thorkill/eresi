/*
** Author  : <strauss at rfdslabs com br>
** Started : Wed Dec  17 00:23:23 2006
** Updated : Wed Dec  17 00:23:23 2006
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_punpcklbw_pq_qq" opcode="0x60"/>
 */

int	i386_punpcklbw_pq_qd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_PUNPCKLBW;

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_DWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;
  new->op2.regset = ASM_REGSET_MM;

  return (new->len);
}

/*
  <i386 func="i386_packuswb_pq_qq" opcode="0x67"/>
 */

int	i386_packuswb_pq_qq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_PACKUSWB;

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_QWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;
  new->op2.regset = ASM_REGSET_MM;

  return (new->len);
}

/*
  <i386 func="i386_punpckhbw_pq_qq" opcode="0x68"/>
 */

int	i386_punpckhbw_pq_qq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_PUNPCKHBW;

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_QWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;
  new->op2.regset = ASM_REGSET_MM;

  return (new->len);
}

/*
  <i386 func="i386_movq_pq_qq" opcode="0x6e"/>
 */

int	i386_movd_pd_qd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOVD;

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_DWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_DWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;

  return (new->len);
}

/*
  <i386 func="i386_movq_pq_qq" opcode="0x6f"/>
 */

int	i386_movq_pq_qq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOVQ;

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_QWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;

  return (new->len);
}

