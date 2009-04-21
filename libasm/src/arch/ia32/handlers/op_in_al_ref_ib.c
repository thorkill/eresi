/**
 * @file op_in_al_ref_ib.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction in al,ib opcode 0xe4
 
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction in al,ib opcode 0xe4
 *
  <instruction func="op_in_al_ref_ib" opcode="0xe4"/>
*/

int op_in_al_ref_ib(asm_instr *new, u_char *opcode, u_int len,
                    asm_processor *proc)
{
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_FIXED, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_IMMEDIATEBYTE, new);
#else

  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = ASM_REGSET_R8;
  new->op[0].baser = ASM_REG_AL;

  new->op[0].content = ASM_CONTENT_FIXED;
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = ASM_REGSET_R8;
  new->op[0].baser = ASM_REG_AL;

  new->op[1].content = ASM_CONTENT_IMMEDIATE;
  new->op[1].type = ASM_OPTYPE_IMM;

  new->len += 1;
  new->op[1].imm = 0;
  memcpy(&new->op[1].imm, opcode + 1, 1);
#endif
  return (new->len);
}
