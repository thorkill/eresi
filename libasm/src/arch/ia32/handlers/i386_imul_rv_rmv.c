/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_imul_rv_rmv" opcode="0xaf"/>
*/

int i386_imul_rv_rmv(asm_instr *new, u_char *opcode, u_int len,
		     asm_processor *proc)
{
    new->instr = ASM_IMUL;
    new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL,				  new, 0);
#else
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL,				  new);
#endif
#if WIP
    new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED,				  new, 0);
#else
    new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED,				  new);
#endif
#else
    new->op[0].content = ASM_CONTENT_GENERAL;
    new->op[0].size = ASM_OSIZE_VECTOR;
    new->op[1].content = ASM_CONTENT_ENCODED;
    new->op[1].size = ASM_OSIZE_VECTOR;
    operand_rv_rmv(new, opcode + 1, len - 1, proc);
#endif
    return (new->len);
}
