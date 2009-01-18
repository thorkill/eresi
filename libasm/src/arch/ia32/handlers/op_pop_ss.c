/**
 * @brief Handler for pop es instruction. Opcode = 0x17
 * @ingroup handlers_ia32
 **
** $Id$
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for pop es instruction. Opcode = 0x17
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 */
int     op_pop_ss(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_POP;
  new->len += 1;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_ASSIGN | ASM_TYPE_LOAD;
  new->spdiff = 4;
  new->ptr_instr = opcode;
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].type = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_SREG;
  new->op[0].baser = ASM_REG_SS;
  return (new->len);
}
