/*
** $Id: op_mov_al_ref_iv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_al_ref_iv" opcode="0xa0"/>
*/

int op_mov_al_ref_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_MOV;
    new->ptr_instr = opcode;

    #if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
    new->op1.content = ASM_OP_BASE;
    new->op1.base_reg = ASM_REG_AL;
    new->op1.regset = ASM_REGSET_R8;
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_OFFSET, proc);
#else
    new->len += 4;
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.content = ASM_OP_BASE;
    new->op1.len = 0;
    new->op1.ptr = 0;
    new->op1.base_reg = ASM_REG_AL;
    new->op1.regset = ASM_REGSET_R8;

    new->op2.type = ASM_OTYPE_OFFSET;
    new->op2.content = ASM_OP_VALUE | ASM_OP_REFERENCE;
    new->op2.len = 4;
    new->op2.ptr = opcode + 1;
    memcpy(&new->op2.imm, opcode + 1, 4);
#endif
    return (new->len);
}
