/**
 * @file op_xchg_eax_reg.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction xchg eax,reg opcode 0x91 to 0x97
** $Id: op_xchg_eax_reg.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xchg eax,reg opcode 0x91 to 0x97
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_xchg_eax_reg(asm_instr *new, unsigned char *opcode, unsigned int len, 
                    asm_processor *proc)
{
  new->instr = ASM_XCHG;
  new->ptr_instr = opcode;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, 
                                new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_OPMOD, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, 
                                new);
  new->op[0].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].len = 0;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_OPMOD, new);
#endif
  return (new->len);
}
