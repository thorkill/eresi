/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movzbl_rv_rmb" opcode="0xb6"/>
*/

int i386_movzbl_rv_rmb(asm_instr *new, u_char *opcode, u_int len,
		       asm_processor *proc)
{
  new->len += 1;
    if (asm_proc_opsize(proc))
      new->instr = ASM_MOVZBW;
    else
      new->instr = ASM_MOVZBL;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new);
#else
  new->op[0].content = ASM_CONTENT_GENERAL;
  new->op[0].size = ASM_OSIZE_VECTOR;
  new->op[1].content = ASM_CONTENT_ENCODED;
  new->op[1].size = ASM_OSIZE_BYTE;
  operand_rv_rmb(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
