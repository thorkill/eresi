/*
** $Id$
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0f 0xa1
  Instruction :         POP
*/

int i386_pop_fs(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_POP;
  new->len += 1;
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].type = ASM_OP_BASE;
  new->op[0].len = 0;
  new->op[0].ptr = 0;
  new->op[0].baser = ASM_REG_FS;
  return (new->len);
}
