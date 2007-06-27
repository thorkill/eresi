/*
** $Id: i386_shrd_rmv_rv_cl.c,v 1.4 2007-06-27 11:25:11 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 <i386 func="i386_shrd_rmv_rv_cl" opcode="0xad"/>
*/

int i386_shrd_rmv_rv_cl(asm_instr *new, u_char *opcode, u_int len, 
			asm_processor *proc) 
{
    new->instr = ASM_SHRD;
    new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				  new);
    new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERAL, 
				  new);
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_FIXED, 
				  new);
    new->op3.content = ASM_OP_BASE;
    new->op3.regset = ASM_REGSET_R8;
    new->op3.ptr = opcode;
    new->op3.len = 0;
    new->op3.baser = ASM_REG_CL;
#else
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    new->op2.type = ASM_OTYPE_GENERAL;
    new->op2.size = ASM_OSIZE_VECTOR;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    new->op3.type = ASM_OTYPE_FIXED;
    new->op3.content = ASM_OP_BASE;
    new->op3.regset = ASM_REGSET_R8;
    new->op3.ptr = opcode;
    new->op3.len = 0;
    new->op3.baser = ASM_REG_CL;
#endif
    return (new->len);
}
