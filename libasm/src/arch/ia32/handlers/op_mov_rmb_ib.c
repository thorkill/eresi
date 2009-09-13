/**
 *
* @file libasm/src/arch/ia32/handlers/op_mov_rmb_ib.c
 *
 * @ingroup IA32_instrs
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 <instruction func="op_mov_rmb_ib" opcode="0xc6"/>
*/

int op_mov_rmb_ib(asm_instr *new, u_char *opcode, u_int len,
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
