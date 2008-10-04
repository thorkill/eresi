/**
 * @file register.c
 * @brief 
 *
 */

#include <libasm.h>
#include <libasm-int.h>

int asm_register_sparc_instructions();
int asm_register_sparc_operands();

/**
 * Initialize sparc vector.
 * @return Always 1.
 */
int	asm_register_sparc()
{
  unsigned int	*dims;
  char **dimstr;
  int	to_ret;
  
  dims = malloc(3 * sizeof (unsigned int));
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
  dimstr[2] = "FPOP2 OPCODE (OPF_LOW)"; /* Should be 0 when unused */

  aspect_register_vector(LIBASM_VECTOR_OPCODE_SPARC, asm_fetch_default,
			 dims, dimstr, 3, ASPECT_TYPE_CADDR);

  /* Initializing SPARC operand handler vector */
  dims = malloc(1 * sizeof (unsigned int));

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
  
  aspect_register_vector(LIBASM_VECTOR_OPERAND_SPARC,  asm_operand_fetch_default,
			 dims, dimstr, 1, ASPECT_TYPE_CADDR);

  asm_register_sparc_instructions();
  asm_register_sparc_operands();
  
  out:
    return (1);
}

/**
 * @brief Create the instruction vector
 * @param opcode First opcode
 * @param opcode2 Second opcode
 * @param fpop
 * @param fcn Default handler of the instruction vector.
 * @return 
 */
int asm_register_sparc_opcode(int opcode, int opcode2, int fpop,
                                   			      unsigned long fcn)
{
  vector_t	*vec;
  unsigned int		dim[3];
  
  LIBASM_PROFILE_FIN();
  vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_SPARC);
  dim[0] = opcode;
  dim[1] = opcode2;
  dim[2] = fpop;

  aspect_vectors_insert(vec, dim, fcn);
  LIBASM_PROFILE_FOUT(1);
}

/**
 * @brief Register each instruction handler for the sparc architecture.
 * @return 1 on success, or -1 on error
 */

