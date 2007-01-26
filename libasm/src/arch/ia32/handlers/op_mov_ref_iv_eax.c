/*
** $Id: op_mov_ref_iv_eax.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_ref_iv_eax" opcode="0xa3"/>
*/

int op_mov_ref_iv_eax(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

    new->instr = ASM_MOV;
    new->len += 1;
    new->ptr_instr = opcode;

    new->op1.type = ASM_OTYPE_OFFSET;
    new->op2.type = ASM_OTYPE_FIXED;

    new->op1.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
    new->op1.ptr = opcode + 1;
    new->op1.len = 4;
    memcpy(&new->op1.imm, opcode + 1, 4);

    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.len = 0;
    new->op2.base_reg = ASM_REG_EAX;
    new->op2.regset = asm_proc_opsize(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;
    new->len += 4;
  return (new->len);
}
