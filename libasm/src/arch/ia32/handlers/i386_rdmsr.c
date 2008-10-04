/*
** $Id: i386_rdmsr.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
   <i386 func="i386_rdmsr" opcode="0x32"/>
 */

int     i386_rdmsr(asm_instr *new, unsigned char *opcode, unsigned int len,
		   asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_RDMSR;
  return (new->len);
}
