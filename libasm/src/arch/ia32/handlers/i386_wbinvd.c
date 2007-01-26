/*
** $Id: i386_wbinvd.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     i386_wbinvd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{ 
  new->len += 1;
  new->instr = ASM_WBINVD;
  return (new->len);

}
