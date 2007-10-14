/**
 * $Id: vectors.c,v 1.13 2007-10-14 00:01:41 heroine Exp $
 * @file vectors.c
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
  if (proc->type == ASM_PROC_SPARC)
    to_ret = asm_sparc_illegal(ins, opcode, len, proc);
  else	
    to_ret = -1;
  LIBASM_PROFILE_FOUT(to_ret);
}
#else
int	asm_fetch_default(asm_instr *ins, u_char *opcode, u_int len, 
			  asm_processor *proc)
{
  int	to_ret;
  LIBASM_PROFILE_FIN();
  if (proc->type == ASM_PROC_SPARC)
    to_ret = asm_sparc_illegal(ins, opcode, len, proc);
  else	
    to_ret = -1;
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
 * @brief Intialize the disasm and operand vectors.
 * @param proc Pointer to the processor structure
 * @return 1 on success, 0 on error
 */
int	asm_init_vectors(asm_processor *proc)
{
  int	to_ret;
  

  /**
   * XXX: This is probably dead code. Remove if exit/printf below is never run.
   */
  
  LIBASM_PROFILE_IN();
  aspect_init();
  to_ret = -1;
  
  config_add_item(ASM_CONFIG_ENDIAN_FLAG,
            		  CONFIG_TYPE_INT,
            		  CONFIG_MODE_RW,
            		  (void *)ASM_CONFIG_LITTLE_ENDIAN);

  config_add_item(ASM_CONFIG_ATT_MARGIN_FLAG,
            		  CONFIG_TYPE_INT,
            		  CONFIG_MODE_RW,
            		  (void *)ASM_CONFIG_ATT_MARGIN_DEFAULT);
 
  // out:
   LIBASM_PROFILE_FOUT(to_ret);
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
