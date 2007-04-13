/*
** $Id: i386_btrl.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
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
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATEBYTE, proc);
  #else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  operand_rmv_ib(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}
