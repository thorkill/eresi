/*
** $Id: op_popf.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction name="popf" func="op_popf" opcode="0x9d"/>
*/

int op_popf(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_POPF;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_LOAD | ASM_TYPE_WRITEFLAG;
  new->spdiff = 4;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_PF | ASM_FLAG_AF |
                        ASM_FLAG_ZF | ASM_FLAG_SF | ASM_FLAG_TF |
                        ASM_FLAG_IF | ASM_FLAG_DF | ASM_FLAG_OF |
                        ASM_FLAG_IOPL | ASM_FLAG_NT | ASM_FLAG_RF |
                        ASM_FLAG_AC | ASM_FLAG_VIF | ASM_FLAG_VIP |
                        ASM_FLAG_ID;
  return (new->len);
}
