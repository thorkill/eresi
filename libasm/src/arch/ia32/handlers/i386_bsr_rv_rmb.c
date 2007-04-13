/*
** $Id: i386_bsr_rv_rmb.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_bsr_rv_rmb" opcode="0xbd"/>
 */

int i386_bsr_rv_rmb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_BSR;
    
    #if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL, proc);
    new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED, proc);
    #else
    new->op1.type = ASM_OTYPE_GENERAL;
    new->op2.type = ASM_OTYPE_ENCODED;
    
    operand_rv_rmb(new, opcode + 1, len - 1, proc);
    #endif
    return (new->len);
}
