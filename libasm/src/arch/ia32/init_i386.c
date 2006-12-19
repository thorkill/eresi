/**
 * $Id: init_i386.c,v 1.7 2006-12-19 10:24:36 heroine Exp $
 *
 */
#ifndef I386_H_
#define I386_h_

#include <libasm.h>
#include <libasm-int.h>

void	init_instr_table(asm_processor *);


/**
 * handler to disassemble ia32 code.
 * fetch in asm_i386_processor structure points to this function
 *
 */
int     fetch_i386(asm_instr *instr, u_char *buf, u_int len, asm_processor *proc) {
  asm_i386_processor	*i386p;

  i386p = (asm_i386_processor *) proc;
  if (buf && len > 0)
    if (i386p->internals->cisc_set[buf[0]])
      return (i386p->internals->cisc_set[buf[0]](instr, buf, len, proc));
  return (0);
}

/**
 *
 *
 *
 *
 */

void asm_init_i386(asm_processor *proc) {
  int i;
  struct s_asm_proc_i386 *inter;

  init_instr_table(proc);

  proc->resolve_immediate = asm_resolve_ia32;
  proc->resolve_data = 0;
  // proc->get_operand = create_i386_operand;
  proc->fetch = fetch_i386;
  proc->display_handle = asm_ia32_display_instr_att;
  inter = proc->internals = malloc(sizeof (struct s_asm_proc_i386));
  inter->opsize = inter->addsize = 0;
  inter->mode = INTEL_PROT;
  // inter->get_vect_size = get_i386_vect_size;
  for (i = 0; i< 256; i++) {
    inter->cisc_set[i] = 0;
    inter->cisc_i386[i] = 0;
  }
  // inter->type = ASM_ARCH_i386;
  inter->cisc_set[0x00] = op_add_rmb_rb;
  inter->cisc_set[0x01] = op_add_rmv_rv;
  inter->cisc_set[0x02] = op_add_rb_rmb;
  inter->cisc_set[0x03] = op_add_rv_rmv;
  inter->cisc_set[0x04] = op_add_al_ib;
  inter->cisc_set[0x05] = op_add_eax_iv;
  inter->cisc_set[0x06] = op_push_es;
  inter->cisc_set[0x07] = op_pop_es;
  inter->cisc_set[0x08] = op_or_rmb_rb;
  inter->cisc_set[0x09] = op_or_rmv_rv;
  inter->cisc_set[0x0a] = op_or_rb_rmb;
  inter->cisc_set[0x0b] = op_or_rv_rmv;
  inter->cisc_set[0x0c] = op_or_al_ib;
  inter->cisc_set[0x0d] = op_or_eax_iv;
  inter->cisc_set[0x0e] = op_push_cs;
  inter->cisc_set[0x0f] = op_386sp;
  inter->cisc_set[0x10] = op_adc_rmb_rb;
  inter->cisc_set[0x11] = op_adc_rmv_rv;
  inter->cisc_set[0x12] = op_adc_rb_rmb;
  inter->cisc_set[0x13] = op_adc_rv_rmv;
  inter->cisc_set[0x14] = op_adc_al_ib;
  inter->cisc_set[0x15] = op_adc_eax_iv;
  inter->cisc_set[0x16] = op_push_ss;
  inter->cisc_set[0x17] = op_pop_es;
  inter->cisc_set[0x18] = op_sbb_rmb_rb;
  inter->cisc_set[0x19] = op_sbb_rmv_rv;
  inter->cisc_set[0x1a] = op_sbb_rb_rmb;
  inter->cisc_set[0x1b] = op_sbb_rv_rmv;
  inter->cisc_set[0x1c] = op_sbb_al_ib;
  inter->cisc_set[0x1d] = op_sbb_eax_iv;
  inter->cisc_set[0x1e] = op_push_ds;
  inter->cisc_set[0x1f] = op_pop_ds;
  inter->cisc_set[0x20] = op_and_rmb_rb;
  inter->cisc_set[0x21] = op_and_rmv_rv;
  inter->cisc_set[0x22] = op_and_rb_rmb;
  inter->cisc_set[0x23] = op_and_rv_rmv;
  inter->cisc_set[0x24] = op_and_al_ib;
  inter->cisc_set[0x25] = op_and_eax_iv;
  inter->cisc_set[0x26] = op_prefix_es;
  inter->cisc_set[0x27] = op_daa;
  inter->cisc_set[0x28] = op_sub_rmb_rb;
  inter->cisc_set[0x29] = op_sub_rmv_rv;
  inter->cisc_set[0x2a] = op_sub_rb_rmb;
  inter->cisc_set[0x2b] = op_sub_rv_rmv;
  inter->cisc_set[0x2c] = op_sub_al_ib;
  inter->cisc_set[0x2d] = op_sub_eax_iv;
  inter->cisc_set[0x2e] = op_prefix_cs;
  inter->cisc_set[0x2f] = op_das;
  inter->cisc_set[0x30] = op_xor_rmb_rb;
  inter->cisc_set[0x31] = op_xor_rmv_rv;
  inter->cisc_set[0x32] = op_xor_rb_rmb;
  inter->cisc_set[0x33] = op_xor_rv_rmv;
  inter->cisc_set[0x34] = op_xor_al_ib;
  inter->cisc_set[0x35] = op_xor_eax_iv;
  inter->cisc_set[0x36] = op_prefix_ss;
  inter->cisc_set[0x37] = op_aaa;
  inter->cisc_set[0x38] = op_cmp_rmb_rb;
  inter->cisc_set[0x39] = op_cmp_rmv_rv;
  inter->cisc_set[0x3a] = op_cmp_rb_rmb;
  inter->cisc_set[0x3b] = op_cmp_rv_rmv;
  inter->cisc_set[0x3c] = op_cmp_al_ib;
  inter->cisc_set[0x3d] = op_cmp_eax_iv;
  inter->cisc_set[0x3e] = op_prefix_ds;
  inter->cisc_set[0x3f] = op_aas;
  inter->cisc_set[0x40] = op_inc_reg;
  inter->cisc_set[0x41] = op_inc_reg;
  inter->cisc_set[0x42] = op_inc_reg;
  inter->cisc_set[0x43] = op_inc_reg;
  inter->cisc_set[0x44] = op_inc_reg;
  inter->cisc_set[0x45] = op_inc_reg;
  inter->cisc_set[0x46] = op_inc_reg;
  inter->cisc_set[0x47] = op_inc_reg;
  inter->cisc_set[0x48] = op_dec_reg;
  inter->cisc_set[0x49] = op_dec_reg;
  inter->cisc_set[0x4a] = op_dec_reg;
  inter->cisc_set[0x4b] = op_dec_reg;
  inter->cisc_set[0x4c] = op_dec_reg;
  inter->cisc_set[0x4d] = op_dec_reg;
  inter->cisc_set[0x4e] = op_dec_reg;
  inter->cisc_set[0x4f] = op_dec_reg;
  inter->cisc_set[0x50] = op_push_reg;
  inter->cisc_set[0x51] = op_push_reg;
  inter->cisc_set[0x52] = op_push_reg;
  inter->cisc_set[0x53] = op_push_reg;
  inter->cisc_set[0x54] = op_push_reg;
  inter->cisc_set[0x55] = op_push_reg;
  inter->cisc_set[0x56] = op_push_reg;
  inter->cisc_set[0x57] = op_push_reg;
  inter->cisc_set[0x58] = op_pop_reg;
  inter->cisc_set[0x59] = op_pop_reg;
  inter->cisc_set[0x5a] = op_pop_reg;
  inter->cisc_set[0x5b] = op_pop_reg;
  inter->cisc_set[0x5c] = op_pop_reg;
  inter->cisc_set[0x5d] = op_pop_reg;
  inter->cisc_set[0x5e] = op_pop_reg;
  inter->cisc_set[0x5f] = op_pop_reg;
  inter->cisc_set[0x60] = op_pusha;
  inter->cisc_set[0x61] = op_popa;
  inter->cisc_set[0x62] = op_bound_gv_ma;
  inter->cisc_set[0x63] = op_arpl_ew_rw;
  inter->cisc_set[0x64] = op_prefix_fs;
  inter->cisc_set[0x65] = op_prefix_gs;
  inter->cisc_set[0x66] = op_opsize;
  inter->cisc_set[0x67] = op_addsize;
  inter->cisc_set[0x68] = op_push_iv;
  inter->cisc_set[0x69] = op_imul_rv_rmv_iv;
  inter->cisc_set[0x6a] = op_push_ib;
  inter->cisc_set[0x6b] = op_imul_gv_ev_ib;
  inter->cisc_set[0x6c] = op_insb;
  inter->cisc_set[0x6d] = op_insw;
  inter->cisc_set[0x6e] = op_outsb;
  inter->cisc_set[0x6f] = op_outsw;
  inter->cisc_set[0x70] = op_jo;
  inter->cisc_set[0x71] = op_jno;
  inter->cisc_set[0x72] = op_jb;
  inter->cisc_set[0x73] = op_jae;
  inter->cisc_set[0x74] = op_je;
  inter->cisc_set[0x75] = op_jne;
  inter->cisc_set[0x76] = op_jbe;
  inter->cisc_set[0x77] = op_ja;
  inter->cisc_set[0x78] = op_js;
  inter->cisc_set[0x79] = op_jns;
  inter->cisc_set[0x7a] = op_jp;
  inter->cisc_set[0x7b] = op_jnp;
  inter->cisc_set[0x7c] = op_jl;
  inter->cisc_set[0x7d] = op_jge;
  inter->cisc_set[0x7e] = op_jle;
  inter->cisc_set[0x7f] = op_jg;
  inter->cisc_set[0x80] = op_immed_rmb_ib;
  inter->cisc_set[0x82] = op_immed_rmb_ib;
  inter->cisc_set[0x81] = op_immed_rmv_iv;
  inter->cisc_set[0x83] = op_immed_rmv_ib;
  inter->cisc_set[0x84] = op_test_rmb_rb;
  inter->cisc_set[0x85] = op_test_rmv_rv;
  inter->cisc_set[0x86] = op_xchg_rmb_rb;
  inter->cisc_set[0x87] = op_xchg_rmv_rv;
  inter->cisc_set[0x88] = op_mov_rmb_rb;
  inter->cisc_set[0x89] = op_mov_rmv_rv;
  inter->cisc_set[0x8a] = op_mov_rb_rmb;
  inter->cisc_set[0x8b] = op_mov_rv_rmv;
  inter->cisc_set[0x8c] = op_mov_rm_segr;
  inter->cisc_set[0x8d] = op_lea_rv_m;
  inter->cisc_set[0x8e] = op_mov_segr_rm;
  inter->cisc_set[0x8f] = op_pop_rmv;
  inter->cisc_set[0x90] = op_nop;
  inter->cisc_set[0x91] = op_xchg_eax_reg;
  inter->cisc_set[0x92] = op_xchg_eax_reg;
  inter->cisc_set[0x93] = op_xchg_eax_reg;
  inter->cisc_set[0x94] = op_xchg_eax_reg;
  inter->cisc_set[0x95] = op_xchg_eax_reg;
  inter->cisc_set[0x96] = op_xchg_eax_reg;
  inter->cisc_set[0x97] = op_xchg_eax_reg;
  inter->cisc_set[0x98] = op_cwtl;
  inter->cisc_set[0x99] = op_cltd;
  inter->cisc_set[0x9b] = op_fwait;
  inter->cisc_set[0x9c] = op_pushf;
  inter->cisc_set[0x9d] = op_popf;
  inter->cisc_set[0x9e] = op_sahf;
  inter->cisc_set[0x9f] = op_lahf;
  inter->cisc_set[0xa0] = op_mov_al_ref_iv;
  inter->cisc_set[0xa1] = op_mov_eax_ref_iv;
  inter->cisc_set[0xa2] = op_mov_ref_iv_al;
  inter->cisc_set[0xa3] = op_mov_ref_iv_eax;
  inter->cisc_set[0xa4] = op_movsb;
  inter->cisc_set[0xa5] = op_movsd;
  inter->cisc_set[0xa6] = op_cmpsb;
  inter->cisc_set[0xa7] = op_cmpsd;
  inter->cisc_set[0xa8] = op_test_al_rb;
  inter->cisc_set[0xa9] = op_test_eax_iv;
  inter->cisc_set[0xaa] = op_stosb;
  inter->cisc_set[0xab] = op_stosd;
  inter->cisc_set[0xac] = op_lodsb;
  inter->cisc_set[0xad] = op_lodsd;
  inter->cisc_set[0xae] = op_scasb;
  inter->cisc_set[0xaf] = op_scasd;
  inter->cisc_set[0xb0] = op_mov_subreg_ib;
  inter->cisc_set[0xb1] = op_mov_subreg_ib;
  inter->cisc_set[0xb2] = op_mov_subreg_ib;
  inter->cisc_set[0xb3] = op_mov_subreg_ib;
  inter->cisc_set[0xb4] = op_mov_subreg_ib;
  inter->cisc_set[0xb5] = op_mov_subreg_ib;
  inter->cisc_set[0xb6] = op_mov_subreg_ib;
  inter->cisc_set[0xb7] = op_mov_subreg_ib;
  inter->cisc_set[0xb8] = op_mov_reg_iv;
  inter->cisc_set[0xb9] = op_mov_reg_iv;
  inter->cisc_set[0xba] = op_mov_reg_iv;
  inter->cisc_set[0xbb] = op_mov_reg_iv;
  inter->cisc_set[0xbc] = op_mov_reg_iv;
  inter->cisc_set[0xbd] = op_mov_reg_iv;
  inter->cisc_set[0xbe] = op_mov_reg_iv;
  inter->cisc_set[0xbf] = op_mov_reg_iv;
  inter->cisc_set[0xc0] = op_shr_rmb_ib;
  inter->cisc_set[0xc1] = op_shr_rmv_ib;
  inter->cisc_set[0xc2] = op_ret_i2;
  inter->cisc_set[0xc3] = op_ret;
  inter->cisc_set[0xc4] = op_les_rm_rmp;
  inter->cisc_set[0xc5] = op_lds_rm_rmp;
  inter->cisc_set[0xc6] = op_mov_rmb_ib;
  inter->cisc_set[0xc7] = op_mov_rmv_iv;
  inter->cisc_set[0xc8] = op_enter;
  inter->cisc_set[0xc9] = op_leave;
  inter->cisc_set[0xca] = op_retf_i2;
  inter->cisc_set[0xcb] = op_retf;
  inter->cisc_set[0xcc] = op_int_3;
  inter->cisc_set[0xcd] = op_int_ib;
  inter->cisc_set[0xce] = op_into;
  inter->cisc_set[0xcf] = op_iret;
  inter->cisc_set[0xd0] = op_shift_rmb_1;
  inter->cisc_set[0xd1] = op_shift_rmv_1;
  inter->cisc_set[0xd2] = op_shift_rmb_cl;
  inter->cisc_set[0xd3] = op_shift_rmv_cl;
  inter->cisc_set[0xd4] = op_aam;
  inter->cisc_set[0xd5] = op_aad;
  inter->cisc_set[0xd7] = op_xlatb;
  inter->cisc_set[0xd8] = op_esc0;
  inter->cisc_set[0xd9] = op_esc1;
  inter->cisc_set[0xda] = op_esc2;
  inter->cisc_set[0xdb] = op_esc3;
  inter->cisc_set[0xdc] = op_esc4;
  inter->cisc_set[0xdd] = op_esc5;
  inter->cisc_set[0xde] = op_esc6;
  inter->cisc_set[0xdf] = op_esc7;
  inter->cisc_set[0xe0] = op_loopne;
  inter->cisc_set[0xe1] = op_loope;
  inter->cisc_set[0xe2] = op_loop;
  inter->cisc_set[0xe3] = op_je_cxz;
  inter->cisc_set[0xe4] = op_in_al_ref_ib;
  inter->cisc_set[0xe5] = op_in_eax_ref_ib;
  inter->cisc_set[0xe6] = op_out_ref_ib_al;
  inter->cisc_set[0xe7] = op_out_ref_ib_eax;
  inter->cisc_set[0xe8] = op_call_iv;
  inter->cisc_set[0xe9] = op_jmp_iv;
  inter->cisc_set[0xea] = op_jmp_ap;
  inter->cisc_set[0xeb] = op_jmp_ib;
  inter->cisc_set[0xec] = op_in_al_dx;
  inter->cisc_set[0xed] = op_in_eax_dx;
  inter->cisc_set[0xee] = op_out_dx_al;
  inter->cisc_set[0xef] = op_out_dx_eax;
  inter->cisc_set[0xf0] = op_lock;
  inter->cisc_set[0xf1] = op_lock;
  inter->cisc_set[0xf2] = op_repnz;
  inter->cisc_set[0xf3] = op_repz;
  inter->cisc_set[0xf4] = op_hlt;
  inter->cisc_set[0xf5] = op_cmc;
  inter->cisc_set[0xf6] = op_unary_rmb;
  inter->cisc_set[0xf7] = op_unary_rmv;
  inter->cisc_set[0xf8] = op_clc;
  inter->cisc_set[0xf9] = op_stc;
  inter->cisc_set[0xfa] = op_cli;
  inter->cisc_set[0xfb] = op_sti;
  inter->cisc_set[0xfc] = op_cld;
  inter->cisc_set[0xfd] = op_std;
  inter->cisc_set[0xfe] = op_incdec_rmb;
  inter->cisc_set[0xff] = op_indir_rmv;
  inter->cisc_i386[0x00] = op_group6;
  inter->cisc_i386[0x01] = op_group7;
  inter->cisc_i386[0x09] = i386_wbinvd;
  inter->cisc_i386[0x0b] = op_ud2a;
  inter->cisc_i386[0x20] = i386_mov_rm_cr;
  inter->cisc_i386[0x22] = i386_mov_cr_rm;
  inter->cisc_i386[0x23] = i386_mov_dr_rm;
  inter->cisc_i386[0x31] = i386_rdtsc;
  inter->cisc_i386[0x32] = i386_rdmsr;
  inter->cisc_i386[0x40] = i386_cmovo;
  inter->cisc_i386[0x41] = i386_cmovno;
  inter->cisc_i386[0x42] = i386_cmovb;
  inter->cisc_i386[0x43] = i386_cmovae;
  inter->cisc_i386[0x44] = i386_cmove;
  inter->cisc_i386[0x45] = i386_cmovne;
  inter->cisc_i386[0x46] = i386_cmovbe;
  inter->cisc_i386[0x47] = i386_cmova;
  inter->cisc_i386[0x48] = i386_cmovs;
  inter->cisc_i386[0x49] = i386_cmovns;
  inter->cisc_i386[0x4a] = i386_cmovp;
  inter->cisc_i386[0x4b] = i386_cmovnp;
  inter->cisc_i386[0x4c] = i386_cmovl;
  inter->cisc_i386[0x4d] = i386_cmovnl;
  inter->cisc_i386[0x4e] = i386_cmovle;
  inter->cisc_i386[0x4f] = i386_cmovnle;
  inter->cisc_i386[0x60] = i386_punpcklbw_pq_qd;
  inter->cisc_i386[0x67] = i386_packuswb_pq_qq;
  inter->cisc_i386[0x68] = i386_punpckhbw_pq_qq;
  inter->cisc_i386[0x6e] = i386_movd_pd_qd;
  inter->cisc_i386[0x6f] = i386_movq_pq_qq;
  inter->cisc_i386[0x71] = i386_group12;
  inter->cisc_i386[0x73] = i386_group14;
  inter->cisc_i386[0x77] = i386_emms;
  inter->cisc_i386[0x7f] = i386_movq_qq_pq;
  inter->cisc_i386[0x82] = i386_jb;
  inter->cisc_i386[0x83] = i386_jae;
  inter->cisc_i386[0x84] = i386_je;
  inter->cisc_i386[0x85] = i386_jne;
  inter->cisc_i386[0x86] = i386_jbe;
  inter->cisc_i386[0x87] = i386_ja;
  inter->cisc_i386[0x88] = i386_js;
  inter->cisc_i386[0x89] = i386_jns;
  inter->cisc_i386[0x8a] = i386_jp;
  inter->cisc_i386[0x8b] = i386_jnp;
  inter->cisc_i386[0x8c] = i386_jl;
  inter->cisc_i386[0x8d] = i386_jge;
  inter->cisc_i386[0x8e] = i386_jle;
  inter->cisc_i386[0x8f] = i386_jg;
  inter->cisc_i386[0x90] = i386_jg;
  inter->cisc_i386[0x91] = op_setno_rmb;
  inter->cisc_i386[0x92] = op_setb_rmb;
  inter->cisc_i386[0x93] = op_setae_rmb;
  inter->cisc_i386[0x94] = op_sete_rmb;
  inter->cisc_i386[0x95] = op_setne_rmb;
  inter->cisc_i386[0x96] = op_setbe_rmb;
  inter->cisc_i386[0x97] = op_seta_rmb;
  inter->cisc_i386[0x98] = op_sets_rmb;
  inter->cisc_i386[0x99] = op_setns_rmb;
  inter->cisc_i386[0x9a] = op_setp_rmb;
  inter->cisc_i386[0x9b] = op_setnp_rmb;
  inter->cisc_i386[0x9c] = op_setl_rmb;
  inter->cisc_i386[0x9d] = op_setge_rmb;
  inter->cisc_i386[0x9e] = op_setle_rmb;
  inter->cisc_i386[0x9f] = op_setg_rmb;
  inter->cisc_i386[0xa0] = i386_push_fs;
  inter->cisc_i386[0xa1] = i386_pop_fs;
  inter->cisc_i386[0xa2] = i386_cpuid;
  inter->cisc_i386[0xa3] = i386_bt_rm_r;
  inter->cisc_i386[0xa4] = i386_shld;
  inter->cisc_i386[0xa5] = i386_shld_rmv_rv_cl;
  inter->cisc_i386[0xa7] = i386_xstorenrg;
  inter->cisc_i386[0xab] = i386_bts;
  inter->cisc_i386[0xac] = i386_shrd_rmv_rv_ib;
  inter->cisc_i386[0xad] = i386_shrd_rmv_rv_cl;
  inter->cisc_i386[0xae] = i386_group15;
  inter->cisc_i386[0xaf] = i386_imul_rv_rmv;
  inter->cisc_i386[0xb1] = op_cmp_xchg;
  inter->cisc_i386[0xb2] = op_lss_rv_rmv;
  inter->cisc_i386[0xb3] = op_btr_rmv_rv;
  inter->cisc_i386[0xb6] = i386_movzbl_rv_rmb;
  inter->cisc_i386[0xb7] = i386_movzwl_rv_rm2;
  inter->cisc_i386[0xba] = i386_btrl;
  inter->cisc_i386[0xbc] = i386_bsf;
  inter->cisc_i386[0xbd] = i386_bsr_rv_rmb;
  inter->cisc_i386[0xbe] = i386_movsbl_rv_rmb;
  inter->cisc_i386[0xbf] = i386_movswl_rv_rm2;
  inter->cisc_i386[0xc1] = i386_xadd;
  inter->cisc_i386[0xc8] = i386_bswap;
  inter->cisc_i386[0xc9] = i386_bswap;
  inter->cisc_i386[0xca] = i386_bswap;
  inter->cisc_i386[0xcb] = i386_bswap;
  inter->cisc_i386[0xcc] = i386_bswap;
  inter->cisc_i386[0xcd] = i386_bswap;
  inter->cisc_i386[0xce] = i386_bswap;
  inter->cisc_i386[0xcf] = i386_bswap;
  inter->cisc_i386[0xd5] = i386_pmullw_pq_qq;
  inter->cisc_i386[0xdb] = i386_pand_pq_qq;
  inter->cisc_i386[0xdc] = i386_paddusb_pq_qq;
  inter->cisc_i386[0xdd] = i386_paddusw_pq_qq;
  inter->cisc_i386[0xeb] = i386_por_pq_qq;
  inter->cisc_i386[0xef] = i386_pxor_pq_qq;

}

void asm_free_i386(asm_processor *proc) {
  free(proc->internals);
  free(proc->instr_table);
}
#endif
