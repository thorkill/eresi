/**
 * @file libasm-mips.h
 * $Id: libasm-mips.h,v 1.3 2007-11-15 02:59:35 thor Exp $
 * Manuel Martin - 2007
 */

#ifndef LIBASM_MIPS_H_
#define LIBASM_MIPS_H_

#include <libasm.h>

/**
 * @todo XXX: Rename fetch_mips to asm_fetch_mips.
 */

//LIBASM_HANDLER_DISPLAY(asm_mips_display_instr);
//LIBASM_HANDLER_FETCH(fetch_mips);

#define ASM_CONFIG_MIPS_REGISTER_FLAVOUR "libasm.mips.output.registers"
#define ASM_CONFIG_MIPS_STANDARD_REGISTERS 0
#define ASM_CONFIG_MIPS_EXTENDED_REGISTERS 1

char	*asm_mips_display_instr(asm_instr *, int);
int	fetch_mips(asm_instr *, u_char *, u_int, asm_processor *);
void	asm_register_mips_operands(asm_processor *proc);

#define Ob(x)  ((unsigned)Ob_(0 ## x ## uL))
#define Ob_(x) ((x & 1) | ((x >> 2) & 2) | ((x >> 4) & 4) | ((x >> 6) & 8) |    \
   ((x >> 8) & 16) | ((x >> 10) & 32) | ((x >> 12) & 64) | ((x >> 14) & 128))
/*
 * funciones
 * TODO
 */

/**
 * @page mips.html
 * 
 * <pre>
 *  
 * Instruction field meanings, from the
 *  mips32 architecture manual
 * 
 *  opcode         6-bit primary operation code
 *  rd             5-bit specier for the destination register
 *  rs             5-bit spec for the source register
 *  rt             5-bit specr for the target (source/destination) 
 *                 register or used to specify functions within the primary 
 *                 opcode REGIMM
 *  immediate      16-bit signed immediate used for logical operands,
 *                 arithmetic signed operands, load/store address 
 *                 byte offsets, and PC-relative branch signed 
 *                 instruction displacement
 *  instr_index    26-bit index shifted left two bits to supply the 
 *                 low-order 28 bits of the jump target address
 *  sa             5-bit shift amount
 *  function       6-bit function ld used to specify functions 
 *                 within the primary opcode SPECIAL
 * </pre>
 */

/* Immediate/(I-type) instruction 
 * used to work with immediate values (up to 16 bits) */
struct s_mips_insn_imm
{
   u_int32_t opcode:6;
   u_int32_t rs:5;
   u_int32_t rt:5; /* operation (sometimes) */
   u_int32_t immediate:16;
};

/* This is a subtype of the type inmmediate, it's used
 * in some branch-conditional instructions*/
struct s_mips_insn_imm_bc
{
   u_int32_t opcode:6;
   u_int32_t rs:5; /* operation */
   u_int32_t cc:3; /* condition code*/
   u_int32_t nd:1; /* nullify delay slot*/ 
   u_int32_t tf:1; /* true/false*/
   u_int32_t offset:16;
};

/* Jump/(J-type) instruction
 * used for jumps/branches */
struct s_mips_insn_jump
{
   u_int32_t opcode:6;
   u_int32_t instr_index:26;
};

/* Register/(R-type) instruction
 * used to transfer data between registers */
struct s_mips_insn_reg
{
   u_int32_t opcode:6;
   u_int32_t rs:5;
   u_int32_t rt:5;
   u_int32_t rd:5;
   u_int32_t sa:5;
   u_int32_t function:6;
};

/* This is a subtype of the register type, it's
 * used only for the BREAK and SYSCALL instructions */
struct s_mips_insn_reg_break_syscall
{
   u_int32_t opcode:6;
   u_int32_t code:20;
   u_int32_t function:6;
};

/* This is a subtype of the register type, it's
 * used only for the DERET insn*/
struct s_mips_insn_reg_deret
{
   u_int32_t opcode:6;
   u_int32_t co:1;
   u_int32_t zero:19;
   u_int32_t function:6;
};

