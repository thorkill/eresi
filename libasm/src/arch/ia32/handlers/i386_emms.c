/*
** $Id: i386_emms.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_emms" opcode="0x77"/>
 */

int     i386_emms(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOVQ;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  
  return (new->len);
}
