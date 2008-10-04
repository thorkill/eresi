/*
** $Id: op_enter.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
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

int op_enter(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->instr = ASM_ENTER;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,				ASM_OTYPE_IMMEDIATEWORD, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1,				ASM_OTYPE_IMMEDIATEWORD, new);
#endif
#else

  new->op[0].type = ASM_OTYPE_IMMEDIATE;
  new->op[0].content = ASM_OP_VALUE;
  new->op[0].len = 2;
  new->op[0].ptr = opcode + 1;

  new->op[0].imm = 0;
  memcpy(&new->op[0].imm, opcode + 1, 2);
  new->len += 2;
#endif
  new->spdiff = -new->op[0].imm;
  return (new->len);
}
