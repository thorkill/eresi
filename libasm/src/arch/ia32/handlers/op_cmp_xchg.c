/*
** $Id: op_cmp_xchg.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="op_cmp_xchg" opcode="0xb1"/>
*/

int     op_cmp_xchg(asm_instr *new, u_char *opcode, u_int len, 
		    asm_processor *proc)
{
  new->len += 1;
  new->type = ASM_TYPE_TEST | ASM_TYPE_CONTROL;
  new->instr = ASM_CMPXCHG;
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, 
				new);
#else
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_ENCODED;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
