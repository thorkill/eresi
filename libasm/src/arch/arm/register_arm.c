/**
 * @file register.c
 * @brief 
 *
 */

#include <libasm.h>

/* Helper function prototypes */
static int asm_register_arm_instructions();
static int asm_register_arm_operands();

static int asm_register_arm_opcode(int type, int subtype,
                                   int instruction, unsigned long fcn);
static int asm_register_arm_operand(int operand_type, unsigned long fcn);

/**
 * Initialize arm vector.
 * @return Always 1.
 */
int     asm_register_arm()
{
  u_int         *dims;
  char          **dimstr;
  
  LIBASM_PROFILE_FIN();

  dims = malloc(3 * sizeof (u_int));
  if (!dims)
    {
      goto out;
    }
  dimstr = malloc(3 * sizeof (char *));
  if (!dimstr)
    {
      goto out;
    }
    
  dims[0] = 4;
  dims[1] = 8;
  dims[2] = 16;
    
  /* TODO: change these strings */
  dimstr[0] = "TYPE";
  dimstr[1] = "SUBTYPE";
  dimstr[2] = "INSTRUCTION";

  aspect_register_vector(LIBASM_VECTOR_OPCODE_ARM, asm_fetch_default,
			 dims, dimstr, 3, ASPECT_TYPE_CADDR);

  /* Initializing ARM operand handler vector */
  dims = malloc(1 * sizeof (u_int));

  if (!dims)
    {
      goto out;
    }
  dimstr = malloc(1 * sizeof (char *));
  if (!dimstr)
    {
      goto out;
    }
  
  dims[0] = ASM_ARM_OTYPE_NUM;

  dimstr[0] = "OPERAND";
  
  aspect_register_vector(LIBASM_VECTOR_OPERAND_ARM,  asm_operand_fetch_default,
			 dims, dimstr, 1, ASPECT_TYPE_CADDR);

  asm_register_arm_instructions();
  asm_register_arm_operands();
  
 out:
  LIBASM_PROFILE_FOUT(1);
}

/**
 * @brief Create the instruction vector
 * @param opcode First opcode
 * @param opcode2 Second opcode
 * @param fpop
 * @param fcn Default handler of the instruction vector.
 * @return 
 */
int     asm_register_arm_opcode(int type, int subtype, int instruction,
                                   			      unsigned long fcn)
{
  vector_t      *vec;
  u_int         dim[3];
  
  LIBASM_PROFILE_FIN();
  vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_ARM);
  dim[0] = type;
  dim[1] = subtype;
  dim[2] = instruction;

  aspect_vectors_insert(vec, dim, fcn);
  LIBASM_PROFILE_FOUT(1);
}

/**
 * @brief Register each instruction handler for the arm architecture.
 * @return 1 on success, or -1 on error
 */

