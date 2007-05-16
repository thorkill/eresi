/*
** $Id: i386_cmova.c,v 1.3 2007-05-16 18:38:13 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_cmova" opcode="0x47"/>
 */

int     i386_cmova(asm_instr *new, u_char *opcode, u_int len, 
		   asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_CMOVA;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
				proc);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, 
				proc);    
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
