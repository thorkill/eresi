/*
** $Id: i386_pop_fs.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0f 0xa1
  Instruction :         POP
*/

int i386_pop_fs(asm_instr *new, u_char *opcode, u_int len,
                asm_processor *proc) 
{
    new->instr = ASM_POP;
    new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
    new->op1.regset = ASM_REGSET_SREG;
    new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op1.len = 0;
    new->op1.ptr = 0;
    new->op1.base_reg = ASM_REG_FS;    
#else
    new->op1.type = ASM_OTYPE_FIXED;
    new->op1.regset = ASM_REGSET_SREG;
    new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
    new->op1.len = 0;
    new->op1.ptr = 0;
    new->op1.base_reg = ASM_REG_FS;
#endif
  return (new->len);
}
