/*
** $Id: op_cli.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cli" opcode="0xfa"/>
*/

int op_cli(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLI;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
