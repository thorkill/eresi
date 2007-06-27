/**
 * @file op_shift_rmb_cl.c
 * @ingroup handlers_ia32
 * $Id: op_shift_rmb_cl.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
 *
 * Changelog
 * 2007-05-29 : instruction set was not complete.
 *		operand type for operand 1 was incorrect.
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmb_cl" opcode="0xd2"/>
 */

int op_shift_rmb_cl(asm_instr *new, u_char *opcode, u_int len, 
		    asm_processor *proc) 
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch(modrm->r) 
    {
    case 0: new->instr = ASM_ROL; break;
    case 1: new->instr = ASM_ROR; break;
    case 2: new->instr = ASM_RCL; break;
    case 3: new->instr = ASM_RCR; break;
    case 4: new->instr = ASM_SHL; break;
    case 5: new->instr = ASM_SHR; break;
    case 6: new->instr = ASM_SAL; break;
    case 7: new->instr = ASM_SAR; break;
  }

  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
				new);
  /**
   * @bug
   * This may raise some bug. Should be checked as soon as possible.
   * Check wether fetching ASM_OTYPE_ENCODED* should return 0 or 1 if
   * only the modrm byte is used.
   */
  new->len += 1;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_FIXED, new);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.baser = ASM_REG_CL;
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->op2.content = ASM_OP_BASE;
  new->op2.regset = ASM_REGSET_R8;
  new->op2.baser = ASM_REG_CL;
  new->len += new->op1.len;
#endif
  return (new->len);
}
