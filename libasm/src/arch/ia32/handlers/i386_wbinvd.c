/*
** $Id: i386_wbinvd.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     i386_wbinvd(asm_instr *new, u_char *opcode, u_int len, 
		    asm_processor *proc)
{ 
  new->len += 1;
  new->instr = ASM_WBINVD;
  return (new->len);

}