int asm_register_sparc_instructions()
{
  asm_register_sparc_opcode( 0x02, 0x00, 0x00, (unsigned long) asm_sparc_add);
  asm_register_sparc_opcode( 0x02, 0x08, 0x00, (unsigned long) asm_sparc_addc);
  asm_register_sparc_opcode( 0x02, 0x10, 0x00, (unsigned long) asm_sparc_addcc);
  asm_register_sparc_opcode( 0x02, 0x18, 0x00, (unsigned long) asm_sparc_addccc);
  asm_register_sparc_opcode( 0x02, 0x01, 0x00, (unsigned long) asm_sparc_and);
  asm_register_sparc_opcode( 0x02, 0x11, 0x00, (unsigned long) asm_sparc_andcc);
  asm_register_sparc_opcode( 0x02, 0x05, 0x00, (unsigned long) asm_sparc_andn);
  asm_register_sparc_opcode( 0x02, 0x15, 0x00, (unsigned long) asm_sparc_andncc);
  asm_register_sparc_opcode( 0x00, 0x02, 0x00, (unsigned long) asm_sparc_bicc);
  asm_register_sparc_opcode( 0x00, 0x01, 0x00, (unsigned long) asm_sparc_bpcc);
  asm_register_sparc_opcode( 0x00, 0x03, 0x00, (unsigned long) asm_sparc_bpr);
  asm_register_sparc_opcode( 0x01, 0x00, 0x00, (unsigned long) asm_sparc_call);
  asm_register_sparc_opcode( 0x03, 0x3c, 0x00, (unsigned long) asm_sparc_casa);
  asm_register_sparc_opcode( 0x03, 0x3e, 0x00, (unsigned long) asm_sparc_casxa);
  asm_register_sparc_opcode( 0x02, 0x3e, 0x00, (unsigned long) asm_sparc_done);
  asm_register_sparc_opcode( 0x00, 0x06, 0x00, (unsigned long) asm_sparc_fbfcc);
  asm_register_sparc_opcode( 0x00, 0x05, 0x00, (unsigned long) asm_sparc_fbpfcc);
  asm_register_sparc_opcode( 0x02, 0x35, 0x12, (unsigned long) asm_sparc_fcmpd);
  asm_register_sparc_opcode( 0x02, 0x35, 0x16, (unsigned long) asm_sparc_fcmped);
  asm_register_sparc_opcode( 0x02, 0x35, 0x17, (unsigned long) asm_sparc_fcmpeq);
  asm_register_sparc_opcode( 0x02, 0x35, 0x15, (unsigned long) asm_sparc_fcmpes);
  asm_register_sparc_opcode( 0x02, 0x35, 0x13, (unsigned long) asm_sparc_fcmpq);
  asm_register_sparc_opcode( 0x02, 0x35, 0x11, (unsigned long) asm_sparc_fcmps);
  asm_register_sparc_opcode( 0x02, 0x3b, 0x00, (unsigned long) asm_sparc_flush);
  asm_register_sparc_opcode( 0x02, 0x2b, 0x00, (unsigned long) asm_sparc_flushw);
  asm_register_sparc_opcode( 0x02, 0x35, 0x02, (unsigned long) asm_sparc_fmovdcc);
  asm_register_sparc_opcode( 0x02, 0x35, 0x06, (unsigned long) asm_sparc_fmovdr);
  asm_register_sparc_opcode( 0x02, 0x35, 0x03, (unsigned long) asm_sparc_fmovqcc);
  asm_register_sparc_opcode( 0x02, 0x35, 0x07, (unsigned long) asm_sparc_fmovqr);
  asm_register_sparc_opcode( 0x02, 0x35, 0x01, (unsigned long) asm_sparc_fmovscc);
  asm_register_sparc_opcode( 0x02, 0x35, 0x05, (unsigned long) asm_sparc_fmovsr);
  asm_register_sparc_opcode( 0x02, 0x34, 0x00, (unsigned long) asm_sparc_fpop1);
  asm_register_sparc_opcode( 0x00, 0x00, 0x00, (unsigned long) asm_sparc_illtrap);
  asm_register_sparc_opcode( 0x02, 0x36, 0x00, (unsigned long) asm_sparc_impdep1);
  asm_register_sparc_opcode( 0x02, 0x37, 0x00, (unsigned long) asm_sparc_impdep2);
  asm_register_sparc_opcode( 0x02, 0x38, 0x00, (unsigned long) asm_sparc_jmpl);
  asm_register_sparc_opcode( 0x03, 0x03, 0x00, (unsigned long) asm_sparc_ldd);
  asm_register_sparc_opcode( 0x03, 0x13, 0x00, (unsigned long) asm_sparc_ldda);
  asm_register_sparc_opcode( 0x03, 0x23, 0x00, (unsigned long) asm_sparc_lddf);
  asm_register_sparc_opcode( 0x03, 0x33, 0x00, (unsigned long) asm_sparc_lddfa);
  asm_register_sparc_opcode( 0x03, 0x20, 0x00, (unsigned long) asm_sparc_ldf);
  asm_register_sparc_opcode( 0x03, 0x30, 0x00, (unsigned long) asm_sparc_ldfa);
  asm_register_sparc_opcode( 0x03, 0x21, 0x00, (unsigned long) asm_sparc_ldfsr);
  asm_register_sparc_opcode( 0x03, 0x22, 0x00, (unsigned long) asm_sparc_ldqf);
  asm_register_sparc_opcode( 0x03, 0x32, 0x00, (unsigned long) asm_sparc_ldqfa);
  asm_register_sparc_opcode( 0x03, 0x09, 0x00, (unsigned long) asm_sparc_ldsb);
  asm_register_sparc_opcode( 0x03, 0x19, 0x00, (unsigned long) asm_sparc_ldsba);
  asm_register_sparc_opcode( 0x03, 0x0a, 0x00, (unsigned long) asm_sparc_ldsh);
  asm_register_sparc_opcode( 0x03, 0x1a, 0x00, (unsigned long) asm_sparc_ldsha);
  asm_register_sparc_opcode( 0x03, 0x0d, 0x00, (unsigned long) asm_sparc_ldstub);
  asm_register_sparc_opcode( 0x03, 0x1d, 0x00, (unsigned long) asm_sparc_ldstuba);
  asm_register_sparc_opcode( 0x03, 0x08, 0x00, (unsigned long) asm_sparc_ldsw);
  asm_register_sparc_opcode( 0x03, 0x18, 0x00, (unsigned long) asm_sparc_ldswa);
  asm_register_sparc_opcode( 0x03, 0x01, 0x00, (unsigned long) asm_sparc_ldub);
  asm_register_sparc_opcode( 0x03, 0x11, 0x00, (unsigned long) asm_sparc_lduba);
  asm_register_sparc_opcode( 0x03, 0x02, 0x00, (unsigned long) asm_sparc_lduh);
  asm_register_sparc_opcode( 0x03, 0x12, 0x00, (unsigned long) asm_sparc_lduha);
  asm_register_sparc_opcode( 0x03, 0x00, 0x00, (unsigned long) asm_sparc_lduw);
  asm_register_sparc_opcode( 0x03, 0x10, 0x00, (unsigned long) asm_sparc_lduwa);
  asm_register_sparc_opcode( 0x03, 0x0b, 0x00, (unsigned long) asm_sparc_ldx);
  asm_register_sparc_opcode( 0x03, 0x1b, 0x00, (unsigned long) asm_sparc_ldxa);
  asm_register_sparc_opcode( 0x02, 0x2c, 0x00, (unsigned long) asm_sparc_movcc);
  asm_register_sparc_opcode( 0x02, 0x2f, 0x00, (unsigned long) asm_sparc_movr);
  asm_register_sparc_opcode( 0x02, 0x24, 0x00, (unsigned long) asm_sparc_mulscc);
  asm_register_sparc_opcode( 0x02, 0x09, 0x00, (unsigned long) asm_sparc_mulx);
  asm_register_sparc_opcode( 0x02, 0x02, 0x00, (unsigned long) asm_sparc_or);
  asm_register_sparc_opcode( 0x02, 0x12, 0x00, (unsigned long) asm_sparc_orcc);
  asm_register_sparc_opcode( 0x02, 0x06, 0x00, (unsigned long) asm_sparc_orn);
  asm_register_sparc_opcode( 0x02, 0x16, 0x00, (unsigned long) asm_sparc_orncc);
  asm_register_sparc_opcode( 0x02, 0x2e, 0x00, (unsigned long) asm_sparc_popc);
  asm_register_sparc_opcode( 0x03, 0x2d, 0x00, (unsigned long) asm_sparc_prefetch);
  asm_register_sparc_opcode( 0x03, 0x3d, 0x00, (unsigned long) asm_sparc_prefetcha);
  asm_register_sparc_opcode( 0x02, 0x28, 0x00, (unsigned long) asm_sparc_rd);
  asm_register_sparc_opcode( 0x02, 0x2a, 0x00, (unsigned long) asm_sparc_rdpr);
  asm_register_sparc_opcode( 0x02, 0x3d, 0x00, (unsigned long) asm_sparc_restore);
  asm_register_sparc_opcode( 0x02, 0x39, 0x00, (unsigned long) asm_sparc_return);
  asm_register_sparc_opcode( 0x02, 0x3c, 0x00, (unsigned long) asm_sparc_save);
  asm_register_sparc_opcode( 0x02, 0x31, 0x00, (unsigned long) asm_sparc_saved);
  asm_register_sparc_opcode( 0x02, 0x0f, 0x00, (unsigned long) asm_sparc_sdiv);
  asm_register_sparc_opcode( 0x02, 0x1f, 0x00, (unsigned long) asm_sparc_sdivcc);
  asm_register_sparc_opcode( 0x02, 0x2d, 0x00, (unsigned long) asm_sparc_sdivx);
  asm_register_sparc_opcode( 0x00, 0x04, 0x00, (unsigned long) asm_sparc_sethi);
  asm_register_sparc_opcode( 0x02, 0x25, 0x00, (unsigned long) asm_sparc_sll);
  asm_register_sparc_opcode( 0x02, 0x0b, 0x00, (unsigned long) asm_sparc_smul);
  asm_register_sparc_opcode( 0x02, 0x1b, 0x00, (unsigned long) asm_sparc_smulcc);
  asm_register_sparc_opcode( 0x02, 0x27, 0x00, (unsigned long) asm_sparc_sra);
  asm_register_sparc_opcode( 0x02, 0x26, 0x00, (unsigned long) asm_sparc_srl);
  asm_register_sparc_opcode( 0x03, 0x05, 0x00, (unsigned long) asm_sparc_stb);
  asm_register_sparc_opcode( 0x03, 0x15, 0x00, (unsigned long) asm_sparc_stba);
  asm_register_sparc_opcode( 0x03, 0x07, 0x00, (unsigned long) asm_sparc_std);
  asm_register_sparc_opcode( 0x03, 0x17, 0x00, (unsigned long) asm_sparc_stda);
  asm_register_sparc_opcode( 0x03, 0x27, 0x00, (unsigned long) asm_sparc_stdf);
  asm_register_sparc_opcode( 0x03, 0x37, 0x00, (unsigned long) asm_sparc_stdfa);
  asm_register_sparc_opcode( 0x03, 0x24, 0x00, (unsigned long) asm_sparc_stf);
  asm_register_sparc_opcode( 0x03, 0x34, 0x00, (unsigned long) asm_sparc_stfa);
  asm_register_sparc_opcode( 0x03, 0x25, 0x00, (unsigned long) asm_sparc_stfsr);
  asm_register_sparc_opcode( 0x03, 0x06, 0x00, (unsigned long) asm_sparc_sth);
  asm_register_sparc_opcode( 0x03, 0x16, 0x00, (unsigned long) asm_sparc_stha);
  asm_register_sparc_opcode( 0x03, 0x26, 0x00, (unsigned long) asm_sparc_stqf);
  asm_register_sparc_opcode( 0x03, 0x36, 0x00, (unsigned long) asm_sparc_stqfa);
  asm_register_sparc_opcode( 0x03, 0x04, 0x00, (unsigned long) asm_sparc_stw);
  asm_register_sparc_opcode( 0x03, 0x14, 0x00, (unsigned long) asm_sparc_stwa);
  asm_register_sparc_opcode( 0x03, 0x0e, 0x00, (unsigned long) asm_sparc_stx);
  asm_register_sparc_opcode( 0x03, 0x1e, 0x00, (unsigned long) asm_sparc_stxa);
  asm_register_sparc_opcode( 0x02, 0x04, 0x00, (unsigned long) asm_sparc_sub);
  asm_register_sparc_opcode( 0x02, 0x0c, 0x00, (unsigned long) asm_sparc_subc);
  asm_register_sparc_opcode( 0x02, 0x14, 0x00, (unsigned long) asm_sparc_subcc);
  asm_register_sparc_opcode( 0x02, 0x1c, 0x00, (unsigned long) asm_sparc_subccc);
  asm_register_sparc_opcode( 0x03, 0x0f, 0x00, (unsigned long) asm_sparc_swap);
  asm_register_sparc_opcode( 0x03, 0x1f, 0x00, (unsigned long) asm_sparc_swapa);
  asm_register_sparc_opcode( 0x02, 0x20, 0x00, (unsigned long) asm_sparc_taddcc);
  asm_register_sparc_opcode( 0x02, 0x22, 0x00, (unsigned long) asm_sparc_taddcctv);
  asm_register_sparc_opcode( 0x02, 0x3a, 0x00, (unsigned long) asm_sparc_tcc);
  asm_register_sparc_opcode( 0x02, 0x21, 0x00, (unsigned long) asm_sparc_tsubcc);
  asm_register_sparc_opcode( 0x02, 0x23, 0x00, (unsigned long) asm_sparc_tsubcctv);
  asm_register_sparc_opcode( 0x02, 0x0e, 0x00, (unsigned long) asm_sparc_udiv);
  asm_register_sparc_opcode( 0x02, 0x1e, 0x00, (unsigned long) asm_sparc_udivcc);
  asm_register_sparc_opcode( 0x02, 0x0d, 0x00, (unsigned long) asm_sparc_udivx);
  asm_register_sparc_opcode( 0x02, 0x0a, 0x00, (unsigned long) asm_sparc_umul);
  asm_register_sparc_opcode( 0x02, 0x1a, 0x00, (unsigned long) asm_sparc_umulcc);
  asm_register_sparc_opcode( 0x02, 0x30, 0x00, (unsigned long) asm_sparc_wr);
  asm_register_sparc_opcode( 0x02, 0x32, 0x00, (unsigned long) asm_sparc_wrpr);
  asm_register_sparc_opcode( 0x02, 0x07, 0x00, (unsigned long) asm_sparc_xnor);
  asm_register_sparc_opcode( 0x02, 0x17, 0x00, (unsigned long) asm_sparc_xnorcc);
  asm_register_sparc_opcode( 0x02, 0x03, 0x00, (unsigned long) asm_sparc_xor);
  asm_register_sparc_opcode( 0x02, 0x13, 0x00, (unsigned long) asm_sparc_xorcc);  	
  return (1);
}

