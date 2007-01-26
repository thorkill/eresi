/*
** $Id: i386_btrl.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 <i386 func="i386_btrl" opcode="0xba"/>
*/

int     i386_btrl(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{ 
  new->len += 1;
  new->instr = ASM_BTRL;
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_ib(new, opcode + 1, len - 1, proc);
  return (new->len);
}
