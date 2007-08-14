/**
 * @file op_unary_rmb.c
 * @ingroup handlers_ia32
* $Id: op_unary_rmb.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
*
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_unary_rmb" opcode="0xf6"/>
*/

int op_unary_rmb(asm_instr *new, u_char *opcode, u_int len, 
                 asm_processor *proc)
{
  struct s_modrm        *modrm;
  int                    olen;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  new->type = ASM_TYPE_ARITH;

  switch(modrm->r) {
    case 0:
      new->instr = ASM_TEST;
      new->op1.type = ASM_OTYPE_ENCODED;

      new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1,
                                            ASM_OTYPE_ENCODEDBYTE, new));
      new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen,
                                    ASM_OTYPE_IMMEDIATEBYTE, new);

      break;
    case 1:
      return (0);
      break;
    case 2:
      new->instr = ASM_NOT;
      break;
    case 3:
      new->instr = ASM_NEG;
      new->type |= ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_CF;
      break;
    case 4:
      new->instr = ASM_MUL;
      new->type |= ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;
      break;
    case 5:
      new->instr = ASM_IMUL;
      new->type |= ASM_TYPE_WRITEFLAG;
      new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF;
      break;
    case 6:
      new->instr = ASM_DIV;
      break;
    case 7:
      new->instr = ASM_IDIV;
      break;
    default:
      break;
  }

  if (!new->op1.type) 
  {
    new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                  ASM_OTYPE_ENCODEDBYTE, new);
  }
  return (new->len);
}
