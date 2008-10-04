/**
 * @file op_esc5.c
 * @ingroup handlers_ia32
 * $Id: op_esc5.c,v 1.7 2008/01/07 01:32:56 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for esc5 instruction group, opcode 0xdd
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int op_esc5(asm_instr *new, unsigned char *opcode, unsigned int len,
	    asm_processor *proc)
{
  struct s_modrm        *modrm;
  new->ptr_instr = opcode;

  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  if (modrm->mod == 3)
    {
      new->len += 1;
      switch(modrm->r)
	{
	case 3:
	  new->instr = ASM_FSTP;
	  break;
	case 4:
	  new->instr = ASM_FUCOM;
	  break;
	case 5:
	  new->instr = ASM_FUCOMP;
	  break;
	default:
	  new->instr = ASM_BAD;
	  break;
	}
    }
  else
    switch (modrm->r) {
    case 0:
      new->instr = ASM_FLD;
      break;
    case 1:
      new->instr = ASM_BAD;
      break;
    case 2:
      new->instr = ASM_FST;
      break;
    case 3:
      new->instr = ASM_FSTP;
      break;
    case 4:
      new->instr = ASM_BAD; // ASM_FRSTOR;
      break;
    case 5:
      new->instr = ASM_BAD; // ;
      break;
    case 6:
      if (!(new->prefix & ASM_PREFIX_FWAIT))
	new->instr = ASM_FNSAVE;
      else
	new->instr = ASM_FSAVE;
      break;
    case 7:
      if (!(new->prefix & ASM_PREFIX_FWAIT))
	new->instr = ASM_FNSAVE;
      else
	new->instr = ASM_FSAVE;
      break;
    }

  if (modrm->mod == 3)
    {
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE, 
					       modrm->m, 0));

#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
      new->op[0].content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op[0].len = 1;
      new->op[0].ptr = opcode + 1;
      new->op[0].scale = modrm->m;
#else
      new->op[0].type = ASM_OTYPE_FIXED;
      new->op[0].content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op[0].len = 1;
      new->op[0].ptr = opcode + 1;
      new->op[0].scale = modrm->m;
#endif
    }
  else
    {
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new);
#endif
#else
      new->op[0].type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
#endif
    }
#if LIBASM_USE_OPERAND_VECTOR
#else
  if (new->op[0].type)
    new->len += new->op[0].len;
#endif
  return (new->len);
}
