/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the enter instruction, opcode 0xc8
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
*/

int op_enter(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_ENTER;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,				ASM_CONTENT_IMMEDIATEWORD, new);
#else
  new->op[0].content = ASM_CONTENT_IMMEDIATE;
  new->op[0].type = ASM_OPTYPE_IMM;
  new->op[0].len = 2;
  new->op[0].ptr = opcode + 1;

  new->op[0].imm = 0;
  memcpy(&new->op[0].imm, opcode + 1, 2);
  new->len += 2;
#endif
  new->spdiff = -new->op[0].imm;
  return (new->len);
}
