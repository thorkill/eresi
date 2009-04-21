/**
 * $Id$
 * @param 
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for fpu instruction for opcode 0xdf
 *
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 *
 <instruction func="op_esc7" opcode="0xdf"/>
*/

int op_esc7(asm_instr *instr, u_char *opcode, u_int len, asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode + 1;
  instr->ptr_instr = opcode;
  instr->len += 1;
  if (*(opcode + 1) == 0xe0) 
  {
    if (!(instr->prefix & ASM_PREFIX_FWAIT))
      instr->instr = ASM_FNSTSW;
    else
      instr->instr = ASM_FSTSW;
    instr->op[0].content = ASM_CONTENT_FIXED;
    instr->op[0].type = ASM_OPTYPE_REG;
    instr->op[0].regset = ASM_REGSET_R16;
    instr->op[0].baser = ASM_REG_EAX;
  } 
  else
  switch (modrm->r) 
  {
    case 0:    
    instr->instr = ASM_FILD;
    break;
    case 1:
    // bad instr->instr = ASM_;
    break;
    case 2:
    instr->instr = ASM_FIST;
    break;
    case 3:
    instr->instr = ASM_FISTP;
    break;
    case 4:
    instr->instr = ASM_FBLD;
    break;
    case 5:
    instr->instr = ASM_FILD;
    break;
    case 6:
    instr->instr = ASM_FBSTP;
    break;
    case 7:
    instr->instr = ASM_FISTP;
    break;
  }
  if (*(opcode + 1) != 0xe0) {
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
    instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_ENCODED,
				  instr, 0);
#else
    instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_ENCODED,
				  instr);
#endif
#else
    instr->op[0].content = ASM_CONTENT_ENCODED;
    operand_rmv(&instr->op[0], opcode + 1, len - 1, proc);
    instr->len += instr->op[0].len;
#endif
  } else
  instr->len++;
  return (instr->len);
}
