/**
 * $Id: libasm-int.h,v 1.6 2006-12-19 10:24:36 heroine Exp $
 *
 *
 */
#ifndef LIBASM_INT_H
#define LIBASM_INT_H

/**
 *
 */

typedef struct s_modrm {
  u_char        m:3;
  u_char        r:3;
  u_char        mod:2;
} asm_modrm;

/**
 *
 *
 */

typedef struct s_sidbyte {
  u_char	base:3;
  u_char	index:3;
  u_char	sid:2;

} asm_sidbyte;

/**
 *
 */

enum {
  INTEL_REAL,
  INTEL_PROT
} e_asm_proc_mode;


int		asm_int_pow2(int);

int		fetch_i386(asm_instr *, u_char *, u_int, asm_processor *);

void    asm_resolve_immediate(asm_processor *proc, u_int val, char *buffer, u_int len);


char	*asm_ia32_display_instr_att(asm_instr *ins, int addr);
char	*asm_sparc_display_instr(asm_instr *, int addr);


int		asm_proc_opsize(asm_processor *proc);
int		asm_proc_addsize(asm_processor *proc);
int		asm_proc_vector_size(asm_processor *proc);
int		asm_proc_vector_len(asm_processor *);
int		asm_proc_is_protected(asm_processor *);

/**
 *
 */

int	operand_rmb_rb(asm_instr *, u_char *, int, asm_processor *);
int	operand_rmv_rv(asm_instr *, u_char *, int, asm_processor *);
int	operand_rb_rmb(asm_instr *, u_char *, int, asm_processor *);
int	operand_rv_rmv(asm_instr *, u_char *, int, asm_processor *);
int	operand_rv_rmb(asm_instr *, u_char *, int, asm_processor *);

int	operand_rmb_ib(asm_instr *, u_char *, int, asm_processor *);
int	operand_rmv_iv(asm_instr *, u_char *, int, asm_processor *);
int	operand_rmv_ib(asm_instr *, u_char *, int, asm_processor *);

int	operand_rv_rm2(asm_instr *, u_char *, int, asm_processor *);

int	operand_rv_m(asm_instr *, u_char *, int, asm_processor *);

int	operand_rmv(asm_operand *, u_char *, u_int, asm_processor *);
int	operand_rmb(asm_operand *, u_char *, u_int, asm_processor *);

/**
 *
 *
 */