#define MIPS_SPECIAL_FUNCTION_NUM 64
#define MIPS_REGIMM_FUNCTION_NUM 32
#define MIPS_SPECIAL2_FUNCTION_NUM 64
#define MIPS_SPECIAL3_FUNCTION_NUM 64

#define MIPS_LEVEL0_OPCODES_NUM 64
#define MIPS_LEVEL1_OPCODES_NUM 224
#define MIPS_LEVEL2_OPCODES_NUM 38

#define ASM_MIPS_OTYPE_NUM 8

#define ASM_MIPS_TABLE_END -1

enum {
   MIPS_OPCODE_SPECIAL, /* class */
   MIPS_OPCODE_REGIMM, /* class */
   MIPS_OPCODE_J,
   MIPS_OPCODE_JAL,
   MIPS_OPCODE_BEQ,
   MIPS_OPCODE_BNE,
   MIPS_OPCODE_BLEZ,
   MIPS_OPCODE_BGTZ,
   MIPS_OPCODE_ADDI,
   MIPS_OPCODE_ADDIU,
   MIPS_OPCODE_SLTI,
   MIPS_OPCODE_SLTIU,
   MIPS_OPCODE_ANDI,
   MIPS_OPCODE_ORI,
   MIPS_OPCODE_XORI,
   MIPS_OPCODE_LUI,
   MIPS_OPCODE_COP0, /* class */
   MIPS_OPCODE_COP1, /* class */
   MIPS_OPCODE_COP2, /* class */
   MIPS_OPCODE_COP1X, /* class */
   MIPS_OPCODE_BEQL, /* obsolete */
   MIPS_OPCODE_BNEL, /* obsolete */
   MIPS_OPCODE_BLEZL, /* obsolete*/
   MIPS_OPCODE_BGTZL, /* obsolete */
   MIPS_OPCODE_SPECIAL2 = (Ob(011100)), /* class */
   MIPS_OPCODE_JALX, /* reserved for ASE? */
   MIPS_OPCODE_SPECIAL3 = Ob(011111), /* class / release2 */
   MIPS_OPCODE_LB,
   MIPS_OPCODE_LH,
   MIPS_OPCODE_LWL,
   MIPS_OPCODE_LW,
   MIPS_OPCODE_LBU,
   MIPS_OPCODE_LHU,
   MIPS_OPCODE_LWR,
   MIPS_OPCODE_SB = Ob(101000),
   MIPS_OPCODE_SH,
   MIPS_OPCODE_SWL,
   MIPS_OPCODE_SW,
   MIPS_OPCODE_SWR = Ob(101110),
   MIPS_OPCODE_CACHE,
   MIPS_OPCODE_LL,
   MIPS_OPCODE_LWC1,
   MIPS_OPCODE_LWC2, /* 3rd party */
   MIPS_OPCODE_PREF,
   MIPS_OPCODE_LDC1 = Ob(110101),
   MIPS_OPCODE_LDC2, /* 3rd party */
   MIPS_OPCODE_SC = Ob(111000),
   MIPS_OPCODE_SWC1,
   MIPS_OPCODE_SWC2, /* 3rd party */
   MIPS_OPCODE_SDC1 = Ob(111101),
   MIPS_OPCODE_SDC2 /* 3rd party */
} opcode_field_encoding;

