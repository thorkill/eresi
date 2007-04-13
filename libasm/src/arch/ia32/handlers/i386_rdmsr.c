/*
** $Id: i386_rdmsr.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
   <i386 func="i386_rdmsr" opcode="0x32"/>
 */

int     i386_rdmsr(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{ 
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_RDMSR;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);
}
