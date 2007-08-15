/*
** $Id: op_immed_rmv_iv.c,v 1.6 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_immed_rmv_iv" opcode="0x81"/>
 */

int op_immed_rmv_iv(asm_instr *new, u_char *opcode, u_int len, 
                    asm_processor *proc)
{
  int                   olen;
  struct s_modrm        *modrm;
  new->ptr_instr = opcode;

  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_OF |
                        ASM_FLAG_PF | ASM_FLAG_SF | ASM_FLAG_ZF;

  switch(modrm->r) {
    case 0:
      new->instr = ASM_ADD;
      break;
    case 1:
      new->instr = ASM_OR;
      new->flagswritten ^= ASM_FLAG_AF;
      break;
    case 2:
      new->instr = ASM_ADC;
      new->type |= ASM_TYPE_READFLAG;
      new->flagsread = ASM_FLAG_CF;
      break;
    case 3:
      new->instr = ASM_SBB;
      new->type |= ASM_TYPE_READFLAG;
      new->flagsread = ASM_FLAG_CF;
      break;
    case 4:
      new->instr = ASM_AND;
      new->flagswritten ^= ASM_FLAG_AF;
      break;
    case 5:
      new->instr = ASM_SUB;
      if (new->op1.content == ASM_OP_BASE &&
            new->op1.baser == ASM_REG_ESP)
        new->type |= ASM_TYPE_EPILOG;
      break;
    case 6:
      new->instr = ASM_XOR;
      new->flagswritten ^= ASM_FLAG_AF;
      break;
    case 7:
      new->instr = ASM_CMP;
      new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
      break;
  }

  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1,
                                        ASM_OTYPE_ENCODED, new));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen,
                                ASM_OTYPE_IMMEDIATE, new);

  return (new->len);
}
