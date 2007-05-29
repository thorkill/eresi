/*
** $Id: i386_rdmsr.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
   <i386 func="i386_rdmsr" opcode="0x32"/>
 */

int     i386_rdmsr(asm_instr *new, u_char *opcode, u_int len, 
		   asm_processor *proc)
{ 
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_RDMSR;
  return (new->len);
}
