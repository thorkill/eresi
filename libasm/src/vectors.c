/**
 * @defgroup libasm_engine libasm Engine.
 */
/**
 * $Id: vectors.c,v 1.13 2007/10/14 00:01:41 heroine Exp $
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
#if 0 && WIP
int	asm_fetch_default(asm_instr *ins, u_char *opcode, u_int len, 
			  asm_processor *proc, int opt)
{
  int	to_ret;
  LIBASM_PROFILE_FIN();
#if LIBASM_ENABLE_SPARC
  if (proc->type == ASM_PROC_SPARC)
    to_ret = asm_sparc_illegal(ins, opcode, len, proc);
  else	
#endif
    to_ret = -1;
  LIBASM_PROFILE_FOUT(to_ret);
}
#else
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
#endif

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
#if WIP
int	asm_operand_fetch_default(asm_operand *op, u_char *opcode, int otype,
				  asm_instr *ins, int opt)
{
  LIBASM_PROFILE_FIN();
  LIBASM_PROFILE_FOUT(-1);
}
#else
int	asm_operand_fetch_default(asm_operand *op, u_char *opcode, int otype,
				  asm_instr *ins)
{
  LIBASM_PROFILE_FIN();
  LIBASM_PROFILE_FOUT(-1);
}
#endif

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
