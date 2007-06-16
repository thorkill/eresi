/**
 * $Id: vectors.c,v 1.11 2007-06-16 20:24:25 strauss Exp $
 * @file vectors.c
 *  Initialize the instruction and opcode vectors.
 */

#include <libasm.h>

/**
 * Default handler for the disasm vector.
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
  if (proc->type == ASM_PROC_SPARC)
    to_ret = asm_sparc_illegal(ins, opcode, len, proc);
  else	
    to_ret = -1;
  LIBASM_PROFILE_FOUT(to_ret);
}

/**
 * Default handler for the operand vector
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
 * Intialize the disasm and operand vectors.
 * @param proc Pointer to the processor structure
 * @return 1 on success, 0 on error
 */
int	asm_init_vectors(asm_processor *proc)
{
  u_int	*dims;
  char	**dimstr;
  int	to_ret;
  
  LIBASM_PROFILE_IN();
  aspect_init();
  to_ret = -1;
 
  if (proc->type == ASM_PROC_IA32) {
    /* Initializing IA-32 instruction handler vector */
    dims = malloc(1 * sizeof (u_int));
    if (!dims)
    {
      to_ret = 0;
      goto out;
    }
    dimstr = malloc(1 * sizeof (char *));
    if (!dimstr)
    {
      to_ret = 0;
      goto out;
    }
  
    dims[0] = 512;
  
    dimstr[0] = "OPCODES";

    aspect_register_vector("disasm", asm_fetch_default,
                    			 dims, dimstr, 1, ASPECT_TYPE_CADDR);

    /* Initializing IA-32 operand handler vector */
    dims = malloc(1 * sizeof (u_int));

    if (!dims)
    {
      to_ret = 0;
      goto out;
    }
    dimstr = malloc(1 * sizeof (char *));
    if (!dimstr)
    {
      to_ret = 0;
      goto out;
    }
  
    dims[0] = ASM_OTYPE_NUM;

    dimstr[0] = "OPERAND";
  
    aspect_register_vector("operand",  asm_operand_fetch_default,
                     			 dims, dimstr, 1, ASPECT_TYPE_CADDR);
  }

  else if (proc->type == ASM_PROC_SPARC) {
    /* Initializing SPARC instruction handler vector */
    dims = malloc(3 * sizeof (u_int));
    if (!dims)
    {
      to_ret = 0;
      goto out;
    }
    dimstr = malloc(3 * sizeof (char *));
    if (!dimstr)
    {
      to_ret = 0;
      goto out;
    }
  
    dims[0] = 4;
    dims[1] = 64;
    dims[2] = 32;
  
    dimstr[0] = "OPCODES";
    dimstr[1] = "SECONDARY OPCODES"; /* Should be 0 when unused */
    dimstr[2] = "FPOP OPCODE"; /* Should be 0 when unused */

    aspect_register_vector("disasm-sparc", asm_fetch_default,
                    			 dims, dimstr, 3, ASPECT_TYPE_CADDR);

    /* Initializing SPARC operand handler vector */
    dims = malloc(1 * sizeof (u_int));

    if (!dims)
    {
      to_ret = 0;
      goto out;
    }
    dimstr = malloc(1 * sizeof (char *));
    if (!dimstr)
    {
      to_ret = 0;
      goto out;
    }
  
    dims[0] = ASM_SP_OTYPE_NUM;

    dimstr[0] = "OPERAND";
  
    aspect_register_vector("operand-sparc",  asm_operand_fetch_default,
                     			 dims, dimstr, 1, ASPECT_TYPE_CADDR);
  }
                           
  config_add_item(ASM_CONFIG_ENDIAN_FLAG,
            		  CONFIG_TYPE_INT,
            		  CONFIG_MODE_RW,
            		  (void *)ASM_CONFIG_LITTLE_ENDIAN);

  config_add_item(ASM_CONFIG_ATT_MARGIN_FLAG,
            		  CONFIG_TYPE_INT,
            		  CONFIG_MODE_RW,
            		  (void *)ASM_CONFIG_ATT_MARGIN_DEFAULT);
 
 out:
   LIBASM_PROFILE_FOUT(to_ret);
}


