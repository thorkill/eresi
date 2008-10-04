/*
** $Id: op_pop_reg.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_pop_reg" opcode="0x58"/>
  <instruction func="op_pop_reg" opcode="0x59"/>
  <instruction func="op_pop_reg" opcode="0x5a"/>
  <instruction func="op_pop_reg" opcode="0x5b"/>
  <instruction func="op_pop_reg" opcode="0x5c"/>
  <instruction func="op_pop_reg" opcode="0x5d"/>
  <instruction func="op_pop_reg" opcode="0x5e"/>
  <instruction func="op_pop_reg" opcode="0x5f"/>
*/

int op_pop_reg(asm_instr *new, unsigned char *opcode, unsigned int len, 
               asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_POP;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_ASSIGN | ASM_TYPE_LOAD;
  new->spdiff = 4;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_OPMOD, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_OPMOD, new);
#endif
  if (new->op[0].baser == ASM_REG_EBP)
    new->type |= ASM_TYPE_EPILOG;

  return (new->len);
}
