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
 * @brief Decode a modRM operand of size Byte.
 *
 * @param op Pointer to operand to fill.
 * @param opcode Pointer to operand bytes.
 * @param len Length of data.
 * @param proc Pointer to processor structure
 * @return Length of the operand.
 *
 */ 
int operand_rmb(asm_operand *op, u_char *opcode, asm_processor *proc) 
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
      op->type = ASM_OPTYPE_MEM;
      switch (modrm->m) 
      { 

        case ASM_REG_ESP:
          if (sidbyte->base == ASM_REG_EBP) 
          {
            op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
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
              op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
            else
              op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_SCALE;
            op->len = 2;
            op->ptr = opcode;
            op->regset = ASM_REGSET_R32;
            op->baser = sidbyte->base;
            op->scale = asm_int_pow2(sidbyte->sid);
          }
          break;

        case ASM_REG_EBP:
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE;
          op->ptr = opcode;
          op->len = (pmode ? 5 : 3);			  //
          memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));	  //
          break;

        default:
          op->ptr = opcode;
          op->len = 1;
          op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE;
          op->baser = modrm->m;
          op->regset = pmode ? ASM_REGSET_R32 : ASM_REGSET_R16;
          break;
      }
      break;

      /** mod == 01 : operand : base + sbyte */
    case 1:
		  op->type = ASM_OPTYPE_MEM;
      if (modrm->m == ASM_REG_ESP) 
      {
        if ((op->indexr = sidbyte->index) != ASM_REG_ESP)
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
        else
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
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
        op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
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
		  op->type = ASM_OPTYPE_MEM;
      op->ptr = opcode;
      if (modrm->m == ASM_REG_ESP) 
      {
        if (sidbyte->base == ASM_REG_ESP)
          op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
        else
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;

        op->regset = ASM_REGSET_R32;
        op->baser = sidbyte->base;
        op->indexr = sidbyte->index;
        op->scale = asm_int_pow2(sidbyte->sid);
        op->len = (pmode ? 6 : 4);				//	  
        memcpy(&op->imm, opcode + 2, (pmode ? 4 : 2));		//	  
      } 
      else 
      {	  
        op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
        op->len = (pmode ? 5 : 3);				//
        op->regset = ASM_REGSET_R32;
        op->baser = modrm->m;
        op->imm = 0;
        memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));		//
      }
      break;

      /* mod == 3 */
    case 3:
      op->type = ASM_OPTYPE_REG;
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

int operand_rmv(asm_operand *op, u_char *opcode, asm_processor *proc) 
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
      op->type = ASM_OPTYPE_MEM;
      switch (modrm->m)
      {

        /* modrm == ESP */
        case ASM_REG_ESP:
          switch (sidbyte->base)
          {

            case ASM_REG_EBP:

              /* pushl 0x8050fe0(,%eax,4) ; opcode = 'ff 34 85 e0 0f 05 08' */
              op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_INDEX | ASM_OP_SCALE;
              op->regset = ASM_REGSET_R32;
              op->len = (pmode ? 6 : 4);					//
              op->ptr = opcode;
              op->scale = asm_int_pow2(sidbyte->sid);
              memcpy((char *) &op->imm, opcode + 2, (pmode ? 4 : 2));	//
              op->baser = -1;
              op->indexr = sidbyte->index;
              break;

            case ASM_REG_ESP:
              op->memtype = ASM_OP_REFERENCE | ASM_OP_SCALE | ASM_OP_BASE;
              op->len = 2;
              op->baser = sidbyte->base;
              op->scale = asm_int_pow2(sidbyte->sid);
              break;

            default:
              op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
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
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE;
          op->ptr = opcode;
          op->len = (pmode ? 5 : 3);					//
          memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));			//
          break;

          /* modrm != EBP and modrm != ESP */
        default:
          op->ptr = opcode;
          op->len = 1;
          op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE;
          op->baser = modrm->m;
          break;
      }
      break;

      /* modrm->mod = 1 */
    case 1:
		  op->type = ASM_OPTYPE_MEM;
      if (modrm->m == ASM_REG_ESP) 
      {
        if (sidbyte->base == ASM_REG_ESP)
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
        else
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;
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
        op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
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
		  op->type = ASM_OPTYPE_MEM;
      if (modrm->m == ASM_REG_ESP) 
      {
        if (sidbyte->base == ASM_REG_ESP)
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE;
        else
          op->memtype = ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE;

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
        op->memtype = ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_VALUE;
        op->len = (pmode ? 5 : 3);					//
        op->ptr = opcode;
        op->regset = ASM_REGSET_R32;
        op->baser = modrm->m;
        memcpy(&op->imm, opcode + 1, (pmode ? 4 : 2));			//
      }
      break;

      /* modrm->mod = 3 */
    case 3:
      op->type = ASM_OPTYPE_REG;
      op->len = 1;
      op->ptr = opcode;
      op->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
      op->baser = modrm->m;
      break;
  }

  return (op->len);
}


