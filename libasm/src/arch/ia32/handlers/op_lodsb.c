/**
 * @file op_lodsb.c
 * $Id: op_lodsb.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
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

#if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_YDEST, proc);
    new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_FIXED, proc);
    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.regset = asm_proc_opsize(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_EAX;
#else
    
    new->op1.type = ASM_OTYPE_YDEST;
    new->op2.type = ASM_OTYPE_XSRC;

    new->op1.regset = ASM_REGSET_R32;
    new->op1.prefix = ASM_PREFIX_ES;
    new->op1.base_reg = ASM_REG_EDI;

    new->op2.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op2.regset = asm_proc_opsize(proc) ?
      ASM_REGSET_R16 : ASM_REGSET_R32;
    new->op2.base_reg = ASM_REG_EAX;
#endif
  return (new->len);
}
