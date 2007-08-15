/**
 * @file op_xchg_rmb_rb.c
 * @ingroup handlers_ia32
** $Id: op_xchg_rmb_rb.c,v 1.5 2007-08-15 21:30:21 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_xchg_rmb_rb" opcode="0x86"/>
*/

int op_xchg_rmb_rb(asm_instr *new, u_char *opcode, u_int len, 
                   asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XCHG;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_STORE;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERALBYTE, 
                                new);

  return (new->len);
}
