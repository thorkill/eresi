/*
** $Id: op_adc_eax_iv.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_eax_iv" opcode="0x15"/>
*/

int op_adc_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc)
{
  new->instr = ASM_ADC;
  new->ptr_instr = opcode;
  new->len += 1;
  
  new->type = ASM_TYPE_ARITH | ASM_TYPE_READFLAG | ASM_TYPE_WRITEFLAG;
  new->flagsread = ASM_FLAG_CF;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF | 
                       ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.type = ASM_OTYPE_FIXED;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.baser = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATE, 
                                new);

  return (new->len);
}
