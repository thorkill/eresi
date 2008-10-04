/*
** $Id: op_imul_gv_ev_ib.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_imul_gv_ev_ib" opcode="0x6b"/>
*/

int     op_imul_gv_ev_ib(asm_instr *new, unsigned char *opcode, unsigned int len,
			 asm_processor *proc)
{
  int	olen;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_IMUL;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, new);
#endif
#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[1], opcode + 1,                                        ASM_OTYPE_ENCODED, new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[1], opcode + 1,                                        ASM_OTYPE_ENCODED, new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[2], opcode + 1 + olen,                                ASM_OTYPE_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[2], opcode + 1 + olen,                                ASM_OTYPE_IMMEDIATEBYTE, new);
#endif

  return (new->len);
}