int     asm_register_arm_instructions()
{
  LIBASM_PROFILE_FIN();

  // TODO: verify if a different function is needed for different forms of a
  // single instruction, such as MCR and MCR2.
  // If not, maybe it'll be better to remove the multiple entries used in
  // the opcode vector

  /* multiply */
  asm_register_arm_opcode(0x00, 0x01, 0x00, (unsigned long) asm_arm_mul);
  asm_register_arm_opcode(0x00, 0x01, 0x01, (unsigned long) asm_arm_mla);
  asm_register_arm_opcode(0x00, 0x01, 0x04, (unsigned long) asm_arm_umull);
  asm_register_arm_opcode(0x00, 0x01, 0x05, (unsigned long) asm_arm_umlal);
  asm_register_arm_opcode(0x00, 0x01, 0x06, (unsigned long) asm_arm_smull);
  asm_register_arm_opcode(0x00, 0x01, 0x07, (unsigned long) asm_arm_smlal);
  /* swap */
  asm_register_arm_opcode(0x00, 0x02, 0x00, (unsigned long) asm_arm_swp);
  asm_register_arm_opcode(0x00, 0x02, 0x01, (unsigned long) asm_arm_swpb);
  /* extra load/store */
  asm_register_arm_opcode(0x00, 0x03, 0x01, (unsigned long) asm_arm_strh);
  asm_register_arm_opcode(0x00, 0x03, 0x02, (unsigned long) asm_arm_ldrd);
  asm_register_arm_opcode(0x00, 0x03, 0x03, (unsigned long) asm_arm_strd);
  asm_register_arm_opcode(0x00, 0x03, 0x05, (unsigned long) asm_arm_ldrh);
  asm_register_arm_opcode(0x00, 0x03, 0x06, (unsigned long) asm_arm_ldrsb);
  asm_register_arm_opcode(0x00, 0x03, 0x07, (unsigned long) asm_arm_ldrsh);
  /* miscellaneous */
  /* .. dsp multiply */
  asm_register_arm_opcode(0x00, 0x04, 0x00, (unsigned long) asm_arm_smlaxy);
  asm_register_arm_opcode(0x00, 0x04, 0x01, (unsigned long) asm_arm_smlawy);
  asm_register_arm_opcode(0x00, 0x04, 0x02, (unsigned long) asm_arm_smlalxy);
  asm_register_arm_opcode(0x00, 0x04, 0x03, (unsigned long) asm_arm_smulxy);
  asm_register_arm_opcode(0x00, 0x04, 0x05, (unsigned long) asm_arm_smulwy);
  /* .. dsp add/sub */
  asm_register_arm_opcode(0x00, 0x05, 0x00, (unsigned long) asm_arm_qadd);
  asm_register_arm_opcode(0x00, 0x05, 0x01, (unsigned long) asm_arm_qsub);
  asm_register_arm_opcode(0x00, 0x05, 0x02, (unsigned long) asm_arm_qdadd);
  asm_register_arm_opcode(0x00, 0x05, 0x03, (unsigned long) asm_arm_qdsub);
  /* .. other */
  /* ... status register manipulation */
  asm_register_arm_opcode(0x00, 0x06, 0x00, (unsigned long) asm_arm_mrs);
  asm_register_arm_opcode(0x00, 0x00, 0x00, (unsigned long) asm_arm_msr); /* MSR(1) */
  asm_register_arm_opcode(0x00, 0x06, 0x02, (unsigned long) asm_arm_msr); /* MSR(2) */

  asm_register_arm_opcode(0x00, 0x06, 0x03, (unsigned long) asm_arm_bx);
  asm_register_arm_opcode(0x00, 0x06, 0x07, (unsigned long) asm_arm_clz);
  asm_register_arm_opcode(0x00, 0x06, 0x05, (unsigned long) asm_arm_blx2); /* BLX(2) */
  asm_register_arm_opcode(0x00, 0x06, 0x09, (unsigned long) asm_arm_bkpt);
  /* data processing */
  asm_register_arm_opcode(0x00, 0x07, 0x00, (unsigned long) asm_arm_and);
  asm_register_arm_opcode(0x00, 0x07, 0x01, (unsigned long) asm_arm_eor);
  asm_register_arm_opcode(0x00, 0x07, 0x02, (unsigned long) asm_arm_sub);
  asm_register_arm_opcode(0x00, 0x07, 0x03, (unsigned long) asm_arm_rsb);
  asm_register_arm_opcode(0x00, 0x07, 0x04, (unsigned long) asm_arm_add);
  asm_register_arm_opcode(0x00, 0x07, 0x05, (unsigned long) asm_arm_adc);
  asm_register_arm_opcode(0x00, 0x07, 0x06, (unsigned long) asm_arm_sbc);
  asm_register_arm_opcode(0x00, 0x07, 0x07, (unsigned long) asm_arm_rsc);
  asm_register_arm_opcode(0x00, 0x07, 0x08, (unsigned long) asm_arm_tst);
  asm_register_arm_opcode(0x00, 0x07, 0x09, (unsigned long) asm_arm_teq);
  asm_register_arm_opcode(0x00, 0x07, 0x0A, (unsigned long) asm_arm_cmp);
  asm_register_arm_opcode(0x00, 0x07, 0x0B, (unsigned long) asm_arm_cmn);
  asm_register_arm_opcode(0x00, 0x07, 0x0C, (unsigned long) asm_arm_orr);
  asm_register_arm_opcode(0x00, 0x07, 0x0D, (unsigned long) asm_arm_mov);
  asm_register_arm_opcode(0x00, 0x07, 0x0E, (unsigned long) asm_arm_bic);
  asm_register_arm_opcode(0x00, 0x07, 0x0F, (unsigned long) asm_arm_mvn);

  /* load/store */
  asm_register_arm_opcode(0x01, 0x00, 0x00, (unsigned long) asm_arm_str);
  asm_register_arm_opcode(0x01, 0x00, 0x01, (unsigned long) asm_arm_strt);
  asm_register_arm_opcode(0x01, 0x00, 0x02, (unsigned long) asm_arm_strb);
  asm_register_arm_opcode(0x01, 0x00, 0x03, (unsigned long) asm_arm_strbt);
  asm_register_arm_opcode(0x01, 0x01, 0x00, (unsigned long) asm_arm_ldr);
  asm_register_arm_opcode(0x01, 0x01, 0x01, (unsigned long) asm_arm_ldrt);
  asm_register_arm_opcode(0x01, 0x01, 0x02, (unsigned long) asm_arm_ldrb);
  asm_register_arm_opcode(0x01, 0x01, 0x03, (unsigned long) asm_arm_ldrbt);
  /* PLD */
  asm_register_arm_opcode(0x01, 0x02, 0x00, (unsigned long) asm_arm_pld);

  /* load/store multiple */
  asm_register_arm_opcode(0x02, 0x00, 0x00, (unsigned long) asm_arm_stm); // first form
  asm_register_arm_opcode(0x02, 0x00, 0x02, (unsigned long) asm_arm_ldm); // first form
  asm_register_arm_opcode(0x02, 0x00, 0x04, (unsigned long) asm_arm_stm); // second form
  asm_register_arm_opcode(0x02, 0x00, 0x06, (unsigned long) asm_arm_ldm); // second form
  asm_register_arm_opcode(0x02, 0x00, 0x07, (unsigned long) asm_arm_ldm); // third form
  /* branch */
  asm_register_arm_opcode(0x02, 0x01, 0x00, (unsigned long) asm_arm_b);
  asm_register_arm_opcode(0x02, 0x01, 0x01, (unsigned long) asm_arm_bl);
  asm_register_arm_opcode(0x02, 0x01, 0x02, (unsigned long) asm_arm_blx1); /* BLX(1) */

  /* coprocessor */
  asm_register_arm_opcode(0x03, 0x00, 0x00, (unsigned long) asm_arm_mcrr);
  asm_register_arm_opcode(0x03, 0x00, 0x01, (unsigned long) asm_arm_mrrc);
  asm_register_arm_opcode(0x03, 0x01, 0x00, (unsigned long) asm_arm_stc);
  asm_register_arm_opcode(0x03, 0x01, 0x01, (unsigned long) asm_arm_ldc);
  asm_register_arm_opcode(0x03, 0x01, 0x02, (unsigned long) asm_arm_stc); /* STC2 */
  asm_register_arm_opcode(0x03, 0x01, 0x03, (unsigned long) asm_arm_ldc); /* LDC2 */
  asm_register_arm_opcode(0x03, 0x02, 0x00, (unsigned long) asm_arm_cdp);
  asm_register_arm_opcode(0x03, 0x02, 0x01, (unsigned long) asm_arm_cdp); /* CDP2 */
  asm_register_arm_opcode(0x03, 0x03, 0x00, (unsigned long) asm_arm_mcr);
  asm_register_arm_opcode(0x03, 0x03, 0x01, (unsigned long) asm_arm_mrc);
  asm_register_arm_opcode(0x03, 0x03, 0x02, (unsigned long) asm_arm_mcr); /* MCR2 */
  asm_register_arm_opcode(0x03, 0x03, 0x03, (unsigned long) asm_arm_mrc); /* MRC2 */
  /* exception */
  asm_register_arm_opcode(0x03, 0x04, 0x00, (unsigned long) asm_arm_swi);

  LIBASM_PROFILE_FOUT(1);
}

