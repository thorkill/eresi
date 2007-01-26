/*
** $Id: i386_movswl_rv_rm2.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movswl_rv_rm2" opcode="0xbf"/>
*/


int i386_movswl_rv_rm2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  if (asm_proc_opsize(proc))
    new->instr = ASM_MOVSBW;
  else
    new->instr = ASM_MOVSWL;
  new->len += 1;
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rm2(new, opcode + 1, len - 1, proc);
  return (new->len);
}
