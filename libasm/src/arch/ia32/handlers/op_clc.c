/*
** $Id: op_clc.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_clc" opcode="0xf8"/>
*/

int op_clc(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
    new->len += 1;
  new->ptr_instr = opcode;
    new->instr = ASM_CLC;
    #if LIBASM_USE_OPERAND_VECTOR
    #endif
  return (new->len);
}