/**
 * Register an handler for an ARM operand content.
 * @param operand_type Content of the operand to register
 * @param fcn Function pointer.
 * @return 1 on success, 0 on error.
 */
int asm_register_arm_operand(int operand_type, unsigned long fcn)
{
  LIBASM_PROFILE_FIN();
  asm_register_operand(LIBASM_VECTOR_OPERAND_ARM, operand_type, fcn);
  LIBASM_PROFILE_FOUT(1);
}

/**
 * Register operands.
 *
 */
int asm_register_arm_operands()
{
  asm_register_arm_operand(ASM_ARM_OTYPE_REGISTER, (unsigned long) asm_arm_op_fetch_register);
  asm_register_arm_operand(ASM_ARM_OTYPE_IMMEDIATE, (unsigned long) asm_arm_op_fetch_immediate);
  asm_register_arm_operand(ASM_ARM_OTYPE_REG_SCALED, (unsigned long) asm_arm_op_fetch_reg_scaled);
  asm_register_arm_operand(ASM_ARM_OTYPE_REG_OFFSET, (unsigned long) asm_arm_op_fetch_reg_offset);
  asm_register_arm_operand(ASM_ARM_OTYPE_REG_LIST, (unsigned long) asm_arm_op_fetch_reg_list);
  asm_register_arm_operand(ASM_ARM_OTYPE_DISP, (unsigned long) asm_arm_op_fetch_disp);
  asm_register_arm_operand(ASM_ARM_OTYPE_DISP_HALF, (unsigned long) asm_arm_op_fetch_disp_half);
  /* WIP */

  return (1);
}
