/*
** $Id: i386_rdtsc.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_rdtsc" opcode="0x31"/>
 */

int     i386_rdtsc(asm_instr *new, unsigned char *opcode, unsigned int len,
		   asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_RDTSC;
  return (new->len);
}
