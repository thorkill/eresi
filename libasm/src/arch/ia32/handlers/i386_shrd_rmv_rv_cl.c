/*
** $Id$
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
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED, new);
    new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_GENERAL, new);
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_FIXED, new);
    new->op[2].type = ASM_OPTYPE_REG;
    new->op[2].regset = ASM_REGSET_R8;
    new->op[2].ptr = opcode;
    new->op[2].len = 0;
    new->op[2].baser = ASM_REG_CL;
#else
    new->op[0].content = ASM_CONTENT_ENCODED;
    new->op[0].size = ASM_OSIZE_VECTOR;
    new->op[1].content = ASM_CONTENT_GENERAL;
    new->op[1].size = ASM_OSIZE_VECTOR;
    operand_rmv_rv(new, opcode + 1, len - 1, proc);
    new->op[2].content = ASM_CONTENT_FIXED;
    new->op[2].type = ASM_OPTYPE_REG;
    new->op[2].regset = ASM_REGSET_R8;
    new->op[2].ptr = opcode;
    new->op[2].len = 0;
    new->op[2].baser = ASM_REG_CL;
#endif
    return (new->len);
}
