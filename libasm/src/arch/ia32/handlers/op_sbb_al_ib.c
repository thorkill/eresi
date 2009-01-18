/**
 * @file op_sbb_al_ib.c
 * @ingroup handlers_ia32
** $Id$
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_sbb_al_ib" opcode="0x1c"/>
*/

int op_sbb_al_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_SBB;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG | ASM_TYPE_READFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_FIXED, new);
  new->op[0].size = new->op[1].size = ASM_OSIZE_BYTE;
  new->op[0].type = ASM_OP_BASE;
  new->op[0].ptr = opcode;
  new->op[0].len = 0;
  new->op[0].baser = ASM_REG_AL;
  new->op[0].regset = ASM_REGSET_R8;
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_IMMEDIATEBYTE, new);
  return (new->len);
}
