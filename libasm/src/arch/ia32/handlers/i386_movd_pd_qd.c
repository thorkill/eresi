/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movq_pq_qq" opcode="0x6e"/>
 */

int     i386_movd_pd_qd(asm_instr *new, u_char *opcode, u_int len,
			asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOVD;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED, new);
#else
  new->op[0].content = ASM_CONTENT_PMMX;
  new->op[0].size = ASM_OSIZE_DWORD;
  new->op[1].content = ASM_CONTENT_QMMX;
  new->op[1].size = ASM_OSIZE_DWORD;

  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op[0].regset = ASM_REGSET_MM;
#endif
  return (new->len);
}