enum {
   MIPS_OPCODE_SLL,
   MIPS_OPCODE_MOVCI, /* class */
   MIPS_OPCODE_SRL, /* class */
   MIPS_OPCODE_SRA,
   MIPS_OPCODE_SLLV,
   MIPS_OPCODE_SRLV = Ob(000110), /* class */
   MIPS_OPCODE_SRAV,
   MIPS_OPCODE_JR,
   MIPS_OPCODE_JALR,
   MIPS_OPCODE_MOVZ,
   MIPS_OPCODE_MOVN,
   MIPS_OPCODE_SYSCALL,
   MIPS_OPCODE_BREAK,
   MIPS_OPCODE_SYNC = Ob(001111),
   MIPS_OPCODE_MFHI,
   MIPS_OPCODE_MTHI,
   MIPS_OPCODE_MFLO,
   MIPS_OPCODE_MTLO,
   MIPS_OPCODE_MULT = Ob(011000),
   MIPS_OPCODE_MULTU,
   MIPS_OPCODE_DIV,
   MIPS_OPCODE_DIVU,
   MIPS_OPCODE_ADD = Ob(100000),
   MIPS_OPCODE_ADDU,
   MIPS_OPCODE_SUB,
   MIPS_OPCODE_SUBU,
   MIPS_OPCODE_AND,
   MIPS_OPCODE_OR,
   MIPS_OPCODE_XOR,
   MIPS_OPCODE_NOR,
   MIPS_OPCODE_SLT = Ob(101010),
   MIPS_OPCODE_SLTU,
   MIPS_OPCODE_TGE = Ob(110000),
   MIPS_OPCODE_TGEU,
   MIPS_OPCODE_TLT,
   MIPS_OPCODE_TLTU,
   MIPS_OPCODE_TEQ,
   MIPS_OPCODE_TNE = Ob(110110)
} SPECIAL_function_field_encoding;

enum {
   MIPS_OPCODE_BLTZ,
   MIPS_OPCODE_BGEZ,
   MIPS_OPCODE_BLTZL, /* obsolete */
   MIPS_OPCODE_BGEZL, /* obsolete */
   MIPS_OPCODE_TGEI = Ob(01000),
   MIPS_OPCODE_TGEIU,
   MIPS_OPCODE_TLTI,
   MIPS_OPCODE_TLTIU,
   MIPS_OPCODE_TEQI,
   MIPS_OPCODE_TNEI = Ob(01110),
   MIPS_OPCODE_BLTZAL = Ob(10000),
   MIPS_OPCODE_BGEZAL,
   MIPS_OPCODE_BLTZALL, /* obsolete */
   MIPS_OPCODE_BGEZALL, /* obsolete */
   MIPS_OPCODE_SYNCI = Ob(11111) /* release 2 */
} REGIMM_rt_field_encoding;

enum {
   MIPS_OPCODE_MADD,
   MIPS_OPCODE_MADDU,
   MIPS_OPCODE_MUL,
   MIPS_OPCODE_MSUB = Ob(000100),
   MIPS_OPCODE_MSUBU,
   MIPS_OPCODE_CLZ = Ob(100000),
   MIPS_OPCODE_CLO,
   MIPS_OPCODE_SDBBP = Ob(111111) /* EJTAG */
} SPECIAL2_function_field_encoding;

/* only for release 2*/
enum {
   MIPS_OPCODE_EXT,
   MIPS_OPCODE_INS = Ob(000100),
   MIPS_OPCODE_BSHFL = Ob(100000), /* class */
   MIPS_OPCODE_RDHWR = Ob(111011),
} SPECIAL3_function_field_encoding;

enum {
   MIPS_OPCODE_MOVF,
   MIPS_OPCODE_MOVT
} MOVCI_tf_field_encoding;

enum {
  // XXX: _42 was added to allow compiling because MIPS_OPCODE_SRL is already in an other enum.
  MIPS_OPCODE_SRL_42,
  MIPS_OPCODE_ROTR
} SRL_shiftrotate_field_encoding;

enum {
  // XXX: _42 was added to allow compiling because MIPS_OPCODE_SRL is already in an other enum.
   MIPS_OPCODE_SRLV_42,
   MIPS_OPCODE_ROTRV
} SRLV_shiftrotate_field_ecoding;

enum {
   MIPS_OPCODE_WSBH = Ob(00010),
   MIPS_OPCODE_SEB = Ob(10000),
   MIPS_OPCODE_SEH = Ob(11000)
} BSHFL_sa_field_encoding;

/* XXX: privileged and fpu stuff.. not implemented yet*/
/* 
enum {} COP0_rs_field_encoding;
enum {} COP0_function_field_encoding_rsCO;
enum {} COP1_rs_field_encoding;
enum {} COP1_function_field_encoding_rsS;
enum {} COP1_function_field_encoding_rsD;
enum{} COP1_function_field_encoding_rsW_or_rsL;
enum {} COP1_function_field_encoding_rsPS;
*/

