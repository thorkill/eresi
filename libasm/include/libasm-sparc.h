/*
** 
** sparc.h in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Dec  2 22:43:08 2003
** Updated : Thu Dec  4 03:29:25 2003
**
** $Id: libasm-sparc.h,v 1.14 2008/01/07 01:29:53 heroine Exp $
**
*/

#ifndef LIBASM_SPARC_H_
#define LIBASM_SPARC_H_

#include <libasm-sparc-decode.h>

#define MGETBIT(val, shift)	(((val) >> shift) & 1)

#define ASM_FREG_FSR -1 /* Nasty little hack */
#define ASM_SP_ASI_P 0x80 /* Primary ASI */
#define ASM_SP_ASI_P_L 0x88 /* Primary ASI little-endian */

/* SPARC Instruction decoders */
int asm_sparc_add (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_addc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_addcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_addccc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_and (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_andcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_andn (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_andncc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_bicc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_bpcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_bpr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_call (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_casa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_casxa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_done (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fbfcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fbpfcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fcmpd (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fcmped (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fcmpeq (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fcmpes (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fcmpq (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fcmps (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_flush (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_flushw (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fmovdcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fmovdr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fmovqcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fmovqr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fmovscc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fmovsr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_fpop1 (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_hu (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_hu2 (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_illegal (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_illtrap (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_impdep1 (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_impdep2 (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_jmpl (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldd (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldda (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lddf (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lddfa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldf (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldfa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldfsr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldqf (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldqfa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldsb (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldsba (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldsh (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldsha (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldstub (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldstuba (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldsw (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldswa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldub (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lduba (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lduh (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lduha (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lduw (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_lduwa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldx (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_ldxa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_movcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_movr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_mulscc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_mulx (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_or (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_orcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_orn (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_orncc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_popc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_prefetch (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_prefetcha (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_rd (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_rdpr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_restore (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_return (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_save (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_saved (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sdiv (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sdivcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sdivx (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sethi (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sll (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_smul (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_smulcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sra (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_srl (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stb (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stba (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_std (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stda (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stdf (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stdfa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stf (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stfa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stfsr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sth (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stha (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stqf (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stqfa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stw (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stwa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stx (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_stxa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_sub (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_subc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_subcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_subccc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_swap (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_swapa (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_taddcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_taddcctv (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_tcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_tsubcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_tsubcctv (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_udiv (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_udivcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_udivx (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_umul (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_umulcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_wr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_wrpr (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_xnor (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_xnorcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_xor (asm_instr *, unsigned char *, unsigned int, asm_processor *);
int asm_sparc_xorcc (asm_instr *, unsigned char *, unsigned int, asm_processor *);

void sparc_convert_pbranch(struct s_decode_pbranch *, unsigned char *);
void sparc_convert_rbranch(struct s_decode_rbranch *, unsigned char *);
void sparc_convert_branch(struct s_decode_branch *, unsigned char *);
void sparc_convert_call(struct s_decode_call *, unsigned char *);
void sparc_convert_format3(struct s_decode_format3 *, unsigned char *);
void sparc_convert_format4(struct s_decode_format4 *, unsigned char *);

void asm_resolve_sparc(void *, unsigned int, char *, unsigned int);

/* Get operand name */
char *asm_sparc_get_op_name (asm_operand *op);

char *get_sparc_register(int reg);
char *get_sparc_sregister(int reg);
char *get_sparc_pregister(int reg);
char *get_sparc_fregister(int reg);
char *get_sparc_cc(int cc);

/* SPARC operand handlers (ie. fetchers) */
int asm_sparc_op_fetch (asm_operand *operand, unsigned char *opcode, 
                        int otype, asm_instr *ins);

int asm_sparc_op_fetch_register (asm_operand *operand, unsigned char *opcode,
                                  int otype, asm_instr *ins);
int asm_sparc_op_fetch_immediate (asm_operand *operand, unsigned char *opcode,
                                   int otype, asm_instr *ins);
int asm_sparc_op_fetch_displacement (asm_operand *operand, unsigned char *opcode,
                                      int otype, asm_instr *ins);
int asm_sparc_op_fetch_disp30 (asm_operand *operand, unsigned char *opcode,
                               int otype, asm_instr *ins);
int asm_sparc_op_fetch_sethi (asm_operand *operand, unsigned char *opcode,
                               int otype, asm_instr *ins);
int asm_sparc_op_fetch_fregister (asm_operand *operand, unsigned char *opcode,
                                  int otype, asm_instr *ins);
int asm_sparc_op_fetch_sregister (asm_operand *operand, unsigned char *opcode,
                                  int otype, asm_instr *ins);
int asm_sparc_op_fetch_pregister (asm_operand *operand, unsigned char *opcode,
                                  int otype, asm_instr *ins);
int asm_sparc_op_fetch_cc (asm_operand *operand, unsigned char *opcode,
                             int otype, asm_instr *ins);
int asm_sparc_op_fetch_imm_address (asm_operand *operand, unsigned char *opcode,
                                    int otype, asm_instr *ins);
int asm_sparc_op_fetch_reg_address (asm_operand *operand, unsigned char *opcode,
                                    int otype, asm_instr *ins);

/**
 * Registration functions.
 *
 */

int	asm_register_sparc();

/*****
 * sparc processor internals
 */

struct s_asm_ins_sparc {
  int	instr;
  asm_operand	op1;
  asm_operand	op2;
  asm_operand	op3;
};

/**
 * SPARC "flags" (ie. condition codes) 
 */
enum e_sparc_flags {
  /* Flags inside the integer condition codes (icc, xcc) */
  ASM_SP_FLAG_C = 1 << 0, // Carry
  ASM_SP_FLAG_V = 1 << 1, // oVerflow
  ASM_SP_FLAG_Z = 1 << 2, // Zero
  ASM_SP_FLAG_N = 1 << 3, // Negative
  /* Floating-point condition codes */
  ASM_SP_FLAG_FCC0 = 1 << 4,
  ASM_SP_FLAG_FCC1 = 1 << 5,
  ASM_SP_FLAG_FCC2 = 1 << 6,
  ASM_SP_FLAG_FCC3 = 1 << 7
};


struct s_asm_proc_sparc {
  /* handlers for x86 instructions referenced by opcode	*/
  int *bcc_table;
  int *brcc_table;
  int *fbcc_table;
  int *shift_table;
  int *movcc_table;
  int *movfcc_table;
  int *movr_table;
  int *fpop1_table;
  int *fmovcc_table;
  int *fmovfcc_table;
  int *fmovr_table;
  int *fcmp_table;
  int *tcc_table;
  int *op2_table;
  int *op3_table;
};


enum e_sparc_gregisters {
  ASM_REG_R0,
  ASM_REG_R1
} ;

enum e_sparc_sregisters {
  ASM_REG_G0,
  ASM_REG_G1,
  ASM_REG_G2,
  ASM_REG_G3,
  ASM_REG_G4,
  ASM_REG_G5,
  ASM_REG_G6,
  ASM_REG_G7,
  ASM_REG_O0,
  ASM_REG_O1,
  ASM_REG_O2,
  ASM_REG_O3,
  ASM_REG_O4,
  ASM_REG_O5,
  ASM_REG_O6,
  ASM_REG_O7,
  ASM_REG_L0,
  ASM_REG_L1,
  ASM_REG_L2,
  ASM_REG_L3,
  ASM_REG_L4,
  ASM_REG_L5,
  ASM_REG_L6,
  ASM_REG_L7,
  ASM_REG_I0,
  ASM_REG_I1,
  ASM_REG_I2,
  ASM_REG_I3,
  ASM_REG_I4,
  ASM_REG_I5,
  ASM_REG_I6,
  ASM_REG_I7
} ;

enum e_sparc_state_registers {
  ASM_SREG_Y,
  ASM_SREG_BAD,
  ASM_SREG_CCR,
  ASM_SREG_ASI,
  ASM_SREG_TICK,
  ASM_SREG_PC,
  ASM_SREG_FPRS
  /* Values from 7 to 31 are either unused or implementation-specific */
};

enum e_sparc_privileged_registers {
  ASM_PREG_TPC,
  ASM_PREG_TNPC,
  ASM_PREG_TSTATE,
  ASM_PREG_TT,
  ASM_PREG_TICK,
  ASM_PREG_TBA,
  ASM_PREG_PSTATE,
  ASM_PREG_TL,
  ASM_PREG_PIL,
  ASM_PREG_CWP,
  ASM_PREG_CANSAVE,
  ASM_PREG_CANRESTORE,
  ASM_PREG_CLEANWIN,
  ASM_PREG_OTHERWIN,
  ASM_PREG_WSTATE,
  ASM_PREG_FQ,
  ASM_PREG_BAD16, /* Values from 16 to 30 are reserved */
  ASM_PREG_BAD17,
  ASM_PREG_BAD18,
  ASM_PREG_BAD19,
  ASM_PREG_BAD20,
  ASM_PREG_BAD21,
  ASM_PREG_BAD22,
  ASM_PREG_BAD23,
  ASM_PREG_BAD24,
  ASM_PREG_BAD25,
  ASM_PREG_BAD26,
  ASM_PREG_BAD27,
  ASM_PREG_BAD28,
  ASM_PREG_BAD29,
  ASM_PREG_BAD30,
  ASM_PREG_VER
};

enum e_sparc_condition_codes {
  ASM_SP_FCC0,
  ASM_SP_FCC1,
  ASM_SP_FCC2,
  ASM_SP_FCC3,
  ASM_SP_ICC,
  ASM_SP_BADCC, /* 5 and 7 are reserved */
  ASM_SP_XCC  
};

/**
 * Instruction family, depending on op field
 */
enum e_sparc_opcode {
  ASM_SPARC_OP0_BRANCH,
  ASM_SPARC_OP1_CALL,
  ASM_SPARC_OP2_FORMAT3,
  ASM_SPARC_OP3_FORMAT3
};

/* Possible values for SPARC operands' content attribute */
enum e_sparc_operand {
  ASM_SP_OTYPE_REGISTER,
  ASM_SP_OTYPE_IMMEDIATE,
  ASM_SP_OTYPE_DISPLACEMENT,
  ASM_SP_OTYPE_DISP30,
  ASM_SP_OTYPE_SETHI,
  ASM_SP_OTYPE_FREGISTER,
  ASM_SP_OTYPE_SREGISTER,
  ASM_SP_OTYPE_PREGISTER,
  ASM_SP_OTYPE_CC,
  ASM_SP_OTYPE_IMM_ADDRESS, /* [ r[rs1] + imm ] */
  ASM_SP_OTYPE_REG_ADDRESS, /* [ r[rs1] + r[rs2] ] */ 

  ASM_SP_OTYPE_NUM /* This element must be the last in the enum */
};


/***
 * Instruction list
 * 
 * Last instruction must be ASM_SP_BAD
 */
enum e_sparc_instr {
  ASM_SP_,
  
  ASM_SP_ADD, /* ADD */
  ASM_SP_ADDCC, /* ADDcc */
  ASM_SP_ADDC, /* ADDC */
  ASM_SP_ADDCCC, /* ADDCcc */
  ASM_SP_AND, /* AND */
  ASM_SP_ANDCC, /* ANDcc */
  ASM_SP_ANDN, /* ANDN */
  ASM_SP_ANDNCC, /* ANDNcc */
  ASM_SP_BA, /* BPcc, Bicc */
  ASM_SP_BN,
  ASM_SP_BNE,
  ASM_SP_BE,
  ASM_SP_BG,
  ASM_SP_BLE,
  ASM_SP_BGE,
  ASM_SP_BL,
  ASM_SP_BGU,
  ASM_SP_BLEU,
  ASM_SP_BCC,
  ASM_SP_BCS,
  ASM_SP_BPOS,
  ASM_SP_BNEG,
  ASM_SP_BVC,
  ASM_SP_BVS,
  ASM_SP_BRZ, /* BPr */
  ASM_SP_BRLEZ,
  ASM_SP_BRLZ,
  ASM_SP_BRNZ,
  ASM_SP_BRGZ,
  ASM_SP_BRGEZ,
  ASM_SP_CALL, /* CALL */
  ASM_SP_CASA, /* CASA */
  ASM_SP_CASXA, /* CASXA */
  ASM_SP_DONE, /* DONE */
  ASM_SP_FABSS, /* FABS(s,d,q) */
  ASM_SP_FABSD,
  ASM_SP_FABSQ,
  ASM_SP_FADDS, /* FADD(s,d,q) */
  ASM_SP_FADDD,
  ASM_SP_FADDQ,
  ASM_SP_FBA, /* FBfcc, FBPfcc */
  ASM_SP_FBN,
  ASM_SP_FBU,
  ASM_SP_FBG,
  ASM_SP_FBUG,
  ASM_SP_FBL,
  ASM_SP_FBUL,
  ASM_SP_FBLG,
  ASM_SP_FBNE,
  ASM_SP_FBE,
  ASM_SP_FBUE,
  ASM_SP_FBGE,
  ASM_SP_FBUGE,
  ASM_SP_FBLE,
  ASM_SP_FBULE,
  ASM_SP_FBO,
  ASM_SP_FCMPS, /* FCMP(s,d,q) */
  ASM_SP_FCMPD,
  ASM_SP_FCMPQ,
  ASM_SP_FCMPES, /* FCMPE(s,d,q) */
  ASM_SP_FCMPED,
  ASM_SP_FCMPEQ,
  ASM_SP_FDIVS, /* FDIV(s,d,q) */
  ASM_SP_FDIVD,
  ASM_SP_FDIVQ,
  ASM_SP_FDMULQ, /* FdMULq */
  ASM_SP_FITOS, /* FiTO(s,d,q) */
  ASM_SP_FITOD,
  ASM_SP_FITOQ,
  ASM_SP_FLUSH, /* FLUSH */
  ASM_SP_FLUSHW, /* FLUSHW */
  ASM_SP_FMOVS, /* FMOV(s,d,q) */
  ASM_SP_FMOVD,
  ASM_SP_FMOVQ,
  ASM_SP_FMOVSA, /* FMOV(s,d,q)cc */
  ASM_SP_FMOVSN,
  ASM_SP_FMOVSNE,
  ASM_SP_FMOVSE,
  ASM_SP_FMOVSG,
  ASM_SP_FMOVSLE,
  ASM_SP_FMOVSGE,
  ASM_SP_FMOVSL,
  ASM_SP_FMOVSGU,
  ASM_SP_FMOVSLEU,
  ASM_SP_FMOVSCC,
  ASM_SP_FMOVSCS,
  ASM_SP_FMOVSPOS,
  ASM_SP_FMOVSNEG,
  ASM_SP_FMOVSVC,
  ASM_SP_FMOVSVS,
  ASM_SP_FMOVSU,
  ASM_SP_FMOVSUG,
  ASM_SP_FMOVSUL,
  ASM_SP_FMOVSLG,
  ASM_SP_FMOVSUE,
  ASM_SP_FMOVSUGE,
  ASM_SP_FMOVSULE,
  ASM_SP_FMOVSO,
  ASM_SP_FMOVDA,
  ASM_SP_FMOVDN,
  ASM_SP_FMOVDNE,
  ASM_SP_FMOVDE,
  ASM_SP_FMOVDG,
  ASM_SP_FMOVDLE,
  ASM_SP_FMOVDGE,
  ASM_SP_FMOVDL,
  ASM_SP_FMOVDGU,
  ASM_SP_FMOVDLEU,
  ASM_SP_FMOVDCC,
  ASM_SP_FMOVDCS,
  ASM_SP_FMOVDPOS,
  ASM_SP_FMOVDNEG,
  ASM_SP_FMOVDVC,
  ASM_SP_FMOVDVS,
  ASM_SP_FMOVDU,
  ASM_SP_FMOVDUG,
  ASM_SP_FMOVDUL,
  ASM_SP_FMOVDLG,
  ASM_SP_FMOVDUE,
  ASM_SP_FMOVDUGE,
  ASM_SP_FMOVDULE,
  ASM_SP_FMOVDO,
  ASM_SP_FMOVQA,
  ASM_SP_FMOVQN,
  ASM_SP_FMOVQNE,
  ASM_SP_FMOVQE,
  ASM_SP_FMOVQG,
  ASM_SP_FMOVQLE,
  ASM_SP_FMOVQGE,
  ASM_SP_FMOVQL,
  ASM_SP_FMOVQGU,
  ASM_SP_FMOVQLEU,
  ASM_SP_FMOVQCC,
  ASM_SP_FMOVQCS,
  ASM_SP_FMOVQPOS,
  ASM_SP_FMOVQNEG,
  ASM_SP_FMOVQVC,
  ASM_SP_FMOVQVS,
  ASM_SP_FMOVQU,
  ASM_SP_FMOVQUG,
  ASM_SP_FMOVQUL,
  ASM_SP_FMOVQLG,
  ASM_SP_FMOVQUE,
  ASM_SP_FMOVQUGE,
  ASM_SP_FMOVQULE,
  ASM_SP_FMOVQO,  
  ASM_SP_FMOVRSZ, /* FMOV(s,d,q)r */ 
  ASM_SP_FMOVRSLEZ,
  ASM_SP_FMOVRSLZ,
  ASM_SP_FMOVRSNZ,
  ASM_SP_FMOVRSGZ,
  ASM_SP_FMOVRSGEZ,
  ASM_SP_FMOVRDZ, 
  ASM_SP_FMOVRDLEZ,
  ASM_SP_FMOVRDLZ,
  ASM_SP_FMOVRDNZ,
  ASM_SP_FMOVRDGZ,
  ASM_SP_FMOVRDGEZ,
  ASM_SP_FMOVRQZ, 
  ASM_SP_FMOVRQLEZ,
  ASM_SP_FMOVRQLZ,
  ASM_SP_FMOVRQNZ,
  ASM_SP_FMOVRQGZ,
  ASM_SP_FMOVRQGEZ,
  ASM_SP_FMULS, /* FMUL(s,d,q) */
  ASM_SP_FMULD,
  ASM_SP_FMULQ,
  ASM_SP_FNEGS, /* FNEG(s,d,q) */
  ASM_SP_FNEGD,
  ASM_SP_FNEGQ,
  ASM_SP_FSMULD, /* FsMULd */
  ASM_SP_FSQRTS, /* FSQRT(s,d,q) */
  ASM_SP_FSQRTD,
  ASM_SP_FSQRTQ,
  ASM_SP_FSTOI, /* F(s,d,q)TOi */
  ASM_SP_FDTOI,
  ASM_SP_FQTOI,
  ASM_SP_FSTOD, /* F(s,d,q)TO(s,d,q) */
  ASM_SP_FSTOQ,
  ASM_SP_FDTOS,
  ASM_SP_FDTOQ,
  ASM_SP_FQTOS,
  ASM_SP_FQTOD,
  ASM_SP_FSTOX, /* F(s,d,q)TOx */
  ASM_SP_FDTOX,
  ASM_SP_FQTOX,
  ASM_SP_FSUBS, /* FSUB(s,d,q) */
  ASM_SP_FSUBD,
  ASM_SP_FSUBQ,
  ASM_SP_FXTOS, /* FxTO(s,d,q) */
  ASM_SP_FXTOD,
  ASM_SP_FXTOQ,
  ASM_SP_ILLTRAP, /* ILLTRAP */
  ASM_SP_IMPDEP1, /* IMPDEP1 */
  ASM_SP_IMPDEP2, /* IMPDEP2 */
  ASM_SP_JMPL, /* JMPL, RET, RETL */
  ASM_SP_LDD, /* LDD */
  ASM_SP_LDDA, /* LDDA */
  ASM_SP_LDDF, /* LDFF */
  ASM_SP_LDDFA, /* LDDFA */
  ASM_SP_LDF, /* LDF */
  ASM_SP_LDFA, /* LDFA */
  ASM_SP_LDFSR, /* LDFSR */
  ASM_SP_LDQF, /* LDQF */
  ASM_SP_LDQFA, /* LDQFA */
  ASM_SP_LDSB, /* LDSB */
  ASM_SP_LDSBA, /* LDSBA */
  ASM_SP_LDSH, /* LDSH */
  ASM_SP_LDSHA, /* LDSHA */
  ASM_SP_LDSTUB, /* LDSTUB */
  ASM_SP_LDSTUBA, /* LDSTUBA */
  ASM_SP_LDSW, /* LDSW */
  ASM_SP_LDSWA, /* LDSWA */
  ASM_SP_LDUB, /* LDUB */
  ASM_SP_LDUBA, /*LDUBA */
  ASM_SP_LDUH, /* LDUH */
  ASM_SP_LDUHA, /* LDUHA */
  ASM_SP_LDUW, /* LDUW */
  ASM_SP_LDUWA, /* LDUWA */
  ASM_SP_LDX, /* LDX */
  ASM_SP_LDXA, /* LDXA */
  ASM_SP_LDXFSR, /* LDXFSR */
  ASM_SP_MEMBAR, /* MEMBAR */
  ASM_SP_MOVA, /* MOVcc */
  ASM_SP_MOVN,
  ASM_SP_MOVNE,
  ASM_SP_MOVE,
  ASM_SP_MOVG,
  ASM_SP_MOVLE,
  ASM_SP_MOVGE,
  ASM_SP_MOVL,
  ASM_SP_MOVGU,
  ASM_SP_MOVLEU,
  ASM_SP_MOVCC,
  ASM_SP_MOVCS,
  ASM_SP_MOVPOS,
  ASM_SP_MOVNEG,
  ASM_SP_MOVVC,
  ASM_SP_MOVVS,
  ASM_SP_MOVU,
  ASM_SP_MOVUG,
  ASM_SP_MOVUL,
  ASM_SP_MOVLG,
  ASM_SP_MOVUE,
  ASM_SP_MOVUGE,
  ASM_SP_MOVULE,
  ASM_SP_MOVO,
  ASM_SP_MOVRZ, /* MOVr */
  ASM_SP_MOVRLEZ,
  ASM_SP_MOVRLZ,
  ASM_SP_MOVRNZ,
  ASM_SP_MOVRGZ,
  ASM_SP_MOVRGEZ,
  ASM_SP_MULSCC, /* MULScc */
  ASM_SP_MULX, /* MULX */
  ASM_SP_NOP, /* NOP */
  ASM_SP_OR, /* OR */
  ASM_SP_ORCC, /* ORcc */
  ASM_SP_ORN, /* ORC */
  ASM_SP_ORNCC, /* ORNcc */
  ASM_SP_POPC, /* POPC */
  ASM_SP_PREFETCH, /* PREFETCH */
  ASM_SP_PREFETCHA, /* PREFETCHA */
  ASM_SP_RD, /* RDASI, RDASR, RDCCR, RDFPRS, RDPC, RDPR, RDTICK, RDY */
  ASM_SP_RDPR, /* RDPR */
  ASM_SP_RESTORE, /* RESTORE */
  ASM_SP_RESTORED, /* RESTORED */
  ASM_SP_RETRY, /* RETRY */
  ASM_SP_RETURN, /* RETURN */
  ASM_SP_SAVE, /* SAVE */
  ASM_SP_SAVED, /* SAVED */
  ASM_SP_SDIV, /* SDIV */
  ASM_SP_SDIVCC, /* SDIVcc */
  ASM_SP_SDIVX, /* SDIVX */
  ASM_SP_SETHI, /* SETHI */
  ASM_SP_SIR, /* SIR */
  ASM_SP_SLL, /* SLL */
  ASM_SP_SLLX, /* SLLX */
  ASM_SP_SMUL, /* SMUL */
  ASM_SP_SMULCC, /* SMULcc */
  ASM_SP_SQRTS, /* SQRT(s,d,q) */
  ASM_SP_SQRTD,
  ASM_SP_SQRTQ,
  ASM_SP_SRA, /* SRA */
  ASM_SP_SRAX, /* SRAX */
  ASM_SP_SRL, /* SRL */
  ASM_SP_SRLX, /* SRLX */
  ASM_SP_STB, /* STB */
  ASM_SP_STBA, /* STBA */
  ASM_SP_STBAR, /* STBAR */
  ASM_SP_STD, /* STD */
  ASM_SP_STDA, /* STDA */
  ASM_SP_STDF, /* STDF */
  ASM_SP_STDFA, /* STDFA */
  ASM_SP_STF, /* STF */
  ASM_SP_STFA, /* STFA */
  ASM_SP_STFSR, /* STFSR */
  ASM_SP_STH, /* STH */
  ASM_SP_STHA, /* STHA */
  ASM_SP_STQF, /* STQF */
  ASM_SP_STQFA, /* STQFA */
  ASM_SP_STW, /* STW */
  ASM_SP_STWA, /* STWA */
  ASM_SP_STX, /* STX */
  ASM_SP_STXA, /* STXA */
  ASM_SP_STXFSR, /* STXFSR */
  ASM_SP_SUB, /* SUB */
  ASM_SP_SUBCC, /* SUBcc */
  ASM_SP_SUBC, /* SUBC */
  ASM_SP_SUBCCC, /* SUBCcc */
  ASM_SP_SWAP, /* SWAP */
  ASM_SP_SWAPA, /* SWAPA */
  ASM_SP_TADDCC, /* TADDcc */
  ASM_SP_TADDCCTV, /* TADDccTV */
  ASM_SP_TA, /* Tcc */
  ASM_SP_TN,
  ASM_SP_TNE,
  ASM_SP_TE,
  ASM_SP_TG,
  ASM_SP_TLE,
  ASM_SP_TGE,
  ASM_SP_TL,
  ASM_SP_TGU,
  ASM_SP_TLEU,
  ASM_SP_TCC,
  ASM_SP_TCS,
  ASM_SP_TPOS,
  ASM_SP_TNEG,
  ASM_SP_TVC,
  ASM_SP_TVS,
  ASM_SP_TSUBCC, /* TSUBcc */
  ASM_SP_TSUBCCTV, /* TSUBccTV */
  ASM_SP_UDIV, /* UDIV */
  ASM_SP_UDIVCC, /* UDIVcc */
  ASM_SP_UDIVX, /* UDIVX */
  ASM_SP_UMUL, /* UMUL */
  ASM_SP_UMULCC, /* UMULcc */
  ASM_SP_WR, /* WRASI, WRASR, WRCCR, WRFPRS, WRY */
  ASM_SP_WRPR, /* WRPR */
  ASM_SP_XNOR, /* XNOR */
  ASM_SP_XNORCC, /* XNORcc */
  ASM_SP_XOR, /* XOR */
  ASM_SP_XORCC, /* XORcc */
  ASM_SP_CMP, /* Synthetics */
  ASM_SP_JMP,
  ASM_SP_IPREFETCH,
  ASM_SP_TST,
  ASM_SP_RET,
  ASM_SP_RETL,
  ASM_SP_SETUW,
  ASM_SP_SET,
  ASM_SP_SETSW,
  ASM_SP_SETX,
  ASM_SP_SIGNX,
  ASM_SP_NOT,
  ASM_SP_NEG,
  ASM_SP_CAS,
  ASM_SP_CASL,
  ASM_SP_CASX,
  ASM_SP_CASXL,
  ASM_SP_INC,
  ASM_SP_INCCC,
  ASM_SP_DEC,
  ASM_SP_DECCC,
  ASM_SP_BTST,
  ASM_SP_BSET,
  ASM_SP_BCLR,
  ASM_SP_BTOG,
  ASM_SP_CLR,
  ASM_SP_CLRB,
  ASM_SP_CLRH,
  ASM_SP_CLRX,
  ASM_SP_CLRUW,
  ASM_SP_MOV,
    
  ASM_SP_BAD
};

extern char *sparc_instr_list[ASM_SP_BAD + 1];
extern int sparc_bcc_list[16];
extern int sparc_brcc_list[8];
extern int sparc_fbcc_list[16];
extern int sparc_shift_list[6];
extern int sparc_movcc_list[16];
extern int sparc_movfcc_list[16];
extern int sparc_movr_list[8];
extern int sparc_fpop1_list[256];
extern int sparc_fmovcc_list[48];
extern int sparc_fmovfcc_list[48];
extern int sparc_fmovr_list[24];
extern int sparc_fcmp_list[8];
extern int sparc_tcc_list[16];
extern int sparc_op2_table[64];
extern int sparc_op3_table[64];

#endif
