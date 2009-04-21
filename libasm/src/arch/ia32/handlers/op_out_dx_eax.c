/**
 * @file op_out_dx_eax.c
 * @ingroup handlers_ia32
 *
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_dx_eax" opcode="0xef"/>
 */

int     op_out_dx_eax(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_OUT;
  new->type = ASM_TYPE_IO | ASM_TYPE_STORE;


#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_DX,
					       ASM_REGSET_R16));
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
#endif
  new->op[0].type = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[0].regset = ASM_REGSET_R16;
  new->op[0].baser = ASM_REG_DX;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));
  
#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new);
#endif
  new->op[1].type = ASM_OPTYPE_REG;
  new->op[1].regset = ASM_REGSET_R32;
  new->op[1].baser = ASM_REG_EAX;

  return (new->len);
}