/*
 * mips instructions
 * r2 after an instruction means that instruction
 * is only available in the MIPS32 release 2 specification
 * */
enum
{
   /* arithmetics */
   ASM_MIPS_ADD,
   ASM_MIPS_ADDI,
   ASM_MIPS_ADDIU,
   ASM_MIPS_ADDU,
   ASM_MIPS_CLO,
   ASM_MIPS_CLZ,
   ASM_MIPS_DIV,
   ASM_MIPS_DIVU,
   ASM_MIPS_MADD,
   ASM_MIPS_MADDU,
   ASM_MIPS_MSUB,
   ASM_MIPS_MSUBU,
   ASM_MIPS_MUL,
   ASM_MIPS_MULT,
   ASM_MIPS_MULTU,
   ASM_MIPS_SEB, /*r2*/
   ASM_MIPS_SEH, /*r2*/
   ASM_MIPS_SLT,
   ASM_MIPS_SLTI,
   ASM_MIPS_SLTIU,
   ASM_MIPS_SLTU,
   ASM_MIPS_SUB,
   ASM_MIPS_SUBU,
   /* branch */
   ASM_MIPS_B,
   ASM_MIPS_BAL,
   ASM_MIPS_BEQ,
   ASM_MIPS_BGEZ,
   ASM_MIPS_BGEZAL,
   ASM_MIPS_BGTZ,
   ASM_MIPS_BLEZ,
   ASM_MIPS_BLTZ,
   ASM_MIPS_BLTZAL,
   ASM_MIPS_BNE,
   ASM_MIPS_J,
   ASM_MIPS_JAL,
   ASM_MIPS_JALR,
   ASM_MIPS_JALR_HB,
   ASM_MIPS_JR,
   ASM_MIPS_JR_HB,
   /* obsolete branch */
   ASM_MIPS_BEQL,
   ASM_MIPS_BGEZALL,
   ASM_MIPS_BGEZL,
   ASM_MIPS_BGTZL,
   ASM_MIPS_BLEZL,
   ASM_MIPS_BLTZALL,
   ASM_MIPS_BLTZL,
   ASM_MIPS_BNEL,
   /* cpu execution control*/
   ASM_MIPS_EHB, /*r2*/
   ASM_MIPS_NOP,
   ASM_MIPS_SSNOP,
   /* memory */
   ASM_MIPS_LB,
   ASM_MIPS_LBU,
   ASM_MIPS_LH,
   ASM_MIPS_LHU,
   ASM_MIPS_LL,
   ASM_MIPS_LW,
   ASM_MIPS_LWL,
   ASM_MIPS_LWR,
   ASM_MIPS_PREF,
   ASM_MIPS_SB,
   ASM_MIPS_SC,
   ASM_MIPS_SH,
   ASM_MIPS_SW,
   ASM_MIPS_SWL,
   ASM_MIPS_SWR,
   ASM_MIPS_SYNC,
   ASM_MIPS_SYNCI, /*r2*/
   /* logic */
   ASM_MIPS_AND,
   ASM_MIPS_ANDI,
   ASM_MIPS_LUI,
   ASM_MIPS_NOR,
   ASM_MIPS_OR,
   ASM_MIPS_ORI,
   ASM_MIPS_XOR,
   ASM_MIPS_XORI,
   /* insert/extract */
   ASM_MIPS_EXT, /*r2*/
   ASM_MIPS_INS, /*r2*/
   ASM_MIPS_WSBH, /*r2*/
   /* cpu move */
   ASM_MIPS_MFHI,
   ASM_MIPS_MFLO,
   ASM_MIPS_MOVF,
   ASM_MIPS_MOVN,
   ASM_MIPS_MOVT,
   ASM_MIPS_MOVZ,
   ASM_MIPS_MTHI,
   ASM_MIPS_MTLO,
   ASM_MIPS_RDHWR, /*r2*/
   /* shift */
   ASM_MIPS_ROTR, /*r2*/
   ASM_MIPS_ROTRV, /*r2*/
   ASM_MIPS_SLL,
   ASM_MIPS_SLLV,
   ASM_MIPS_SRA,
   ASM_MIPS_SRAV,
   ASM_MIPS_SRL,
   ASM_MIPS_SRLV,
   /* traps */
   ASM_MIPS_BREAK,
   ASM_MIPS_SYSCALL,
   ASM_MIPS_TEQ,
   ASM_MIPS_TEQI,
   ASM_MIPS_TGE,
   ASM_MIPS_TGEI,
   ASM_MIPS_TGEIU,
   ASM_MIPS_TGEU,
   ASM_MIPS_TLT,
   ASM_MIPS_TLTI,
   ASM_MIPS_TLTIU,
   ASM_MIPS_TLTU,
   ASM_MIPS_TNE,
   ASM_MIPS_TNEI,

