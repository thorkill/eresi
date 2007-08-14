/**
 * @file op_lodsb.c
 * @ingroup handlers_ia32
 * $Id: op_lodsb.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction func="op_lodsb" opcode="0xac"/>
 */

int op_lodsb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_LODSB;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op2.regset = asm_proc_opsize(proc) ?
                      ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op2.baser = ASM_REG_EAX;

  return (new->len);
}
