/**
 * @file op_mov_eax_ref_iv.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Instruction handler for opcode 0xa1
 *
  <instruction func="op_mov_eax_ref_iv" opcode="0xa1"/>
*/

int op_mov_eax_ref_iv(asm_instr *new, u_char *opcode, u_int len,
                      asm_processor *proc)
{
  new->instr = ASM_MOV;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ASSIGN;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));
  
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_FIXED, new);
#endif
  new->op[0].type = ASM_OP_BASE;
  new->op[0].ptr = opcode;
  new->op[0].content = ASM_CONTENT_FIXED;
  new->op[0].regset = asm_proc_opsize(proc) ?
                      ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op[0].baser = ASM_REG_EAX;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_OFFSET, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_OFFSET, new);
#endif

  return (new->len);
}