int op_add_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_add_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_add_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_add_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_add_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_add_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_es(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_es(asm_instr *, u_char *, u_int, asm_processor *);
int op_or_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_or_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_or_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_or_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_or_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_or_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_cs(asm_instr *, u_char *, u_int, asm_processor *);
int op_386sp(asm_instr *, u_char *, u_int, asm_processor *);
int op_adc_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_adc_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_adc_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_adc_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_adc_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_adc_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_ss(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_es(asm_instr *, u_char *, u_int, asm_processor *);
int op_sbb_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_sbb_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_sbb_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_sbb_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_sbb_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_sbb_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_ds(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_ds(asm_instr *, u_char *, u_int, asm_processor *);
int op_and_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_and_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_and_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_and_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_and_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_and_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_prefix_es(asm_instr *, u_char *, u_int, asm_processor *);
int op_daa(asm_instr *, u_char *, u_int, asm_processor *);
int op_sub_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_sub_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_sub_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_sub_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_sub_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_sub_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_prefix_cs(asm_instr *, u_char *, u_int, asm_processor *);
int op_das(asm_instr *, u_char *, u_int, asm_processor *);
int op_xor_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_xor_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_xor_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_xor_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_xor_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_xor_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_prefix_ss(asm_instr *, u_char *, u_int, asm_processor *);
int op_aaa(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_al_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmp_xchg(asm_instr *, u_char *, u_int, asm_processor *);
int op_prefix_ds(asm_instr *, u_char *, u_int, asm_processor *);
int op_aas(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_inc_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_dec_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_pusha(asm_instr *, u_char *, u_int, asm_processor *);
int op_popa(asm_instr *, u_char *, u_int, asm_processor *);
int op_bound_gv_ma(asm_instr *, u_char *, u_int, asm_processor *);
int op_arpl_ew_rw(asm_instr *, u_char *, u_int, asm_processor *);
int op_prefix_fs(asm_instr *, u_char *, u_int, asm_processor *);
int op_prefix_gs(asm_instr *, u_char *, u_int, asm_processor *);
int op_opsize(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_imul_rv_rmv_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_push_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_imul_gv_ev_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_insb(asm_instr *, u_char *, u_int, asm_processor *);
int op_insw(asm_instr *, u_char *, u_int, asm_processor *);
int op_outsb(asm_instr *, u_char *, u_int, asm_processor *);
int op_outsw(asm_instr *, u_char *, u_int, asm_processor *);
int op_jo(asm_instr *, u_char *, u_int, asm_processor *);
int op_jno(asm_instr *, u_char *, u_int, asm_processor *);
int op_jb(asm_instr *, u_char *, u_int, asm_processor *);
int op_jae(asm_instr *, u_char *, u_int, asm_processor *);
int op_je(asm_instr *, u_char *, u_int, asm_processor *);
int op_jne(asm_instr *, u_char *, u_int, asm_processor *);
int op_jbe(asm_instr *, u_char *, u_int, asm_processor *);
int op_ja(asm_instr *, u_char *, u_int, asm_processor *);
int op_js(asm_instr *, u_char *, u_int, asm_processor *);
int op_jns(asm_instr *, u_char *, u_int, asm_processor *);
int op_jp(asm_instr *, u_char *, u_int, asm_processor *);
int op_jnp(asm_instr *, u_char *, u_int, asm_processor *);
int op_jl(asm_instr *, u_char *, u_int, asm_processor *);
int op_jge(asm_instr *, u_char *, u_int, asm_processor *);
int op_jle(asm_instr *, u_char *, u_int, asm_processor *);
int op_jg(asm_instr *, u_char *, u_int, asm_processor *);
int op_immed_rmb_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_immed_rmb_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_immed_rmv_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_immed_rmv_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_test_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_test_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rmb_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rm_segr(asm_instr *, u_char *, u_int, asm_processor *);
int op_lea_rv_m(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_segr_rm(asm_instr *, u_char *, u_int, asm_processor *);
int op_pop_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_nop(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_xchg_eax_reg(asm_instr *, u_char *, u_int, asm_processor *);
int op_cwtl(asm_instr *, u_char *, u_int, asm_processor *);
int op_cltd(asm_instr *, u_char *, u_int, asm_processor *);
int op_fwait(asm_instr *, u_char *, u_int, asm_processor *);
int op_pushf(asm_instr *, u_char *, u_int, asm_processor *);
int op_popf(asm_instr *, u_char *, u_int, asm_processor *);
int op_sahf(asm_instr *, u_char *, u_int, asm_processor *);
int op_lahf(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_al_ref_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_eax_ref_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_ref_iv_al(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_ref_iv_eax(asm_instr *, u_char *, u_int, asm_processor *);
int op_movsb(asm_instr *, u_char *, u_int, asm_processor *);
int op_movsd(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmpsb(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmpsd(asm_instr *, u_char *, u_int, asm_processor *);
int op_test_al_rb(asm_instr *, u_char *, u_int, asm_processor *);
int op_test_eax_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_stosb(asm_instr *, u_char *, u_int, asm_processor *);
int op_stosd(asm_instr *, u_char *, u_int, asm_processor *);
int op_lodsb(asm_instr *, u_char *, u_int, asm_processor *);
int op_lodsd(asm_instr *, u_char *, u_int, asm_processor *);
int op_scasb(asm_instr *, u_char *, u_int, asm_processor *);
int op_scasd(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_subreg_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_reg_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_shr_rmb_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_shr_rmv_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_ret_i2(asm_instr *, u_char *, u_int, asm_processor *);
int op_ret(asm_instr *, u_char *, u_int, asm_processor *);
int op_les_rm_rmp(asm_instr *, u_char *, u_int, asm_processor *);
int op_lds_rm_rmp(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rmb_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_mov_rmv_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_enter(asm_instr *, u_char *, u_int, asm_processor *);
int op_leave(asm_instr *, u_char *, u_int, asm_processor *);
int op_retf_i2(asm_instr *, u_char *, u_int, asm_processor *);
int op_retf(asm_instr *, u_char *, u_int, asm_processor *);
int op_int_3(asm_instr *, u_char *, u_int, asm_processor *);
int op_int_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_into(asm_instr *, u_char *, u_int, asm_processor *);
int op_iret(asm_instr *, u_char *, u_int, asm_processor *);
int op_shift_rmb_1(asm_instr *, u_char *, u_int, asm_processor *);
int op_shift_rmv_1(asm_instr *, u_char *, u_int, asm_processor *);
int op_shift_rmb_cl(asm_instr *, u_char *, u_int, asm_processor *);
int op_shift_rmv_cl(asm_instr *, u_char *, u_int, asm_processor *);
int op_aam(asm_instr *, u_char *, u_int, asm_processor *);
int op_aad(asm_instr *, u_char *, u_int, asm_processor *);
int op_xlatb(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc0(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc1(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc2(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc3(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc4(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc5(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc6(asm_instr *, u_char *, u_int, asm_processor *);
int op_esc7(asm_instr *, u_char *, u_int, asm_processor *);
int op_loopne(asm_instr *, u_char *, u_int, asm_processor *);
int op_loope(asm_instr *, u_char *, u_int, asm_processor *);
int op_loop(asm_instr *, u_char *, u_int, asm_processor *);
int op_je_cxz(asm_instr *, u_char *, u_int, asm_processor *);
int op_in_al_ref_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_in_eax_ref_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_out_ref_ib_al(asm_instr *, u_char *, u_int, asm_processor *);
int op_out_ref_ib_eax(asm_instr *, u_char *, u_int, asm_processor *);
int op_call_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_jmp_iv(asm_instr *, u_char *, u_int, asm_processor *);
int op_jmp_ap(asm_instr *, u_char *, u_int, asm_processor *);
int op_jmp_ib(asm_instr *, u_char *, u_int, asm_processor *);
int op_in_al_dx(asm_instr *, u_char *, u_int, asm_processor *);
int op_in_eax_dx(asm_instr *, u_char *, u_int, asm_processor *);
int op_out_dx_al(asm_instr *, u_char *, u_int, asm_processor *);
int op_out_dx_eax(asm_instr *, u_char *, u_int, asm_processor *);
int op_lock(asm_instr *, u_char *, u_int, asm_processor *);
int op_lock(asm_instr *, u_char *, u_int, asm_processor *);
int op_repnz(asm_instr *, u_char *, u_int, asm_processor *);
int op_repz(asm_instr *, u_char *, u_int, asm_processor *);
int op_hlt(asm_instr *, u_char *, u_int, asm_processor *);
int op_cmc(asm_instr *, u_char *, u_int, asm_processor *);
int op_unary_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_unary_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_clc(asm_instr *, u_char *, u_int, asm_processor *);
int op_stc(asm_instr *, u_char *, u_int, asm_processor *);
int op_cli(asm_instr *, u_char *, u_int, asm_processor *);
int op_sti(asm_instr *, u_char *, u_int, asm_processor *);
int op_cld(asm_instr *, u_char *, u_int, asm_processor *);
int op_std(asm_instr *, u_char *, u_int, asm_processor *);
int op_incdec_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_indir_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_group6(asm_instr *, u_char *, u_int, asm_processor *);
int op_group7(asm_instr *, u_char *, u_int, asm_processor *);
int op_ud2a(asm_instr *, u_char *, u_int, asm_processor *);
int i386_rdtsc(asm_instr *, u_char *, u_int, asm_processor *);
int i386_mov_rm_cr(asm_instr *, u_char *, u_int, asm_processor *);
int i386_mov_cr_rm(asm_instr *, u_char *, u_int, asm_processor *);
int i386_mov_dr_rm(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovae(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmove(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovne(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovno(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovbe(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovb(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovo(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmova(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovs(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovns(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovp(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovnp(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovl(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovnl(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovle(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cmovnle(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jb(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jae(asm_instr *, u_char *, u_int, asm_processor *);
int i386_je(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jne(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jbe(asm_instr *, u_char *, u_int, asm_processor *);
int i386_ja(asm_instr *, u_char *, u_int, asm_processor *);
int i386_js(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jp(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jnp(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jns(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jl(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jge(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jle(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jg(asm_instr *, u_char *, u_int, asm_processor *);
int i386_jg(asm_instr *, u_char *, u_int, asm_processor *);
int op_setno_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setb_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setae_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_sete_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setne_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setbe_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_seta_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_sets_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setns_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setp_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setnp_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setl_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setge_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setle_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int op_setg_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int i386_push_fs(asm_instr *, u_char *, u_int, asm_processor *);
int i386_pop_fs(asm_instr *, u_char *, u_int, asm_processor *);
int i386_cpuid(asm_instr *, u_char *, u_int, asm_processor *);
int i386_bt_rm_r(asm_instr *, u_char *, u_int, asm_processor *);
int i386_xadd(asm_instr *, u_char *, u_int, asm_processor *);
int i386_group12(asm_instr *, u_char *, u_int, asm_processor *);
int i386_group14(asm_instr *, u_char *, u_int, asm_processor *);
int i386_group15(asm_instr *, u_char *, u_int, asm_processor *);
int i386_bswap(asm_instr *, u_char *, u_int, asm_processor *);
int i386_shld(asm_instr *, u_char *, u_int, asm_processor *);
int i386_shld_rmv_rv_cl(asm_instr *, u_char *, u_int, asm_processor *);
int i386_bts(asm_instr *, u_char *, u_int, asm_processor *);
int i386_shrd_rmv_rv_ib(asm_instr *, u_char *, u_int, asm_processor *);
int i386_shrd_rmv_rv_cl(asm_instr *, u_char *, u_int, asm_processor *);
int i386_imul_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_lss_rv_rmv(asm_instr *, u_char *, u_int, asm_processor *);
int op_btr_rmv_rv(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movzbl_rv_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movzwl_rv_rm2(asm_instr *, u_char *, u_int, asm_processor *);
int i386_bsf(asm_instr *, u_char *, u_int, asm_processor *);
int i386_bsr_rv_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movsbl_rv_rmb(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movswl_rv_rm2(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movd_pd_qd(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movq_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_movq_qq_pq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_pand_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_por_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_pxor_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_pmullw_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_paddusw_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_paddusb_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_punpcklbw_pq_qd(asm_instr *, u_char *, u_int, asm_processor *);
int i386_punpckhbw_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_packuswb_pq_qq(asm_instr *, u_char *, u_int, asm_processor *);
int i386_emms(asm_instr *, u_char *, u_int, asm_processor *);
int op_addsize(asm_instr *, u_char *, u_int, asm_processor *);
int i386_wbinvd(asm_instr *, u_char *, u_int, asm_processor *);
int i386_rdmsr(asm_instr *, u_char *, u_int, asm_processor *);
int i386_btrl(asm_instr *, u_char *, u_int, asm_processor *);
int i386_xstorenrg(asm_instr *, u_char *, u_int, asm_processor *);
#endif
