/*
** $Id: op_pop_rmv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the pop [rmv] instruction opcode = 0x8f
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Lenght of buffer to disassemble.
 * @param proc Pointer to processor structure.
 *
*/

int op_pop_rmv(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_POP;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_ASSIGN | ASM_TYPE_LOAD;
  new->spdiff = 4;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,
                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,
                                new);
#endif

  return (new->len);
}
