/*
** $Id: i386_rdtsc.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_rdtsc" opcode="0x31"/>
 */

int     i386_rdtsc(asm_instr *new, u_char *opcode, u_int len, 
		   asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_RDTSC;
  return (new->len);
}
