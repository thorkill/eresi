/**
 * @file register.c
 * @ingroup libasm_ia32
 * @brief Registration of the ia32 disassembling vector.
 *
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Initialize the disassembling vector for ia32.
 * @return Always returns 1
 * @todo Add return error code.
 * prototype should be changed to (asm_processor *proc).
 * for error handling.
 */

int asm_register_ia32()
{
  static int	init_ok = 0;

  LIBASM_PROFILE_FIN();
  if (init_ok == 1)
    goto out;  
  asm_register_opcode_create(LIBASM_VECTOR_OPCODE_IA32, 512);
  asm_register_ia32_opcodes();
  
  asm_register_operand_create(LIBASM_VECTOR_OPERAND_IA32, ASM_CONTENT_NUM);
  asm_register_ia32_operands();
  
  init_ok = 1;
 out:
  LIBASM_PROFILE_FOUT(init_ok);
}

/**
 * Register an ia32 disassembly handler for an opcode
 * @param opcode opcode to raise new handler
 * @param fcn new handler.
 */
int	asm_register_ia32_opcode(int opcode, unsigned long fcn)
{
  LIBASM_PROFILE_FIN();
  asm_register_opcode(LIBASM_VECTOR_OPCODE_IA32, opcode, fcn);
  LIBASM_PROFILE_FOUT(1);
}

/**
 * Register an handler for a ia32 operand type.
 * @param operand_type Type of the operand to register
 * @param fcn Function pointer.
 * @return 1 on success, 0 on error.
 */

int	asm_register_ia32_operand(int operand_type, unsigned long fcn)
{
  LIBASM_PROFILE_FIN();
  asm_register_operand(LIBASM_VECTOR_OPERAND_IA32, operand_type, fcn);
  LIBASM_PROFILE_FOUT(1);
}

int	asm_register_ia32_operands()
{
  asm_register_ia32_operand(ASM_CONTENT_NONE, (unsigned long) asm_operand_fetch_default);
  asm_register_ia32_operand(ASM_CONTENT_FIXED, (unsigned long) asm_operand_fetch_fixed);
  asm_register_ia32_operand(ASM_CONTENT_OPMOD, (unsigned long) asm_operand_fetch_opmod);
  asm_register_ia32_operand(ASM_CONTENT_ADDRESS, (unsigned long) asm_operand_fetch_address);
  asm_register_ia32_operand(ASM_CONTENT_CONTROL, (unsigned long) asm_operand_fetch_control);
  asm_register_ia32_operand(ASM_CONTENT_DEBUG, (unsigned long) asm_operand_fetch_debug);
  asm_register_ia32_operand(ASM_CONTENT_ENCODED, (unsigned long) asm_operand_fetch_encoded);
  asm_register_ia32_operand(ASM_CONTENT_ENCODEDBYTE, (unsigned long) asm_operand_fetch_encodedbyte);
  asm_register_ia32_operand(ASM_CONTENT_FLAGS, (unsigned long) asm_operand_fetch_default);
  asm_register_ia32_operand(ASM_CONTENT_GENERAL, (unsigned long) asm_operand_fetch_general);
  asm_register_ia32_operand(ASM_CONTENT_GENERALBYTE, (unsigned long) asm_operand_fetch_generalbyte);
  asm_register_ia32_operand(ASM_CONTENT_IMMEDIATE, (unsigned long) asm_operand_fetch_immediate);
  asm_register_ia32_operand(ASM_CONTENT_IMMEDIATEWORD, (unsigned long) asm_operand_fetch_immediateword);
  asm_register_ia32_operand(ASM_CONTENT_IMMEDIATEBYTE, (unsigned long) asm_operand_fetch_immediatebyte);
  asm_register_ia32_operand(ASM_CONTENT_JUMP, (unsigned long) asm_operand_fetch_jump);
  asm_register_ia32_operand(ASM_CONTENT_SHORTJUMP, (unsigned long) asm_operand_fetch_shortjump);
  asm_register_ia32_operand(ASM_CONTENT_MEMORY, (unsigned long) asm_operand_fetch_memory);
  asm_register_ia32_operand(ASM_CONTENT_OFFSET, (unsigned long) asm_operand_fetch_offset);
  asm_register_ia32_operand(ASM_CONTENT_PMMX, (unsigned long) asm_operand_fetch_pmmx);
  asm_register_ia32_operand(ASM_CONTENT_QMMX, (unsigned long) asm_operand_fetch_default);
  asm_register_ia32_operand(ASM_CONTENT_REGISTER, (unsigned long) asm_operand_fetch_register);
  asm_register_ia32_operand(ASM_CONTENT_SEGMENT, (unsigned long) asm_operand_fetch_segment);
  asm_register_ia32_operand(ASM_CONTENT_XSRC, (unsigned long) asm_operand_fetch_xsrc);
  asm_register_ia32_operand(ASM_CONTENT_YDEST, (unsigned long) asm_operand_fetch_ydest);
  asm_register_ia32_operand(ASM_CONTENT_FPU, (unsigned long) asm_operand_fetch_fpu);
  asm_register_ia32_operand(ASM_CONTENT_VALUE, (unsigned long) asm_operand_fetch_default);
  return (1);
}

