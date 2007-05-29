/*
** $Id: i386_emms.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_emms" opcode="0x77"/>
 */

int     i386_emms(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOVQ;
  
  return (new->len);
}