   /*TODO:
    * - FPU insns
    * - COP2 insns
    * - privileged insns
    * */

   ASM_MIPS_BAD
} e_mips_instr_types;

typedef int	e_mips_register_type;

enum
{
   ASM_MIPS_REG_ZERO, /* Zero register (always 0)*/
   ASM_MIPS_REG_AT, /* Assembler temporary (reserved) */
   ASM_MIPS_REG_V0, /* V0-V1 Value returned by subroutine*/
   ASM_MIPS_REG_V1,
   ASM_MIPS_REG_A0, /* A0-A3 Arguments to subroutine*/
   ASM_MIPS_REG_A1,
   ASM_MIPS_REG_A2,
   ASM_MIPS_REG_A3,
   ASM_MIPS_REG_T0, /* T0-T7 Temporary (local variables) */
   ASM_MIPS_REG_T1,
   ASM_MIPS_REG_T2,
   ASM_MIPS_REG_T3,
   ASM_MIPS_REG_T4,
   ASM_MIPS_REG_T5,
   ASM_MIPS_REG_T6,
   ASM_MIPS_REG_T7,
   ASM_MIPS_REG_S0, /* S0-S7 Saved registers */
   ASM_MIPS_REG_S1, /* (preserved across function calls) */
   ASM_MIPS_REG_S2,
   ASM_MIPS_REG_S3,
   ASM_MIPS_REG_S4,
   ASM_MIPS_REG_S5,
   ASM_MIPS_REG_S6,
   ASM_MIPS_REG_S7,
   ASM_MIPS_REG_T8, /* T8-T9 Temporary (local variables) */
   ASM_MIPS_REG_T9,
   ASM_MIPS_REG_K0, /* K0-K1 Kernel registers (reserved for OS) */
   ASM_MIPS_REG_K1,
   ASM_MIPS_REG_GP, /* Global pointer */
   ASM_MIPS_REG_SP, /* Stack pointer */
   ASM_MIPS_REG_FP, /* Frame pointer */
   ASM_MIPS_REG_RA  /* Return Address */
} e_mips_register_types;

/*  operand types */
enum
{
   ASM_MIPS_OTYPE_NONE,
   ASM_MIPS_OTYPE_REGISTER,
   ASM_MIPS_OTYPE_IMMEDIATE
} e_mips_operand_type;

/**
 * Structure to declare a mips instruction in e_mips_instr table.
 */
struct e_mips_instr 
{
  const char *mnemonic;
  int code;
   int index1;
   int index2;
   int index3;
};

/**
 * Structure to store register in the structure e_mips_register;
 */
struct e_mips_register 
{
  const char *ext_mnemonic;
  const char *mnemonic;
  e_mips_register_type code;
};

/**
 * This enum contains MIPS_OPCODE_ values which
 * are not defined upper.
 * 
 *
 */
enum 
  {
    MIPS_OPCODE_EHB,
    MIPS_OPCODE_NOP,
    MIPS_OPCODE_SSNOP
  } e_fix_compile_errors;


/**
 * Those structures arrays are declared in src/arch/mips/tables_mips.c
 */
extern struct e_mips_instr e_mips_instrs[];
extern struct e_mips_register e_mips_registers[];

#endif
