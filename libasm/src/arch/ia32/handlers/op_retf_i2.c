/**
 * @file op_retf_i2.c
 * @ingroup libasm/ia32/handlers
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_retf_i2" opcode="0xca"/>
*/

int op_retf_i2(asm_instr *instr, u_char *opcode, u_int len, 
               asm_processor *proc)
{
  instr->instr = ASM_RETF;
  instr->ptr_instr = opcode;
  instr->len += 1;
  instr->type = ASM_TYPE_RETPROC | ASM_TYPE_TOUCHSP | ASM_TYPE_EPILOG;

#if WIP
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1,
                                ASM_OTYPE_IMMEDIATEWORD, instr, 0);
#else
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1,
                                ASM_OTYPE_IMMEDIATEWORD, instr);
#endif
  return (instr->len);
}
