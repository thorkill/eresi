/*
** $Id: i386_mov_cr_rm.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int i386_mov_cr_rm(asm_instr *new, u_char *opcode, u_int len, 
		   asm_processor *proc)
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 1;
  new->instr = ASM_MOV;
    
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_CONTROL, 
				new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_REGISTER, 
				new);
#else    
  new->op1.type = ASM_OTYPE_CONTROL;
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = ASM_REGSET_CREG;
  new->op1.base_reg = modrm->r;
    
  new->op2.type = ASM_OTYPE_REGISTER;
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R32;
  new->op2.base_reg = modrm->m;
#endif
  return (new->len);
}
