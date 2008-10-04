/**
 *
 * @file op_mov_rmb_ib.c
 * @ingroup handlers_ia32
 * $Id: op_mov_rmb_ib.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 <instruction func="op_mov_rmb_ib" opcode="0xc6"/>
*/

int op_mov_rmb_ib(asm_instr *new, unsigned char *opcode, unsigned int len,
                  asm_processor *proc)
{
  int olen;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;

#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,                                        ASM_OTYPE_ENCODEDBYTE, new, 0));
#else
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,                                        ASM_OTYPE_ENCODEDBYTE, new));
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen ,                                ASM_OTYPE_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen ,                                ASM_OTYPE_IMMEDIATEBYTE, new);
#endif

  return (new->len);
}
