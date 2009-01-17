/**
 * @file op_shr_ib.c
 * @ingroup handlers_ia32
** $Id$
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
  int                   olen;

  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF;

  if (modrm->r == ASM_REG_EAX)
    new->instr = ASM_ROL;
  else if (modrm->r == ASM_REG_EBP) {
    new->instr = ASM_SHR;
    new->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
  }
  else if (modrm->r == ASM_REG_EDI) {
    new->instr = ASM_SAR;
    new->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
  }
  else if (modrm->r == ASM_REG_ECX)
      new->instr = ASM_ROR;
  else if (modrm->r == 3)
    new->instr = ASM_RCR;
  else {
    new->instr = ASM_SHL;
    new->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
  }

#if WIP
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,
                                        ASM_CONTENT_ENCODEDBYTE, new, 0));
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,
                                ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += (olen = asm_operand_fetch(&new->op[0], opcode + 1,
                                        ASM_CONTENT_ENCODEDBYTE, new));
  new->len += asm_operand_fetch(&new->op[1], opcode + 1 + olen,
                                ASM_CONTENT_IMMEDIATEBYTE, new);
#endif
  return (new->len);
}
