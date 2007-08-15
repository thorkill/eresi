/*
** $Id: op_retf_i2.c,v 1.5 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_retf_i2" opcode="0xca"/>
*/

int op_retf_i2(asm_instr *new, u_char *opcode, u_int len, 
               asm_processor *proc)
{
  new->instr = ASM_RETF;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_RETPROC | ASM_TYPE_TOUCHSP | ASM_TYPE_EPILOG;

  new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                ASM_OTYPE_IMMEDIATEWORD, new);

  return (new->len);
}
