/*
** $Id: op_incdec_rmb.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_incdec_rmb" opcode="0xfe"/>
*/

int op_incdec_rmb(asm_instr *new, unsigned char *opcode, unsigned int len,
                  asm_processor *proc)
{
  struct s_modrm        *modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_INCDEC | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;

  switch(modrm->r) {
    case 0:
      new->instr = ASM_INC;
      break;
    case 1:
      new->instr = ASM_DEC;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      break;
    default:
      break;
  }

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODEDBYTE,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODEDBYTE,                                new);
#endif

  return (new->len);
}
