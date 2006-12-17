/*
** Author  : <strauss at rfdslabs com br>
** Started : Wed Dec  17 01:40:00 2006
** Updated : Wed Dec  17 01:40:00 2006
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_por_pq_qq" opcode="0xeb"/>
 */

int	i386_por_pq_qq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_POR;

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
  <i386 func="i386_pxor_pq_qq" opcode="0xef"/>
 */

int	i386_pxor_pq_qq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_PXOR;

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_QWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;
  new->op2.regset = ASM_REGSET_MM;

  return (new->len);
}

