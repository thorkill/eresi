/*
** $Id$
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
#if WIP
    new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new, 				
				  asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
						 asm_proc_is_protected(proc) ?
						 ASM_REGSET_R32 : ASM_REGSET_R16));

#else
    new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
#endif
    new->op[0].regset = ASM_REGSET_SREG;
    new->op[0].type = ASM_OP_BASE | ASM_OP_FIXED;
    new->op[0].len = 0;
    new->op[0].ptr = 0;
    new->op[0].baser = ASM_REG_FS;
#else
    new->op[0].content = ASM_CONTENT_FIXED;
    new->op[0].regset = ASM_REGSET_SREG;
    new->op[0].type = ASM_OP_BASE | ASM_OP_FIXED;
    new->op[0].len = 0;
    new->op[0].ptr = 0;
    new->op[0].baser = ASM_REG_FS;
#endif
  return (new->len);
}
