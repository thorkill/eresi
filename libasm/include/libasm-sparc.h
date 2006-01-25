/*
** 
** sparc.h in 
** 
** Author  : <sk at devhell dot org>
** Started : Tue Dec  2 22:43:08 2003
** Updated : Thu Dec  4 03:29:25 2003
*/

#ifndef LIBASM_SPARC_H_
#define LIBASM_SPARC_H_

#include <libasm-sparc-decode.h>

#define MGETBIT(val, shift)	(((val) >> shift) & 1)

int	sparc_type_instr(int instr);
void	sparc_instruction2synthetic(asm_instr *ins);
int	sparc_decode_branch(asm_instr *, u_char *, u_int, asm_processor *);
int	sparc_decode_other(asm_instr *, u_char *, u_int, asm_processor *);
int	sparc_decode_sethi(asm_instr *, u_char *, u_int, asm_processor *);
int	sparc_decode_call(asm_instr *, u_char *, u_int, asm_processor *);
int	sparc_decode_format3_rs(asm_instr *, u_char *, u_int, asm_processor *);
int	sparc_decode_format3_imm(asm_instr *, u_char *, u_int, asm_processor *);

void	sparc_convert_branch(struct s_decode_branch *, u_char *, asm_processor *);
void    sparc_convert_sethi(struct s_decode_sethi *, u_char *, asm_processor *);
void	sparc_convert_call(struct s_decode_call	*, u_char *, asm_processor *);
void	sparc_convert_format3_rs(struct s_decode_f3rs *, u_char *, asm_processor *);
void	sparc_convert_format3_imm(struct s_decode_f3imm	*, u_char *, asm_processor *);

void	asm_resolve_sparc(void *, u_int, char *, u_int);


/*****
 * sparc processor internal
 *
 */

struct s_asm_ins_sparc {
  int	instr;
  asm_operand	op1;
  asm_operand	op2;
  asm_operand	op3;
};

struct s_asm_proc_sparc {
  /* compatibility				*/
  /* processor state: oplen actived or not	*/
    int			(*get_vect_size)(asm_processor *);
  
  /* handlers for x86 instructions referenced by opcode	*/
  int	*format0_branch_table;
  int	*format3_op2_table;
  int	*format3_op3_table;
};


enum {
  ASM_REG_R0,
  ASM_REG_R1
  

} e_sparc_gregisters;

enum {
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
} e_sparc_sregisters;
/**
 * Instruction family, depending on op field
 *
 */

enum {
  ASM_SPARC_OP0_BRANCH,
  ASM_SPARC_OP1_CALL,
  ASM_SPARC_OP2_FORMAT3,
  ASM_SPARC_OP3_FORMAT3
} e_sparc_opcode;


enum {
  ASM_SP_OTYPE_REGISTER,
  ASM_SP_OTYPE_IMMEDIATE,
  ASM_SP_OTYPE_DISPLACEMENT,
  ASM_SP_OTYPE_DISP30,
  ASM_SP_OTYPE_SETHI,
  ASM_SP_OTYPE_FREGISTER
};



/***
 * Instruction list
 *
 *
 * Last Instruction must be ASM_SP_BAD
 */

enum {
  ASM_SP_,
  ASM_SP_ADD,
  ASM_SP_AND,
  ASM_SP_OR,
  ASM_SP_XOR,
  ASM_SP_SUB,
  ASM_SP_ANDN,
  ASM_SP_ORN,
  ASM_SP_XNOR,
  ASM_SP_ADDX,
  ASM_SP_SUBX,
  ASM_SP_ADDCC,
  ASM_SP_ANDCC,
  ASM_SP_ORCC,
  ASM_SP_XORCC,
  ASM_SP_SUBCC,
  ASM_SP_ANDNCC,
  ASM_SP_ORNCC,
  ASM_SP_XNORCC,
  ASM_SP_ADDXCC,
  ASM_SP_SUBXCC,
  ASM_SP_TADDCC,
  ASM_SP_TSUBCC,
  ASM_SP_TADDCCTV,
  ASM_SP_TSUBCCTV,
  ASM_SP_MULSCC,
  ASM_SP_SLL,
  ASM_SP_SRL,
  ASM_SP_SRA,
  ASM_SP_RDY,
  ASM_SP_RDPSR,
  ASM_SP_RDWIM,
  ASM_SP_RDTBR,
  ASM_SP_WRY,
  ASM_SP_WRPSR,
  ASM_SP_WRWIM,
  ASM_SP_WRTBR,
  ASM_SP_FPOP1,
  ASM_SP_FPOP2,
  ASM_SP_CPOP1,
  ASM_SP_CPOP2,
  ASM_SP_JMPL,
  ASM_SP_RETT,
  ASM_SP_TICC,
  ASM_SP_IFLUSH,
  ASM_SP_SAVE,
  ASM_SP_RESTORE,
  ASM_SP_LD,
  ASM_SP_LDUB,
  ASM_SP_LDUH,
  ASM_SP_LDD,
  ASM_SP_ST,
  ASM_SP_STB,
  ASM_SP_STH,
  ASM_SP_STD,
  ASM_SP_LDSB,
  ASM_SP_LDSH,
  ASM_SP_LDSTUB,
  ASM_SP_SWAP,
  ASM_SP_LDA,
  ASM_SP_LDUBA,
  ASM_SP_LDUHA,
  ASM_SP_LDDA,
  ASM_SP_STAC,
  ASM_SP_STBA,
  ASM_SP_STHA,
  ASM_SP_STDA,
  ASM_SP_LDSBA,
  ASM_SP_LDSHA,
  ASM_SP_LDSTUBA,
  ASM_SP_SWAPA,
  ASM_SP_LDF,
  ASM_SP_LDFSR,
  ASM_SP_LDDF,
  ASM_SP_STF,
  ASM_SP_STFSR,
  ASM_SP_STDFQ,
  ASM_SP_STDF,
  ASM_SP_LDC,
  ASM_SP_LDCSR,
  ASM_SP_LDDC,
  ASM_SP_STC,
  ASM_SP_STCSR,
  ASM_SP_STDCQ,
  ASM_SP_STDC,
  ASM_SP_BN,
  ASM_SP_BZ,
  ASM_SP_BLE,
  ASM_SP_BL,
  ASM_SP_BLEU,
  ASM_SP_BCS,
  ASM_SP_BNEG,
  ASM_SP_BVS,
  ASM_SP_BBA,
  ASM_SP_BNE,
  ASM_SP_BG,
  ASM_SP_BGE,
  ASM_SP_BGU,
  ASM_SP_BCC,
  ASM_SP_BPOS,
  ASM_SP_BVC,
  ASM_SP_SETHI,
  ASM_SP_CALL,
  ASM_SP_NOP,
  ASM_SP_TST,
  ASM_SP_CLR,
  ASM_SP_CMP,
  ASM_SP_JMP,
  ASM_SP_RET,
  ASM_SP_RETL,
  ASM_SP_MOV,
  ASM_SP_BAD
} e_sparc_instr;

extern char	*sparc_instr_list[ASM_SP_BAD + 1];
extern int	sparc_cond_list[16];
extern int	format3_op2_table[64];
extern int	format3_op3_table[64];


#endif

