/**
 * @file i386_shrd_rmv_rv_ib.c
 * $Id: i386_shrd_rmv_rv_ib.c,v 1.3 2007-05-19 23:59:12 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction shrd, opcode 0x0f 0xac
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int i386_shrd_rmv_rv_ib(asm_instr *new, u_char *opcode, u_int len, 
			asm_processor *proc) 
{
  int		olen;
  new->instr = ASM_SHRD;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, 
					ASM_OTYPE_ENCODED, proc));
  new->len += asm_operand_fetch(&new->op2, opcode + 1, 
				ASM_OTYPE_GENERAL, proc);
  new->len += asm_operand_fetch(&new->op3, opcode + 1 + olen, 
				ASM_OTYPE_IMMEDIATEBYTE, proc);
#else
    new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_GENERAL;
  new->op2.size = ASM_OSIZE_VECTOR;
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.size = ASM_OSIZE_BYTE;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  new->op3.type = ASM_OTYPE_IMMEDIATE;
  new->op3.content = ASM_OP_VALUE;
  new->op3.ptr = opcode + 2;
  new->op3.len = 1;
    
  new->op3.imm = 0;
  memcpy(&new->op3.imm, opcode + 2, 1);
#endif
  return (new->len);
}
