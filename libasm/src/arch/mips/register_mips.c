/**
 * @file register_mips.c
 * @brief Registration of the MIPS disassembling vectors.
 * $Id: register_mips.c,v 1.3 2008/03/13 16:15:37 strauss & pi3 Exp $
 *
 * fix and fill
 *          - Adam 'pi3' Zabrocki
 *
 * Manuel Martin - 2007 
 * This file contains mips architecture registration code.
*/

#include <libasm.h>

/**
 * @fn int asm_register_mips_opcode(int op1,int op2,int op3, unsigned long fcn)
 * @brief Register MIPS opcode handler.
 *
 * This function is called by asm_register_mips_opcodes()
 *
 * @param op1 First argument of instruction type.
 * @param op2 Second argument of instruction type.
 * @param op3 Third argument of instruction type.
 * @param fcn Address for registered function.
 * @return Always 1
 */
int asm_register_mips_opcode(int op1,int op2,int op3, unsigned long fcn)
{
   vector_t *vec;
   unsigned int dim[3];

   LIBASM_PROFILE_FIN();

   vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_MIPS);
   dim[0] = op1;
   dim[1] = op2;
   dim[2] = op3;

   aspect_vectors_insert(vec,dim,fcn);
   LIBASM_PROFILE_FOUT(1);
}

/**
 * @fn int asm_register_mips_opcodes()
 * @brief Register MIPS opcode handlers.
 *
 * @return 1 on success
 */
int asm_register_mips_opcodes()
{
  int i = 0;
  struct e_mips_instr	*insns;

  insns = e_mips_instrs;
  for(i=0;insns[i].code != ASM_MIPS_TABLE_END;i++)
    {
      asm_register_mips_opcode(insns[i].index1,insns[i].index2,insns[i].index3, (unsigned long) insns[i].func_op);
    }
  return (1);
}

/**
 * @fn int asm_register_mips()
 * @brief Initialize the disassembling vector for MIPS.
 *
 * @return Always 1.
 */
int asm_register_mips()
{
  unsigned int	*dims;
  char  **dimstr;
  
  dims = malloc(3 * sizeof (unsigned int));
  if (!dims)
    {
      goto out;
    }
  dimstr = malloc(3 * sizeof (char *));
  if (!dimstr)
    {
      goto out;
    }
    
  /* TODO: These sizes _HAVE_ to be reviewed */
  dims[0] = 64;
  dims[1] = 64;
  dims[2] = 64;
    
  dimstr[0] = "OPCODES";
  dimstr[1] = "SECONDARY OPCODES"; /* Should be 0 when unused */
  dimstr[2] = "TERTIARY OPCODES"; /* Should be 0 when unused */

  aspect_register_vector(LIBASM_VECTOR_OPCODE_MIPS, asm_fetch_default,
			 dims, dimstr, 3, ASPECT_TYPE_CADDR);

  /* Initializing MIPS operand handler vector */
  /* This section is just a stub for when the operand vector is actually
  * implemented. */

  dims = malloc(1 * sizeof (unsigned int));

  if (!dims)
    {
      goto out;
    }
  dimstr = malloc(1 * sizeof (char *));
  if (!dimstr)
    {
      goto out;
    }
  
  dims[0] = ASM_MIPS_OTYPE_LAST;

  dimstr[0] = "OPERAND";
  
  aspect_register_vector(LIBASM_VECTOR_OPERAND_MIPS,  asm_operand_fetch_default,
			 dims, dimstr, 1, ASPECT_TYPE_CADDR);

  asm_register_mips_opcodes();
  asm_register_mips_operands();

  out:
    return (1);
}

/**
 * @fn int asm_register_mips()
 * @brief Register MIPS operand handlers.
 *
 * @return Always 1.
 */
int asm_register_mips_operands()
{
  asm_register_mips_operand(ASM_MIPS_OTYPE_NONE, (unsigned long) asm_mips_operand_none);
  asm_register_mips_operand(ASM_MIPS_OTYPE_REGISTER, (unsigned long) asm_mips_operand_r);
  asm_register_mips_operand(ASM_MIPS_OTYPE_IMMEDIATE, (unsigned long) asm_mips_operand_i);
  asm_register_mips_operand(ASM_MIPS_OTYPE_JUMP, (unsigned long) asm_mips_operand_j);
  asm_register_mips_operand(ASM_MIPS_OTYPE_NOOP, (unsigned long) asm_mips_operand_noop);
  asm_register_mips_operand(ASM_MIPS_OTYPE_BRANCH, (unsigned long) asm_mips_operand_branch);
  asm_register_mips_operand(ASM_MIPS_OTYPE_REGBASE, (unsigned long) asm_mips_operand_regbase);

  return 1;
}


/**
 * @fn int asm_register_mips_operand(unsigned int type, unsigned long func)
 * @brief Register MIPS operand handler.
 *
 * This function is called by asm_register_mips_operands()
 *
 * @param type Type of the operand to register.
 * @param func Function pointer.
 * @return 1 on success, 0 on error.
 */
int asm_register_mips_operand(unsigned int type, unsigned long func)
{
  LIBASM_PROFILE_FIN();
  asm_register_operand(LIBASM_VECTOR_OPERAND_MIPS, type, func);
  LIBASM_PROFILE_FOUT(1);
}
