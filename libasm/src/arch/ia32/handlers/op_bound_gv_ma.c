/*
** $Id: op_bound_gv_ma.c,v 1.5 2007-06-27 11:25:11 heroine Exp $
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
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_MEMORY, 
				new);
#else
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op2.type = ASM_OTYPE_MEMORY;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.baser = modrm->r;
  new->len += new->op1.len;
#endif
  return (new->len);
}
