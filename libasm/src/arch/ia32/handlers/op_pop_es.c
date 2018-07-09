/*
** $Id$
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x07
  Instruction :         POP
*/

int op_pop_es(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_POP;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_ASSIGN | ASM_TYPE_LOAD;
  new->spdiff = 4;
  new->len += 1;
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].baser = ASM_REG_ES;
  return (new->len);
}
