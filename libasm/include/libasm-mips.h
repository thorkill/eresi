/**
 * $Id: libasm-mips.h,v 1.1 2007-01-23 09:53:03 heroine Exp $
 *
 *
 */
#ifndef LIBASM_MIPS_H_
#define LIBASM_MIPS_H_
#include <libasm-mips-structs.h>
int	mips_extract_opcode(int);
int	mips_decode_rtype(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc);
int	mips_decode_itype(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc);
int	mips_decode_jtype(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc);
int	mips_decode_ctype(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc);

void	mips_convert_format_r(struct s_mips_decode_reg *, u_char *);
void	mips_convert_format_i(struct s_mips_decode_imm *, u_char *);
void	mips_convert_format_j(struct s_mips_decode_jump *, u_char *);

char	*asm_mips_display_instr(asm_instr *, int vaddr);

enum 
{
  ASM_MIPS_,
  ASM_MIPS_ADD,
  ASM_MIPS_ADDI,
  ASM_MIPS_ADDIU,
  ASM_MIPS_ADDU,
  ASM_MIPS_AND,
  ASM_MIPS_ANDI,
  ASM_MIPS_BEQ,
  ASM_MIPS_BGTZ,
  ASM_MIPS_BLEZ,
  ASM_MIPS_BNE,
  ASM_MIPS_DIV,
  ASM_MIPS_DIVU,
  ASM_MIPS_J,
  ASM_MIPS_JAL,
  ASM_MIPS_JALR,
  ASM_MIPS_JR,
  ASM_MIPS_LB,
  ASM_MIPS_LBU,
  ASM_MIPS_LH,
  ASM_MIPS_LHI,
  ASM_MIPS_LHU,
  ASM_MIPS_LO,
  ASM_MIPS_LW,
  ASM_MIPS_MFHI,
  ASM_MIPS_MFLO,
  ASM_MIPS_MTHI,
  ASM_MIPS_MTLO,
  ASM_MIPS_MULT,
  ASM_MIPS_MULTU,
  ASM_MIPS_NOR,
  ASM_MIPS_OR,
  ASM_MIPS_ORI,
  ASM_MIPS_REG,
  ASM_MIPS_SB,
  ASM_MIPS_SH,
  ASM_MIPS_SLL,
  ASM_MIPS_SLLV,
  ASM_MIPS_SLT,
  ASM_MIPS_SLTI,
  ASM_MIPS_SLTIU,
  ASM_MIPS_SLTU,
  ASM_MIPS_SRA,
  ASM_MIPS_SRAV,
  ASM_MIPS_SRL,
  ASM_MIPS_SRLV,
  ASM_MIPS_SUB,
  ASM_MIPS_SUBU,
  ASM_MIPS_SW,
  ASM_MIPS_TRAP,
  ASM_MIPS_XOR,
  ASM_MIPS_XORI,  
  ASM_MIPS_BAD
} e_mips_instr;

enum
  {
    ASM_MIPS_REG_ZERO,
    ASM_MIPS_REG_RES1,
    ASM_MIPS_REG_V0,
    ASM_MIPS_REG_V1,
    ASM_MIPS_REG_A0,
    ASM_MIPS_REG_A1,
    ASM_MIPS_REG_A2,
    ASM_MIPS_REG_A3,
    ASM_MIPS_REG_T0,
    ASM_MIPS_REG_T1,
    ASM_MIPS_REG_T2,
    ASM_MIPS_REG_T3,
    ASM_MIPS_REG_T4,
    ASM_MIPS_REG_T5,
    ASM_MIPS_REG_T6,
    ASM_MIPS_REG_T7,
    ASM_MIPS_REG_S0,
    ASM_MIPS_REG_S1,
    ASM_MIPS_REG_S2,
    ASM_MIPS_REG_S3,
    ASM_MIPS_REG_S4,
    ASM_MIPS_REG_S5,
    ASM_MIPS_REG_S6,
    ASM_MIPS_REG_S7,
    ASM_MIPS_REG_T8,
    ASM_MIPS_REG_T9,
    ASM_MIPS_REG_RES26,
    ASM_MIPS_REG_RES27,
    ASM_MIPS_REG_GP,
    ASM_MIPS_REG_SP,
    ASM_MIPS_REG_FP,
    ASM_MIPS_REG_RA,

  } e_mips_registers;

enum
  {
    ASM_MIPS_OTYPE_NONE,
    ASM_MIPS_OTYPE_REGISTER,
    
  } e_mips_operand_type;

extern int mips_root_table[64];
extern int mips_func_table[64];

#endif
