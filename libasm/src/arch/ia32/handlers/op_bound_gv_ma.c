/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_bound_gv_ma" opcode="0x62"/>
*/

int     op_bound_gv_ma(asm_instr *new, u_char *opcode, u_int len,
		       asm_processor *proc)
{
#if !LIBASM_USE_OPERAND_VECTOR
  struct s_modrm        *modrm;
#endif

  new->instr = ASM_BOUND;
  new->len += 1;
  new->ptr_instr = opcode;

  #if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_MEMORY,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_MEMORY,				new);
#endif
#else

  modrm = (struct s_modrm *) (opcode + 1);
  new->op[0].content = ASM_CONTENT_GENERAL;
  new->op[1].content = ASM_CONTENT_MEMORY;
  operand_rmv(&new->op[0], opcode + 1, len - 1, proc);

  new->op[1].type = ASM_OPTYPE_REG;
  new->op[1].regset = ASM_REGSET_R32;
  new->op[1].baser = modrm->r;
  new->len += new->op[0].len;
#endif
  return (new->len);
}
