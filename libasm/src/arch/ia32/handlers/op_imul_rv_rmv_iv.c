/*
** $Id: op_imul_rv_rmv_iv.c,v 1.5 2007-08-20 07:21:04 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_imul_rv_rmv_iv" opcode="0x69"/>
 */

int     op_imul_rv_rmv_iv(asm_instr *new, u_char *opcode, u_int len, 
			  asm_processor *proc) 
{
  int		olen;
  new->instr = ASM_IMUL;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;
  
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, 
				                        new);
  new->len += (olen = asm_operand_fetch(&new->op2, opcode + 1, 
					                              ASM_OTYPE_ENCODED, new));
  new->len += asm_operand_fetch(&new->op3, opcode + 1 + olen, 
				                       ASM_OTYPE_IMMEDIATE, new);

  return (new->len);
}
