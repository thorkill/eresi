/*
** $Id: op_adc_rv_rmv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_adc_rv_rmv" opcode="0x13"/>
*/

int op_adc_rv_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_ADC;
  new->op1.size = new->op2.size = asm_proc_vector_size(proc);
  new->op1.type = ASM_OTYPE_GENERAL;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op2.type = ASM_OTYPE_ENCODED;
  new->op2.size= ASM_OSIZE_BYTE;
  operand_rv_rmv(new, opcode + 1, len - 1, proc);
  return (new->len);
}
