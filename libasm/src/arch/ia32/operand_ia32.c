/**
 * @file operand_ia32.c
 * @ingroup libasm_ia32
** $Id$
**
** Author  : <sk at devhell dot org>
** Started : Tue May 28 13:06:39 2002
** Updated : Sun Mar 21 00:39:09 2004
*/

#include <libasm.h>
#include <libasm-int.h>


/**
 * @brief Extracts operands from memory and stores them in instruction.
 * @todo This function should not be called.
 */
int			operand_rmb_ib(asm_instr *ins, u_char *opcode, 
				       int len, asm_processor *proc) 
{
  struct s_modrm	*modrm;
  
  modrm = (struct s_modrm *) opcode;
  operand_rmb(&ins->op[0], opcode, len, proc);
  ins->op[1].content = ASM_OP_VALUE;
  ins->op[1].len = 1;
  ins->op[1].ptr = opcode + (ins->op[0].len ? ins->op[0].len : 1);
  // if (*(opcode + ins->op1.len) >= 0x80u)
  //   memcpy((char *) &ins->op2.imm + 1, "\xff\xff\xff", 3);
  // else
  ins->op[1].imm = 0;
  memcpy(&ins->op[1].imm, opcode + (ins->op[0].len ? ins->op[0].len : 1), 1);
    
  ins->len += (ins->op[0].len ? ins->op[0].len : 1) + ins->op[1].len;
  return (1);
}


/**
 * @brief Decode a modRM operand of size Byte.
 *
 * @param op Pointer to operand to fill.
 * @param opcode Pointer to operand bytes.
 * @param len Length of data.
 * @param proc Pointer to processor structure
 * @return Length of the operand.
 *
 */ 
int			operand_rmb(asm_operand *op, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm	*modrm;
  struct s_sidbyte	*sidbyte;
  u_char		mode;
  u_char		pmode;

  modrm = (struct s_modrm *) (opcode);
  sidbyte = (struct s_sidbyte *) (opcode + 1);

  mode = asm_ia32_get_mode(proc);
  pmode = (mode == INTEL_PROT);

#if __DEBUG_MODRM__
  fprintf(stderr, "[DEBUG_MODRM] IA32 processor in %c-mode (RMB) \n",
	  (pmode ? 'p' : (mode == INTEL_REAL ? 'r' : 'U')));
#endif

  switch (modrm->mod) 
    {

      /** mod == 00 : operand base */
    case 0:
      
      switch (modrm->m) 
	{ 

	case ASM_REG_ESP:
	  if (sidbyte->base == ASM_REG_EBP) 
	    {
	      op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
	      op->regset = ASM_REGSET_R32;
	      op->len = (pmode ? 6 : 4);				   //	 
	      op->ptr = opcode;
	      op->scale = asm_int_pow2(sidbyte->sid);

	      // erased by the following memcpy !?
	      //if (((unsigned char)*(opcode + 2)) >= 0x80)
	      //memcpy((char *) &op->imm + 2, "\xff\xff", 2);
	      //else
	      //op->imm = 0;
	      memcpy((char *) &op->imm, opcode + 2, (pmode ? 4 : 2)); //
	      op->indexr = sidbyte->index;
	    } 
	  else 
	    {
	      if ((op->indexr = sidbyte->index) != ASM_REG_ESP)
		op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	      else
		op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_SCALE;
	      op->len = 2;
	      op->ptr = opcode;
	      op->regset = ASM_REGSET_R32;
	      op->baser = sidbyte->base;
	      op->scale = asm_int_pow2(sidbyte->sid);
	    }
	  break;
	  
	case ASM_REG_EBP:
	  op->content = ASM_OP_REFERENCE | ASM_OP_VALUE;
	  op->ptr = opcode;
	  op->len = (pmode ? 5 : 3);			  //
	  memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));	  //
	  break;

	default:
	  op->ptr = opcode;
	  op->len = 1;
	  op->content = ASM_OP_REFERENCE | ASM_OP_BASE;
	  op->baser = modrm->m;
	  op->regset = pmode ? ASM_REGSET_R32 : ASM_REGSET_R16;
	  break;
	}
      break;
      
      /** mod == 01 : operand : base + sbyte */
    case 1:
      if (modrm->m == ASM_REG_ESP) 
	{
	  if ((op->indexr = sidbyte->index) != ASM_REG_ESP)
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	  else
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
	  op->ptr = opcode;
	  op->len = (pmode ? 3 : 1);			//
	  op->regset = ASM_REGSET_R32;
	  op->baser = sidbyte->base;
	  op->indexr = sidbyte->index;
	  op->scale = asm_int_pow2(sidbyte->sid);
	  if (*(opcode + 2) >= 0x80u)
	    memset((char *) &op->imm + 1, '\xff', (pmode ? 3 : 1));	//
	  else
	    op->imm = 0;
	  memcpy(&op->imm, opcode + 2, 1);
	} 
      else 
	{
	  op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
	  op->len = 2;								// ?????
	  op->baser = modrm->m;
	  op->regset = ASM_REGSET_R32;
	  if (*(opcode + 1) >= 0x80u)
	    memset((char *) &op->imm + 1, '\xff', (pmode ? 3 : 1));	//
	  else
	    op->imm = 0;
	  memcpy(&op->imm, opcode + 1, 1);
	}
      break;
      
      /** mod == 10 : operand base + sdword */
    case 2:
      op->ptr = opcode;
      if (modrm->m == ASM_REG_ESP) 
	{
	  if (sidbyte->base == ASM_REG_ESP)
	    op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	  else
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
      
	  op->regset = ASM_REGSET_R32;
	  op->baser = sidbyte->base;
	  op->indexr = sidbyte->index;
	  op->scale = asm_int_pow2(sidbyte->sid);
	  op->len = (pmode ? 6 : 4);				//	  
	  memcpy(&op->imm, opcode + 2, (pmode ? 4 : 2));		//	  
	} 
      else 
	{	  
	  op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
	  op->len = (pmode ? 5 : 3);				//
	  op->regset = ASM_REGSET_R32;
	  op->baser = modrm->m;
	  op->imm = 0;
	  memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));		//
	}
      break;
    
      /*
	if (modrm->m == ASM_REG_ESP)
	op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
	op->len = 5;								
	op->baser = modrm->m;
	op->regset = ASM_REGSET_R32;
	memcpy(&op->imm, opcode + 1, 4);						
	op->baser;
	op->imm;
	break;
      */


    /* mod == 3 */
  case 3:
    op->content = ASM_OP_BASE;
    op->regset = ASM_REGSET_R8;
    op->len = 1;
    op->ptr = opcode;
    op->baser = modrm->m;
    break;
  }

  return (op->len);
}


