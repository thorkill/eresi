/*
** $Id: op_enter.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
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
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_IMMEDIATEWORD, new);
#else

  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.len = 2;
  new->op1.ptr = opcode + 1;
    
  new->op1.imm = 0;
  memcpy(&new->op1.imm, opcode + 1, 2);
  new->len += 2;
#endif
  new->spdiff = -new->op1.imm;
  return (new->len);
}
