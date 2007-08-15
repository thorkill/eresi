/**
 * @file op_test_rmv_rv.c
 * @ingroup handlers_ia32
** $Id: op_test_rmv_rv.c,v 1.5 2007-08-15 21:30:21 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_test_rmv_rv" opcode="0x85"/>
*/

int op_test_rmv_rv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_TEST;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF | ASM_FLAG_PF |
                      ASM_FLAG_SF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                ASM_OTYPE_ENCODED, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1,
                                ASM_OTYPE_GENERAL, new);

  return (new->len);
}