/***
 *
 *
 *
 */

int			operand_rmv(asm_operand *op, u_char *opcode, u_int len, asm_processor *proc) 
{		       
  struct s_modrm	*modrm;
  struct s_sidbyte	*sidbyte;
  u_char		mode;
  u_char		pmode;

  mode = asm_ia32_get_mode(proc);
  pmode = (mode == INTEL_PROT);

#if __DEBUG_MODRM__
  fprintf(stderr, "[DEBUG_MODRM] IA32 processor in %c-mode (RMV) \n",
	  (pmode ? 'p' : (mode == INTEL_REAL ? 'r' : 'U')));
#endif  

  modrm = (struct s_modrm *) (opcode);
  sidbyte = (struct s_sidbyte *) (opcode + 1);
  
  op->regset = pmode ? ASM_REGSET_R32 : ASM_REGSET_R16;
  switch (modrm->mod) 
    {

      /* modrm->mod = 0 */      
    case 0:
      
      switch (modrm->m)
	{

	  /* modrm == ESP */
	case ASM_REG_ESP:
	  switch (sidbyte->base)
	    {
	      
	    case ASM_REG_EBP:
	      
	      /* pushl 0x8050fe0(,%eax,4) ; opcode = 'ff 34 85 e0 0f 05 08' */
	      op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
	      op->regset = ASM_REGSET_R32;
	      op->len = (pmode ? 6 : 4);					//
	      op->ptr = opcode;
	      op->scale = asm_int_pow2(sidbyte->sid);
	      memcpy((char *) &op->imm, opcode + 2, (pmode ? 4 : 2));	//
	      op->baser = -1;
	      op->indexr = sidbyte->index;
	      break;
	  
	    case ASM_REG_ESP:
	      op->content = ASM_OP_REFERENCE | ASM_OP_SCALE | ASM_OP_BASE;
	      op->len = 2;
	      op->baser = sidbyte->base;
	      op->scale = asm_int_pow2(sidbyte->sid);
	      break;

	    default:
	      op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	      // op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	      op->len = 2;
	      op->ptr = opcode;
	      op->regset = ASM_REGSET_R32;
	      op->baser = sidbyte->base;
	      op->scale = asm_int_pow2(sidbyte->sid);
	      op->indexr = sidbyte->index;
	      break;
	    }
	  break;
	  
	  /* modrm == EBP */
	case ASM_REG_EBP:
	  op->content = ASM_OP_REFERENCE | ASM_OP_VALUE;
	  op->ptr = opcode;
	  op->len = (pmode ? 5 : 3);					//
	  memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));			//
	  break;

	  /* modrm != EBP and modrm != ESP */
	default:
	  op->ptr = opcode;
	  op->len = 1;
	  op->content = ASM_OP_REFERENCE | ASM_OP_BASE;
	  op->baser = modrm->m;
	  break;
	}
      break;
   
      /* modrm->mod = 1 */
    case 1:
      if (modrm->m == ASM_REG_ESP) 
	{
	  if (sidbyte->base == ASM_REG_ESP)
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
	  else
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	  op->ptr = opcode;
	  op->len = 3;									// ????
	  op->regset = ASM_REGSET_R32;
	  op->baser = sidbyte->base;
	  op->indexr = sidbyte->index;
	  op->scale = asm_int_pow2(sidbyte->sid);
	  if (*(opcode + 2) >= 0x80u)
	    memset((char *) &op->imm + 1, '\xff', (pmode ? 3 : 1));	//
	  else
	    op->imm = 0;
	  memcpy(&op->imm, opcode + 2, 1);
	} 
    else 
      {
	op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
	op->len = 2;									// ?????
	op->regset = ASM_REGSET_R32;
	op->baser = modrm->m;
	if (*(opcode + 1) >= 0x80u)
	  memset((char *) &op->imm + 1, '\xff', (pmode ? 3 : 1));		//
	else
	  op->imm = 0;
	memcpy(&op->imm, opcode + 1, 1);
      }
      break;

      /* modrm->mod = 2 */      
    case 2:
      if (modrm->m == ASM_REG_ESP) 
	{
	  if (sidbyte->base == ASM_REG_ESP)
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
	  else
	    op->content = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
	  
	  op->len = (pmode ? 6 : 4);					//
	  op->ptr = opcode;
	  op->baser = sidbyte->base;
	  op->regset = ASM_REGSET_R32;
	  op->indexr = sidbyte->index;
	  op->scale = asm_int_pow2(sidbyte->sid);
	  memcpy(&op->imm, opcode + 2, (pmode ? 4 : 2));			//
	} 
      else 
	{
	  op->content = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
	  op->len = (pmode ? 5 : 3);					//
	  op->ptr = opcode;
	  op->regset = ASM_REGSET_R32;
	  op->baser = modrm->m;
	  memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));			//
	}
      break;
      
      /* modrm->mod = 3 */
    case 3:
      op->content = ASM_OP_BASE;
      op->len = 1;
      op->ptr = opcode;
      op->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
      op->baser = modrm->m;
      break;
    }

  return (op->len);
}


/**
 * Note about operand content field.
 * This field contain two packed value
 * one for the content of the operand, which is a bitfield,
 * and an otype value which is the type of the operand.
 * This type is related to the sandpile.org reference and
 * to the enum ASM_CONTENT_* (!!add link to enum!!)
 */

/**
 * Pack content and otype values of the content field.
 * @param content
 * @param otype
 */
int	asm_content_pack(asm_operand *op, int content, int otype)
{
  return (0);
}

/**
 * Return content part of the content field
 */
int	asm_content_extract_otype(asm_operand *op)
{
  return (0);
}

/**
 * Return otype part of the content field
 * param asm_op_content Content field of an operand.
 */
int	asm_content_extract_content(asm_operand *op)
{
  return (0);
}

/**
 * Return a packed value for asm_operand_fetch_fixed optional parameters.
 * @todo Implement this correctly.
 * opt may be a regset or nothing
 * value may be a register, or a fixed value
 * otype is an 
 */
int	asm_fixed_pack(int otype, int content, int value, int opt)
{
  return (otype | content | value | opt);
}




