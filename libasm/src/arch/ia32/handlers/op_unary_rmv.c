/**
 * @file op_unary_rmv.c
 * @ingroup handlers_ia32
 * $Id: op_unary_rmv.c,v 1.6 2007-08-15 21:30:21 strauss Exp $
 *
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_unary_rmv" opcode="0xf7"/>
*/

int op_unary_rmv(asm_instr *new, u_char *opcode, u_int len, 
                 asm_processor *proc) 
{
  struct s_modrm        *modrm;
  int                   olen;

  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ARITH;
  switch (modrm->r) {
  case 0:
    new->instr = ASM_TEST;
    new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
    new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF | ASM_FLAG_PF |
                          ASM_FLAG_SF | ASM_FLAG_ZF;
    break;
  case 1:
    return (0);
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
  } /* switch */

  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1,
                                        ASM_OTYPE_ENCODED, new));

  if (new->instr == ASM_TEST) {
    new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen,
                                  ASM_OTYPE_IMMEDIATE, new);
  }
  return (new->len);
}
