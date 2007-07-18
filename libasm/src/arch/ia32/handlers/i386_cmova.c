/*
** $Id: i386_cmova.c,v 1.5 2007-07-18 15:47:10 strauss Exp $
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

  new->type = ASM_TYPE_ASSIGN | ASM_TYPE_COMPARISON;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, 
				new);    

  return (new->len);
}
