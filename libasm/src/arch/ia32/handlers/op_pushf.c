/*
** $Id: op_pushf.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction name="pushf" func="op_pushf" opcode="0x9c"/>
*/


int op_pushf(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSHF;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE | ASM_TYPE_READFLAG;
  new->spdiff = -4;
  new->flagsread = ASM_FLAG_CF | ASM_FLAG_PF | ASM_FLAG_AF |
                        ASM_FLAG_ZF | ASM_FLAG_SF | ASM_FLAG_TF |
                        ASM_FLAG_IF | ASM_FLAG_DF | ASM_FLAG_OF |
                        ASM_FLAG_IOPL | ASM_FLAG_NT | ASM_FLAG_RF |
                        ASM_FLAG_AC | ASM_FLAG_VIF | ASM_FLAG_VIP |
                        ASM_FLAG_ID;
  return (new->len);
}
