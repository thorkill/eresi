/**
 * @file register_mips.c
 * $Id: register_mips.c,v 1.1 2007-10-30 17:23:31 heroine Exp $
 * Manuel Martin - 2007 
 * This file contains mips architecture registration code.
*/

#include <libasm.h>

/**
 * Register a mips opcode handler.
 * @param op1
 * @param op2
 * @param op3
 * @param fcn
 * @return Always 1
 */

int asm_register_mips_opcode(int op1,int op2,int op3, unsigned long fcn)
{
   vector_t *vec;
   u_int dim[3];

   LIBASM_PROFILE_FIN();

   vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_MIPS);
   dim[0] = op1;
   dim[1] = op2;
   dim[2] = op3;

   aspect_vectors_insert(vec,dim,fcn);
   LIBASM_PROFILE_FOUT(1);
}

/**
 * Register mips opcode handlers.
 * @return 1 on success
 */
int asm_register_mips_opcodes()
{
  int i = 0;
  struct e_mips_instr	*insns;

  /**
   * @todo This is supposed to be working this way.... Check this
   */
  insns = e_mips_instrs;
  for(i=0;insns[i].code != ASM_MIPS_TABLE_END;i++)
    {
      asm_register_mips_opcode(insns[i].index1,insns[i].index2,insns[i].index3, NULL);
    }
  return (1);
}

/**
 *
 *
 */
int	asm_register_mips(asm_processor *proc, int opt)
{
  asm_register_mips_opcodes();
  asm_register_mips_operands();
}

/**
 * Register mips operand handlers.
 * This is currently not implemented.
 */
void asm_register_mips_operands(asm_processor *proc)
{
   return;
}

/**
 * Register the mips vector handlers.
 * @param proc Pointer to the asm processor structure.
 */
void asm_register_mips_arch(asm_processor *proc)
{
   asm_register_mips_operands(proc);
   asm_register_mips_opcodes();
}
