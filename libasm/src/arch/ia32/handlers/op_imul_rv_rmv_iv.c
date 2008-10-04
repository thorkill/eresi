/**
 * @file op_imul_rv_rmv_iv.c
 * @brief Handler for instruction op_imul_rv_rmv_iv opcode 0x69
 * @ingroup handlers_ia32
** $Id: op_imul_rv_rmv_iv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction op_imul_rv_rmv_iv opcode 0x69
 *
 *
 */

int     op_imul_rv_rmv_iv(asm_instr *new, unsigned char *opcode, unsigned int len,
			  asm_processor *proc)
{
  int		olen;
  new->instr = ASM_IMUL;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;


#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, new);
#endif
#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED, new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED, new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[2], opcode + 1 + olen, ASM_OTYPE_IMMEDIATE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[2], opcode + 1 + olen, ASM_OTYPE_IMMEDIATE, new);
#endif
  return (new->len);
}