/**
 * @brief Register ia32 opcode handlers.
 * @return Always 1.
 */

int asm_register_ia32_opcodes()
{
  asm_register_ia32_opcode(0x00, (unsigned long) op_add_rmb_rb);
  asm_register_ia32_opcode(0x01, (unsigned long) op_add_rmv_rv);
  asm_register_ia32_opcode(0x02, (unsigned long) op_add_rb_rmb);
  asm_register_ia32_opcode(0x03, (unsigned long) op_add_rv_rmv);
  asm_register_ia32_opcode(0x04, (unsigned long) op_add_al_ib);
  asm_register_ia32_opcode(0x05, (unsigned long) op_add_eax_iv);
  asm_register_ia32_opcode(0x06, (unsigned long) op_push_es);
  asm_register_ia32_opcode(0x07, (unsigned long) op_pop_es);
  asm_register_ia32_opcode(0x08, (unsigned long) op_or_rmb_rb);
  asm_register_ia32_opcode(0x09, (unsigned long) op_or_rmv_rv);
  asm_register_ia32_opcode(0x0a, (unsigned long) op_or_rb_rmb);
  asm_register_ia32_opcode(0x0b, (unsigned long) op_or_rv_rmv);
  asm_register_ia32_opcode(0x0c, (unsigned long) op_or_al_ib);
  asm_register_ia32_opcode(0x0d, (unsigned long) op_or_eax_iv);
  asm_register_ia32_opcode(0x0e, (unsigned long) op_push_cs);
  asm_register_ia32_opcode(0x0f, (unsigned long) op_386sp);
  asm_register_ia32_opcode(0x10, (unsigned long) op_adc_rmb_rb);
  asm_register_ia32_opcode(0x11, (unsigned long) op_adc_rmv_rv);
  asm_register_ia32_opcode(0x12, (unsigned long) op_adc_rb_rmb);
  asm_register_ia32_opcode(0x13, (unsigned long) op_adc_rv_rmv);
  asm_register_ia32_opcode(0x14, (unsigned long) op_adc_al_ib);
  asm_register_ia32_opcode(0x15, (unsigned long) op_adc_eax_iv);
  asm_register_ia32_opcode(0x16, (unsigned long) op_push_ss);
  asm_register_ia32_opcode(0x17, (unsigned long) op_pop_es);
  asm_register_ia32_opcode(0x18, (unsigned long) op_sbb_rmb_rb);
  asm_register_ia32_opcode(0x19, (unsigned long) op_sbb_rmv_rv);
  asm_register_ia32_opcode(0x1a, (unsigned long) op_sbb_rb_rmb);
  asm_register_ia32_opcode(0x1b, (unsigned long) op_sbb_rv_rmv);
  asm_register_ia32_opcode(0x1c, (unsigned long) op_sbb_al_ib);
  asm_register_ia32_opcode(0x1d, (unsigned long) op_sbb_eax_iv);
  asm_register_ia32_opcode(0x1e, (unsigned long) op_push_ds);
  asm_register_ia32_opcode(0x1f, (unsigned long) op_pop_ds);
  asm_register_ia32_opcode(0x20, (unsigned long) op_and_rmb_rb);
  asm_register_ia32_opcode(0x21, (unsigned long) op_and_rmv_rv);
  asm_register_ia32_opcode(0x22, (unsigned long) op_and_rb_rmb);
  asm_register_ia32_opcode(0x23, (unsigned long) op_and_rv_rmv);
  asm_register_ia32_opcode(0x24, (unsigned long) op_and_al_ib);
  asm_register_ia32_opcode(0x25, (unsigned long) op_and_eax_iv);
  asm_register_ia32_opcode(0x26, (unsigned long) op_prefix_es);
  asm_register_ia32_opcode(0x27, (unsigned long) op_daa);
  asm_register_ia32_opcode(0x28, (unsigned long) op_sub_rmb_rb);
  asm_register_ia32_opcode(0x29, (unsigned long) op_sub_rmv_rv);
  asm_register_ia32_opcode(0x2a, (unsigned long) op_sub_rb_rmb);
  asm_register_ia32_opcode(0x2b, (unsigned long) op_sub_rv_rmv);
  asm_register_ia32_opcode(0x2c, (unsigned long) op_sub_al_ib);
  asm_register_ia32_opcode(0x2d, (unsigned long) op_sub_eax_iv);
  asm_register_ia32_opcode(0x2e, (unsigned long) op_prefix_cs);
  asm_register_ia32_opcode(0x2f, (unsigned long) op_das);
  asm_register_ia32_opcode(0x30, (unsigned long) op_xor_rmb_rb);
  asm_register_ia32_opcode(0x31, (unsigned long) op_xor_rmv_rv);
  asm_register_ia32_opcode(0x32, (unsigned long) op_xor_rb_rmb);
  asm_register_ia32_opcode(0x33, (unsigned long) op_xor_rv_rmv);
  asm_register_ia32_opcode(0x34, (unsigned long) op_xor_al_ib);
  asm_register_ia32_opcode(0x35, (unsigned long) op_xor_eax_iv);
  asm_register_ia32_opcode(0x36, (unsigned long) op_prefix_ss);
  asm_register_ia32_opcode(0x37, (unsigned long) op_aaa);
  asm_register_ia32_opcode(0x38, (unsigned long) op_cmp_rmb_rb);
  asm_register_ia32_opcode(0x39, (unsigned long) op_cmp_rmv_rv);
  asm_register_ia32_opcode(0x3a, (unsigned long) op_cmp_rb_rmb);
  asm_register_ia32_opcode(0x3b, (unsigned long) op_cmp_rv_rmv);
  asm_register_ia32_opcode(0x3c, (unsigned long) op_cmp_al_ib);
  asm_register_ia32_opcode(0x3d, (unsigned long) op_cmp_eax_iv);
  asm_register_ia32_opcode(0x3e, (unsigned long) op_prefix_ds);
  asm_register_ia32_opcode(0x3f, (unsigned long) op_aas);
  asm_register_ia32_opcode(0x40, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x41, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x42, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x43, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x44, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x45, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x46, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x47, (unsigned long) op_inc_reg);
  asm_register_ia32_opcode(0x48, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x49, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x4a, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x4b, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x4c, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x4d, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x4e, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x4f, (unsigned long) op_dec_reg);
  asm_register_ia32_opcode(0x50, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x51, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x52, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x53, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x54, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x55, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x56, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x57, (unsigned long) op_push_reg);
  asm_register_ia32_opcode(0x58, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x59, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x5a, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x5b, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x5c, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x5d, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x5e, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x5f, (unsigned long) op_pop_reg);
  asm_register_ia32_opcode(0x60, (unsigned long) op_pusha);
  asm_register_ia32_opcode(0x61, (unsigned long) op_popa);
  asm_register_ia32_opcode(0x62, (unsigned long) op_bound_gv_ma);
  asm_register_ia32_opcode(0x63, (unsigned long) op_arpl_ew_rw);
  asm_register_ia32_opcode(0x64, (unsigned long) op_prefix_fs);
  asm_register_ia32_opcode(0x65, (unsigned long) op_prefix_gs);
  asm_register_ia32_opcode(0x66, (unsigned long) op_opsize);
  asm_register_ia32_opcode(0x67, (unsigned long) op_addsize);
  asm_register_ia32_opcode(0x68, (unsigned long) op_push_iv);
  asm_register_ia32_opcode(0x69, (unsigned long) op_imul_rv_rmv_iv);
  asm_register_ia32_opcode(0x6a, (unsigned long) op_push_ib);
  asm_register_ia32_opcode(0x6b, (unsigned long) op_imul_gv_ev_ib);
  asm_register_ia32_opcode(0x6c, (unsigned long) op_insb);
  asm_register_ia32_opcode(0x6d, (unsigned long) op_insw);
  asm_register_ia32_opcode(0x6e, (unsigned long) op_outsb);
  asm_register_ia32_opcode(0x6f, (unsigned long) op_outsw);
  asm_register_ia32_opcode(0x70, (unsigned long) op_jo);
  asm_register_ia32_opcode(0x71, (unsigned long) op_jno);
  asm_register_ia32_opcode(0x72, (unsigned long) op_jb);
  asm_register_ia32_opcode(0x73, (unsigned long) op_jae);
  asm_register_ia32_opcode(0x74, (unsigned long) op_je);
  asm_register_ia32_opcode(0x75, (unsigned long) op_jne);
  asm_register_ia32_opcode(0x76, (unsigned long) op_jbe);
  asm_register_ia32_opcode(0x77, (unsigned long) op_ja);
  asm_register_ia32_opcode(0x78, (unsigned long) op_js);
  asm_register_ia32_opcode(0x79, (unsigned long) op_jns);
  asm_register_ia32_opcode(0x7a, (unsigned long) op_jp);
  asm_register_ia32_opcode(0x7b, (unsigned long) op_jnp);
  asm_register_ia32_opcode(0x7c, (unsigned long) op_jl);
  asm_register_ia32_opcode(0x7d, (unsigned long) op_jge);
  asm_register_ia32_opcode(0x7e, (unsigned long) op_jle);
  asm_register_ia32_opcode(0x7f, (unsigned long) op_jg);
  asm_register_ia32_opcode(0x80, (unsigned long) op_immed_rmb_ib);
  asm_register_ia32_opcode(0x82, (unsigned long) op_immed_rmb_ib);
  asm_register_ia32_opcode(0x81, (unsigned long) op_immed_rmv_iv);
  asm_register_ia32_opcode(0x83, (unsigned long) op_immed_rmv_ib);
  asm_register_ia32_opcode(0x84, (unsigned long) op_test_rmb_rb);
  asm_register_ia32_opcode(0x85, (unsigned long) op_test_rmv_rv);
  asm_register_ia32_opcode(0x86, (unsigned long) op_xchg_rmb_rb);
  asm_register_ia32_opcode(0x87, (unsigned long) op_xchg_rmv_rv);
  asm_register_ia32_opcode(0x88, (unsigned long) op_mov_rmb_rb);
  asm_register_ia32_opcode(0x89, (unsigned long) op_mov_rmv_rv);
  asm_register_ia32_opcode(0x8a, (unsigned long) op_mov_rb_rmb);
  asm_register_ia32_opcode(0x8b, (unsigned long) op_mov_rv_rmv);
  asm_register_ia32_opcode(0x8c, (unsigned long) op_mov_rm_segr);
  asm_register_ia32_opcode(0x8d, (unsigned long) op_lea_rv_m);
  asm_register_ia32_opcode(0x8e, (unsigned long) op_mov_segr_rm);
  asm_register_ia32_opcode(0x8f, (unsigned long) op_pop_rmv);
  asm_register_ia32_opcode(0x90, (unsigned long) op_nop);
  asm_register_ia32_opcode(0x91, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x92, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x93, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x94, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x95, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x96, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x97, (unsigned long) op_xchg_eax_reg);
  asm_register_ia32_opcode(0x98, (unsigned long) op_cwtl);
  asm_register_ia32_opcode(0x99, (unsigned long) op_cltd);
  asm_register_ia32_opcode(0x9b, (unsigned long) op_fwait);
  asm_register_ia32_opcode(0x9c, (unsigned long) op_pushf);
  asm_register_ia32_opcode(0x9d, (unsigned long) op_popf);
  asm_register_ia32_opcode(0x9e, (unsigned long) op_sahf);
  asm_register_ia32_opcode(0x9f, (unsigned long) op_lahf);
  asm_register_ia32_opcode(0xa0, (unsigned long) op_mov_al_ref_iv);
  asm_register_ia32_opcode(0xa1, (unsigned long) op_mov_eax_ref_iv);
  asm_register_ia32_opcode(0xa2, (unsigned long) op_mov_ref_iv_al);
  asm_register_ia32_opcode(0xa3, (unsigned long) op_mov_ref_iv_eax);
  asm_register_ia32_opcode(0xa4, (unsigned long) op_movsb);
  asm_register_ia32_opcode(0xa5, (unsigned long) op_movsd);
  asm_register_ia32_opcode(0xa6, (unsigned long) op_cmpsb);
  asm_register_ia32_opcode(0xa7, (unsigned long) op_cmpsd);
  asm_register_ia32_opcode(0xa8, (unsigned long) op_test_al_rb);
  asm_register_ia32_opcode(0xa9, (unsigned long) op_test_eax_iv);
  asm_register_ia32_opcode(0xaa, (unsigned long) op_stosb);
  asm_register_ia32_opcode(0xab, (unsigned long) op_stosd);
  asm_register_ia32_opcode(0xac, (unsigned long) op_lodsb);
  asm_register_ia32_opcode(0xad, (unsigned long) op_lodsd);
  asm_register_ia32_opcode(0xae, (unsigned long) op_scasb);
  asm_register_ia32_opcode(0xaf, (unsigned long) op_scasd);
  asm_register_ia32_opcode(0xb0, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb1, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb2, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb3, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb4, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb5, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb6, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb7, (unsigned long) op_mov_subreg_ib);
  asm_register_ia32_opcode(0xb8, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xb9, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xba, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xbb, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xbc, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xbd, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xbe, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xbf, (unsigned long) op_mov_reg_iv);
  asm_register_ia32_opcode(0xc0, (unsigned long) op_shr_rmb_ib);
  asm_register_ia32_opcode(0xc1, (unsigned long) op_shr_rmv_ib);
  asm_register_ia32_opcode(0xc2, (unsigned long) op_ret_i2);
  asm_register_ia32_opcode(0xc3, (unsigned long) op_ret);
  asm_register_ia32_opcode(0xc4, (unsigned long) op_les_rm_rmp);
  asm_register_ia32_opcode(0xc5, (unsigned long) op_lds_rm_rmp);
  asm_register_ia32_opcode(0xc6, (unsigned long) op_mov_rmb_ib);
  asm_register_ia32_opcode(0xc7, (unsigned long) op_mov_rmv_iv);
  asm_register_ia32_opcode(0xc8, (unsigned long) op_enter);
  asm_register_ia32_opcode(0xc9, (unsigned long) op_leave);
  asm_register_ia32_opcode(0xca, (unsigned long) op_retf_i2);
  asm_register_ia32_opcode(0xcb, (unsigned long) op_retf);
  asm_register_ia32_opcode(0xcc, (unsigned long) op_int_3);
  asm_register_ia32_opcode(0xcd, (unsigned long) op_int_ib);
  asm_register_ia32_opcode(0xce, (unsigned long) op_into);
  asm_register_ia32_opcode(0xcf, (unsigned long) op_iret);
  asm_register_ia32_opcode(0xd0, (unsigned long) op_shift_rmb_1);
  asm_register_ia32_opcode(0xd1, (unsigned long) op_shift_rmv_1);
  asm_register_ia32_opcode(0xd2, (unsigned long) op_shift_rmb_cl);
  asm_register_ia32_opcode(0xd3, (unsigned long) op_shift_rmv_cl);
  asm_register_ia32_opcode(0xd4, (unsigned long) op_aam);
  asm_register_ia32_opcode(0xd5, (unsigned long) op_aad);
  asm_register_ia32_opcode(0xd7, (unsigned long) op_xlatb);
  asm_register_ia32_opcode(0xd8, (unsigned long) op_esc0);
  asm_register_ia32_opcode(0xd9, (unsigned long) op_esc1);
  asm_register_ia32_opcode(0xda, (unsigned long) op_esc2);
  asm_register_ia32_opcode(0xdb, (unsigned long) op_esc3);
  asm_register_ia32_opcode(0xdc, (unsigned long) op_esc4);
  asm_register_ia32_opcode(0xdd, (unsigned long) op_esc5);
  asm_register_ia32_opcode(0xde, (unsigned long) op_esc6);
  asm_register_ia32_opcode(0xdf, (unsigned long) op_esc7);
  asm_register_ia32_opcode(0xe0, (unsigned long) op_loopne);
  asm_register_ia32_opcode(0xe1, (unsigned long) op_loope);
  asm_register_ia32_opcode(0xe2, (unsigned long) op_loop);
  asm_register_ia32_opcode(0xe3, (unsigned long) op_je_cxz);
  asm_register_ia32_opcode(0xe4, (unsigned long) op_in_al_ref_ib);
  asm_register_ia32_opcode(0xe5, (unsigned long) op_in_eax_ref_ib);
  asm_register_ia32_opcode(0xe6, (unsigned long) op_out_ref_ib_al);
  asm_register_ia32_opcode(0xe7, (unsigned long) op_out_ref_ib_eax);
  asm_register_ia32_opcode(0xe8, (unsigned long) op_call_iv);
  asm_register_ia32_opcode(0xe9, (unsigned long) op_jmp_iv);
  asm_register_ia32_opcode(0xea, (unsigned long) op_jmp_ap);
  asm_register_ia32_opcode(0xeb, (unsigned long) op_jmp_ib);
  asm_register_ia32_opcode(0xec, (unsigned long) op_in_al_dx);
  asm_register_ia32_opcode(0xed, (unsigned long) op_in_eax_dx);
  asm_register_ia32_opcode(0xee, (unsigned long) op_out_dx_al);
  asm_register_ia32_opcode(0xef, (unsigned long) op_out_dx_eax);
  asm_register_ia32_opcode(0xf0, (unsigned long) op_lock);
  asm_register_ia32_opcode(0xf1, (unsigned long) op_lock);
  asm_register_ia32_opcode(0xf2, (unsigned long) op_repnz);
  asm_register_ia32_opcode(0xf3, (unsigned long) op_repz);
  asm_register_ia32_opcode(0xf4, (unsigned long) op_hlt);
  asm_register_ia32_opcode(0xf5, (unsigned long) op_cmc);
  asm_register_ia32_opcode(0xf6, (unsigned long) op_unary_rmb);
  asm_register_ia32_opcode(0xf7, (unsigned long) op_unary_rmv);
  asm_register_ia32_opcode(0xf8, (unsigned long) op_clc);
  asm_register_ia32_opcode(0xf9, (unsigned long) op_stc);
  asm_register_ia32_opcode(0xfa, (unsigned long) op_cli);
  asm_register_ia32_opcode(0xfb, (unsigned long) op_sti);
  asm_register_ia32_opcode(0xfc, (unsigned long) op_cld);
  asm_register_ia32_opcode(0xfd, (unsigned long) op_std);
  asm_register_ia32_opcode(0xfe, (unsigned long) op_incdec_rmb);
  asm_register_ia32_opcode(0xff, (unsigned long) op_indir_rmv);
	  
	  
  asm_register_ia32_opcode(0x100 + 0x00, (unsigned long) op_group6);
  asm_register_ia32_opcode(0x100 + 0x01, (unsigned long) op_group7);
  asm_register_ia32_opcode(0x100 + 0x09, (unsigned long) i386_wbinvd);
  asm_register_ia32_opcode(0x100 + 0x0b, (unsigned long) op_ud2a);
  asm_register_ia32_opcode(0x100 + 0x20, (unsigned long) i386_mov_rm_cr);
  asm_register_ia32_opcode(0x100 + 0x22, (unsigned long) i386_mov_cr_rm);
  asm_register_ia32_opcode(0x100 + 0x23, (unsigned long) i386_mov_dr_rm);
  asm_register_ia32_opcode(0x100 + 0x31, (unsigned long) i386_rdtsc);
  asm_register_ia32_opcode(0x100 + 0x32, (unsigned long) i386_rdmsr);
  asm_register_ia32_opcode(0x100 + 0x40, (unsigned long) i386_cmovo);
  asm_register_ia32_opcode(0x100 + 0x41, (unsigned long) i386_cmovno);
  asm_register_ia32_opcode(0x100 + 0x42, (unsigned long) i386_cmovb);
  asm_register_ia32_opcode(0x100 + 0x43, (unsigned long) i386_cmovae);
  asm_register_ia32_opcode(0x100 + 0x44, (unsigned long) i386_cmove);
  asm_register_ia32_opcode(0x100 + 0x45, (unsigned long) i386_cmovne);
  asm_register_ia32_opcode(0x100 + 0x46, (unsigned long) i386_cmovbe);
  asm_register_ia32_opcode(0x100 + 0x47, (unsigned long) i386_cmova);
  asm_register_ia32_opcode(0x100 + 0x48, (unsigned long) i386_cmovs);
  asm_register_ia32_opcode(0x100 + 0x49, (unsigned long) i386_cmovns);
  asm_register_ia32_opcode(0x100 + 0x4a, (unsigned long) i386_cmovp);
  asm_register_ia32_opcode(0x100 + 0x4b, (unsigned long) i386_cmovnp);
  asm_register_ia32_opcode(0x100 + 0x4c, (unsigned long) i386_cmovl);
  asm_register_ia32_opcode(0x100 + 0x4d, (unsigned long) i386_cmovnl);
  asm_register_ia32_opcode(0x100 + 0x4e, (unsigned long) i386_cmovle);
  asm_register_ia32_opcode(0x100 + 0x4f, (unsigned long) i386_cmovnle);
  asm_register_ia32_opcode(0x100 + 0x60, (unsigned long) i386_punpcklbw_pq_qd);
  asm_register_ia32_opcode(0x100 + 0x67, (unsigned long) i386_packuswb_pq_qq);
  asm_register_ia32_opcode(0x100 + 0x68, (unsigned long) i386_punpckhbw_pq_qq);
  asm_register_ia32_opcode(0x100 + 0x6e, (unsigned long) i386_movd_pd_qd);
  asm_register_ia32_opcode(0x100 + 0x6f, (unsigned long) i386_movq_pq_qq);
  asm_register_ia32_opcode(0x100 + 0x71, (unsigned long) i386_group12);
  asm_register_ia32_opcode(0x100 + 0x72, (unsigned long) i386_group14);
  asm_register_ia32_opcode(0x100 + 0x73, (unsigned long) i386_group15);
  asm_register_ia32_opcode(0x100 + 0x77, (unsigned long) i386_emms);
  asm_register_ia32_opcode(0x100 + 0x7f, (unsigned long) i386_movq_qq_pq);
  asm_register_ia32_opcode(0x100 + 0x82, (unsigned long) i386_jb);
  asm_register_ia32_opcode(0x100 + 0x83, (unsigned long) i386_jae);
  asm_register_ia32_opcode(0x100 + 0x84, (unsigned long) i386_je);
  asm_register_ia32_opcode(0x100 + 0x85, (unsigned long) i386_jne);
  asm_register_ia32_opcode(0x100 + 0x86, (unsigned long) i386_jbe);
  asm_register_ia32_opcode(0x100 + 0x87, (unsigned long) i386_ja);
  asm_register_ia32_opcode(0x100 + 0x88, (unsigned long) i386_js);
  asm_register_ia32_opcode(0x100 + 0x89, (unsigned long) i386_jns);
  asm_register_ia32_opcode(0x100 + 0x8a, (unsigned long) i386_jp);
  asm_register_ia32_opcode(0x100 + 0x8b, (unsigned long) i386_jnp);
  asm_register_ia32_opcode(0x100 + 0x8c, (unsigned long) i386_jl);
  asm_register_ia32_opcode(0x100 + 0x8d, (unsigned long) i386_jge);
  asm_register_ia32_opcode(0x100 + 0x8e, (unsigned long) i386_jle);
  asm_register_ia32_opcode(0x100 + 0x8f, (unsigned long) i386_jg);
  asm_register_ia32_opcode(0x100 + 0x90, (unsigned long) i386_jg);
  asm_register_ia32_opcode(0x100 + 0x91, (unsigned long) op_setno_rmb);
  asm_register_ia32_opcode(0x100 + 0x92, (unsigned long) op_setb_rmb);
  asm_register_ia32_opcode(0x100 + 0x93, (unsigned long) op_setae_rmb);
  asm_register_ia32_opcode(0x100 + 0x94, (unsigned long) op_sete_rmb);
  asm_register_ia32_opcode(0x100 + 0x95, (unsigned long) op_setne_rmb);
  asm_register_ia32_opcode(0x100 + 0x96, (unsigned long) op_setbe_rmb);
  asm_register_ia32_opcode(0x100 + 0x97, (unsigned long) op_seta_rmb);
  asm_register_ia32_opcode(0x100 + 0x98, (unsigned long) op_sets_rmb);
  asm_register_ia32_opcode(0x100 + 0x99, (unsigned long) op_setns_rmb);
  asm_register_ia32_opcode(0x100 + 0x9a, (unsigned long) op_setp_rmb);
  asm_register_ia32_opcode(0x100 + 0x9b, (unsigned long) op_setnp_rmb);
  asm_register_ia32_opcode(0x100 + 0x9c, (unsigned long) op_setl_rmb);
  asm_register_ia32_opcode(0x100 + 0x9d, (unsigned long) op_setge_rmb);
  asm_register_ia32_opcode(0x100 + 0x9e, (unsigned long) op_setle_rmb);
  asm_register_ia32_opcode(0x100 + 0x9f, (unsigned long) op_setg_rmb);
  asm_register_ia32_opcode(0x100 + 0xa0, (unsigned long) i386_push_fs);
  asm_register_ia32_opcode(0x100 + 0xa1, (unsigned long) i386_pop_fs);
  asm_register_ia32_opcode(0x100 + 0xa2, (unsigned long) i386_cpuid);
  asm_register_ia32_opcode(0x100 + 0xa3, (unsigned long) i386_bt_rm_r);
  asm_register_ia32_opcode(0x100 + 0xa4, (unsigned long) i386_shld);
  asm_register_ia32_opcode(0x100 + 0xa5, (unsigned long) i386_shld_rmv_rv_cl);
  asm_register_ia32_opcode(0x100 + 0xa7, (unsigned long) i386_xstorenrg);
  asm_register_ia32_opcode(0x100 + 0xab, (unsigned long) i386_bts);
  asm_register_ia32_opcode(0x100 + 0xac, (unsigned long) i386_shrd_rmv_rv_ib);
  asm_register_ia32_opcode(0x100 + 0xad, (unsigned long) i386_shrd_rmv_rv_cl);
  asm_register_ia32_opcode(0x100 + 0xae, (unsigned long) i386_group16);
  asm_register_ia32_opcode(0x100 + 0xaf, (unsigned long) i386_imul_rv_rmv);
  asm_register_ia32_opcode(0x100 + 0xb1, (unsigned long) op_cmp_xchg);
  asm_register_ia32_opcode(0x100 + 0xb2, (unsigned long) op_lss_rv_rmv);
  asm_register_ia32_opcode(0x100 + 0xb3, (unsigned long) op_btr_rmv_rv);
  asm_register_ia32_opcode(0x100 + 0xb6, (unsigned long) i386_movzbl_rv_rmb);
  asm_register_ia32_opcode(0x100 + 0xb7, (unsigned long) i386_movzwl_rv_rm2);
  asm_register_ia32_opcode(0x100 + 0xba, (unsigned long) i386_btrl);
  asm_register_ia32_opcode(0x100 + 0xbc, (unsigned long) i386_bsf);
  asm_register_ia32_opcode(0x100 + 0xbd, (unsigned long) i386_bsr_rv_rmb);
  asm_register_ia32_opcode(0x100 + 0xbe, (unsigned long) i386_movsbl_rv_rmb);
  asm_register_ia32_opcode(0x100 + 0xbf, (unsigned long) i386_movswl_rv_rm2);
  asm_register_ia32_opcode(0x100 + 0xc1, (unsigned long) i386_xadd);
  asm_register_ia32_opcode(0x100 + 0xc8, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xc9, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xca, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xcb, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xcc, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xcd, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xce, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xcf, (unsigned long) i386_bswap);
  asm_register_ia32_opcode(0x100 + 0xd5, (unsigned long) i386_pmullw_pq_qq);
  asm_register_ia32_opcode(0x100 + 0xdb, (unsigned long) i386_pand_pq_qq);
  asm_register_ia32_opcode(0x100 + 0xdc, (unsigned long) i386_paddusb_pq_qq);
  asm_register_ia32_opcode(0x100 + 0xdd, (unsigned long) i386_paddusw_pq_qq);
  asm_register_ia32_opcode(0x100 + 0xeb, (unsigned long) i386_por_pq_qq);
  asm_register_ia32_opcode(0x100 + 0xef, (unsigned long) i386_pxor_pq_qq);
  return (1);
}
