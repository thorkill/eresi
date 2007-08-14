/*
** $Id: op_and_eax_iv.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_eax_iv" opcode="0x25"/>
*/

int op_and_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) 
{
  new->instr = ASM_AND;
  new->len += 1;
  new->ptr_instr = opcode;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_ZF | ASM_FLAG_PF |
                      ASM_FLAG_OF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.regset = ASM_REGSET_R32;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATE, 
                                new);

  return (new->len);
}
