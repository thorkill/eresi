/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_rb_rmb" opcode="0x12"/>
*/

int op_adc_rb_rmb(asm_instr *new, u_char *opcode, u_int len,
		  asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_ADC;
  new->ptr_instr = opcode;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_READFLAG | ASM_TYPE_WRITEFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF |
                       ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERALBYTE, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODEDBYTE, new);

  return (new->len);
}
