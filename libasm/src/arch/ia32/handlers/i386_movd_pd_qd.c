/*
** $Id: i386_movd_pd_qd.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
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
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_GENERAL, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1,
				ASM_OTYPE_ENCODED, new);
#else

  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_DWORD;
  new->op2.type = ASM_OTYPE_QMMX;
  new->op2.size = ASM_OSIZE_DWORD;
  
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;
  #endif
  return (new->len);
}
