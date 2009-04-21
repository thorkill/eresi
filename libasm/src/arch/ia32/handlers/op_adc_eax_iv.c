/*
** $Id$
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_eax_iv" opcode="0x15"/>
*/

int op_adc_eax_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_ADC;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_READFLAG | ASM_TYPE_WRITEFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF |
                       ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF;
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].content = ASM_CONTENT_FIXED;
  new->op[0].size = new->op[1].size = asm_proc_vector_size(proc);
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].baser = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_IMMEDIATE, new);
  return (new->len);
}
