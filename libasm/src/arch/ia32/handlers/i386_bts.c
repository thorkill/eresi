/*
** $Id: i386_bts.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_bts" opcode="0xab"/>
 */

int i386_bts(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_BTS;
    #if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc);
    new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, proc);    
    #else
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op2.type = ASM_OTYPE_GENERAL;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    #endif
  return (new->len);
}
