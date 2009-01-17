/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0c
  Instruction :         OR
*/

int op_or_al_ib(asm_instr *new, u_char *opcode, u_int len,
                asm_processor *proc)
{
  new->instr = ASM_OR;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       ASM_REGSET_R8));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
#endif
  new->op[0].type = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].ptr = opcode;
  new->op[0].baser = ASM_REG_AL;
  new->op[0].regset = ASM_REGSET_R8;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_IMMEDIATEBYTE, new);
#endif

  return (new->len);
}
