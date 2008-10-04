/*
** $Id: op_bound_gv_ma.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_bound_gv_ma" opcode="0x62"/>
*/

int     op_bound_gv_ma(asm_instr *new, unsigned char *opcode, unsigned int len,
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
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_MEMORY,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_MEMORY,				new);
#endif
#else

  modrm = (struct s_modrm *) (opcode + 1);
  new->op[0].type = ASM_OTYPE_GENERAL;
  new->op[1].type = ASM_OTYPE_MEMORY;
  operand_rmv(&new->op[0], opcode + 1, len - 1, proc);

  new->op[1].content = ASM_OP_BASE;
  new->op[1].regset = ASM_REGSET_R32;
  new->op[1].baser = modrm->r;
  new->len += new->op[0].len;
#endif
  return (new->len);
}
