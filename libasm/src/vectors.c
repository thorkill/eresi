/**
 * @defgroup libasm_engine libasm Engine.
 */
/**
 * $Id$
 * @file vectors.c
 * @ingroup libasm_engine
 * @brief Initialization the instruction and opcode vectors.
 */

#include <libasm.h>

/**
 * @brief Default handler for the disasm vector.
 * This handler does nothing, simply returning -1.
 * @param ins Pointer to instruction structure to fill
 * @param opcode Pointer to buffer to disassemble
 * @param len Length of the buffer to disassemble
 * @param proc Pointer to the processor structure
 * @return -1
 */
int	asm_fetch_default(asm_instr *ins, u_char *opcode, u_int len, 
			  asm_processor *proc)
{
  int	to_ret;
  LIBASM_PROFILE_FIN();
#if LIBASM_ENABLE_SPARC
  if (proc->type == ASM_PROC_SPARC)
    to_ret = asm_sparc_illegal(ins, opcode, len, proc);
  else	
    to_ret = -1;
#endif
  LIBASM_PROFILE_FOUT(to_ret);
}

/**
 * @brief Default handler for the operand vector
 * This handler does nothing, simply returning -1
 * @param ins Pointer to instruction structure to fill
 * @param opcode Pointer to buffer to disassemble
 * @param len Length of the buffer to disassemble
 * @param proc Pointer to the processor structure
 * @return -1
 *
 */
int	asm_operand_fetch_default(asm_operand *op, u_char *opcode, int otype,
				  asm_instr *ins)
{
  LIBASM_PROFILE_FIN();
  LIBASM_PROFILE_FOUT(-1);
}

/**
 * @brief Return handler associated with an opcode
 * @param vector_name Name of the vector.
 * @param opcode
 *
 */
void	*asm_opcode_fetch(const char *vector_name, int opcode)
{
  vector_t	*vec;
  u_int		dim[1];
  void		*fcn_ptr;

  vec = aspect_vector_get((char *) vector_name);
  dim[0] = opcode;
  fcn_ptr = aspect_vectors_select(vec, dim);
  return (fcn_ptr);
}
