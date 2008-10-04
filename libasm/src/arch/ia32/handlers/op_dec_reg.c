/*
** $Id: op_dec_reg.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_dec_reg" opcode="0x48"/>
  <instruction func="op_dec_reg" opcode="0x49"/>
  <instruction func="op_dec_reg" opcode="0x4a"/>
  <instruction func="op_dec_reg" opcode="0x4b"/>
  <instruction func="op_dec_reg" opcode="0x4c"/>
  <instruction func="op_dec_reg" opcode="0x4d"/>
  <instruction func="op_dec_reg" opcode="0x4e"/>
  <instruction func="op_dec_reg" opcode="0x4f"/>
*/

int op_dec_reg(asm_instr *new, unsigned char *opcode, unsigned int len,
               asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
  new->instr = ASM_DEC;
  new->len += 1;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_INCDEC | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_OPMOD, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_OPMOD, new);
#endif

  return (new->len);
}
