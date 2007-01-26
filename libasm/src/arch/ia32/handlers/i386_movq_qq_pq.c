/*
** $Id: i386_movq_qq_pq.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movq_qq_pq" opcode="0x7f"/>
 */

int     i386_movq_qq_pq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOVQ;
  
  new->op1.type = ASM_OTYPE_QMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_PMMX;
  new->op2.size = ASM_OSIZE_QWORD;
  
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  new->op2.regset = ASM_REGSET_MM;
  
  return (new->len);
}
