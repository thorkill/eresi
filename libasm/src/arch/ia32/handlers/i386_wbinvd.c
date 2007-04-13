/*
** $Id: i386_wbinvd.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     i386_wbinvd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{ 
  new->len += 1;
  new->instr = ASM_WBINVD;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);

}
