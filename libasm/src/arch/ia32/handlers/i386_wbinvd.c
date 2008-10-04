/*
** $Id: i386_wbinvd.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     i386_wbinvd(asm_instr *new, unsigned char *opcode, unsigned int len,
		    asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_WBINVD;
  return (new->len);

}
