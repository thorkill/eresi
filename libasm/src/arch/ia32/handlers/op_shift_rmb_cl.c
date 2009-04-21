/**
 * @file op_shift_rmb_cl.c
 * @ingroup handlers_ia32
 * $Id$
 *
 * Changelog
 * 2007-05-29 : instruction set was not complete.
 *		operand type for operand 1 was incorrect.
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction shit rmb,cl opcode 0xd2
 * <instruction func="op_shift_rmb_cl" opcode="0xd2"/>
 */

int op_shift_rmb_cl(asm_instr *instr, u_char *opcode, u_int len, 
                    asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode + 1;
  instr->ptr_instr = opcode;
  instr->len += 1;
  instr->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  instr->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF;

  switch(modrm->r)
  {
    case 0: instr->instr = ASM_ROL; break;
    case 1: instr->instr = ASM_ROR; break;
    case 2: instr->instr = ASM_RCL; break;
    case 3: instr->instr = ASM_RCR; break;
    case 4:
      instr->instr = ASM_SHL;
      instr->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
      break;
    case 5:
      instr->instr = ASM_SHR;
      instr->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
      break;
    case 6:
      instr->instr = ASM_SAL;
      instr->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
      break;
    case 7:
      instr->instr = ASM_SAR;
      instr->flagswritten |= ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF;
      break;
  }

#if WIP
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE,
                                instr, 0);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_CONTENT_FIXED, instr,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_CL,
					       ASM_REGSET_R8));

#else
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE,
                                instr);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_CONTENT_FIXED, instr);
  instr->op[1].type = ASM_OPTYPE_REG;
  instr->op[1].regset = ASM_REGSET_R8;
  instr->op[1].baser = ASM_REG_CL;
#endif

  return (instr->len);
}
