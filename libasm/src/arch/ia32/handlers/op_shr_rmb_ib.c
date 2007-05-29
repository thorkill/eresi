/*
** $Id: op_shr_rmb_ib.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shr_rmb_ib" opcode="0xc0"/>
*/

int op_shr_rmb_ib(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) {
  struct s_modrm        *modrm;
  int			olen;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->r == ASM_REG_EAX)
    new->instr = ASM_ROL;
  else if (modrm->r == ASM_REG_EBP)
    new->instr = ASM_SHR;
  else if (modrm->r == ASM_REG_EDI)
    new->instr = ASM_SAR;
  else if (modrm->r == ASM_REG_ECX)
      new->instr = ASM_ROR;
  else if (modrm->r == 3)
    new->instr = ASM_RCR;
  else
    new->instr = ASM_SHL;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, 
					ASM_OTYPE_ENCODEDBYTE, new));
  new->len += (olen = asm_operand_fetch(&new->op2, opcode + 1 + olen, 
					ASM_OTYPE_IMMEDIATEBYTE, new));
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  #endif
  return (new->len);
}