/**
 * Register an handler for a sparc operand content.
 * @param operand_type Content of the operand to register
 * @param fcn Function pointer.
 * @return 1 on success, 0 on error.
 */

int asm_register_sparc_operand(int operand_type, unsigned long fcn)
{
  LIBASM_PROFILE_FIN();
  asm_register_operand(LIBASM_VECTOR_OPERAND_SPARC, operand_type, fcn);
  LIBASM_PROFILE_FOUT(1);
}

/**
 * Register operands.
 *
 */

int asm_register_sparc_operands()
{
  asm_register_sparc_operand(ASM_SP_OTYPE_REGISTER, (unsigned long) asm_sparc_op_fetch_register);
  asm_register_sparc_operand(ASM_SP_OTYPE_IMMEDIATE, (unsigned long) asm_sparc_op_fetch_immediate);
  asm_register_sparc_operand(ASM_SP_OTYPE_DISPLACEMENT, (unsigned long) asm_sparc_op_fetch_displacement);
  asm_register_sparc_operand(ASM_SP_OTYPE_DISP30, (unsigned long) asm_sparc_op_fetch_disp30);
  asm_register_sparc_operand(ASM_SP_OTYPE_SETHI, (unsigned long) asm_sparc_op_fetch_sethi);
  asm_register_sparc_operand(ASM_SP_OTYPE_FREGISTER, (unsigned long) asm_sparc_op_fetch_fregister);
  asm_register_sparc_operand(ASM_SP_OTYPE_SREGISTER, (unsigned long) asm_sparc_op_fetch_sregister);
  asm_register_sparc_operand(ASM_SP_OTYPE_PREGISTER, (unsigned long) asm_sparc_op_fetch_pregister);
  asm_register_sparc_operand(ASM_SP_OTYPE_CC, (unsigned long) asm_sparc_op_fetch_cc);
  asm_register_sparc_operand(ASM_SP_OTYPE_IMM_ADDRESS, (unsigned long) asm_sparc_op_fetch_imm_address);
  asm_register_sparc_operand(ASM_SP_OTYPE_REG_ADDRESS, (unsigned long) asm_sparc_op_fetch_reg_address);
  return (1);
}
