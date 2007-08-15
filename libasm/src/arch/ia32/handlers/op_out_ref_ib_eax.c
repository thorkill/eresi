/**
 * @file op_out_ref_ib_eax.c
 * @ingroup handlers_ia32
 * $Id: op_out_ref_ib_eax.c,v 1.5 2007-08-15 21:30:20 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_ref_ib_eax" opcode="0xe7"/>
*/

int op_out_ref_ib_eax(asm_instr *new, u_char *opcode, u_int len, 
                      asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->instr = ASM_OUT;
  new->len += 1;
  new->type = ASM_TYPE_IO | ASM_TYPE_STORE;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
                                ASM_OTYPE_IMMEDIATEBYTE, new);
  new->len += asm_operand_fetch(&new->op2, opcode, 
                                ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.baser = ASM_REG_EAX;

  return (new->len);
}
