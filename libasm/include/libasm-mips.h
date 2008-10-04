/**
 * @file libasm-mips.h
 * $Id: libasm-mips.h,v 1.5 2008/01/07 01:29:53 heroine Exp $
 *
 * fix and fill
 *          - Adam 'pi3' Zabrocki
 *
 * Manuel Martin - 2007
 */

#ifndef LIBASM_MIPS_H_
#define LIBASM_MIPS_H_

#include <libasm.h>
#include <libasm-mips-structs.h>

//LIBASM_HANDLER_DISPLAY(asm_mips_display_instr);
//LIBASM_HANDLER_FETCH(fetch_mips);

#define ASM_CONFIG_MIPS_REGISTER_FLAVOUR "libasm.mips.output.registers"
#define ASM_CONFIG_MIPS_STANDARD_REGISTERS 0
#define ASM_CONFIG_MIPS_EXTENDED_REGISTERS 1

#define ASM_MIPS_REG_FPU ASM_CONFIG_MIPS_EXTENDED_REGISTERS

char	*asm_mips_display_instr(asm_instr *, int);
void    mips_convert_format_r(struct s_mips_decode_reg  *opcode, unsigned char *buf);
void    mips_convert_format_i(struct s_mips_decode_imm  *opcode, unsigned char *buf);
void    mips_convert_format_j(struct s_mips_decode_jump *opcode, unsigned char *buf);
void    mips_convert_format_t(struct s_mips_decode_trap *opcode, unsigned char *buf);
void    mips_convert_format_cop2(struct s_mips_decode_cop2 *opcode, unsigned char *buf);
void    mips_convert_format_mov(struct s_mips_decode_mov *opcode, unsigned char *buf);
void    mips_convert_format_cop1x(struct s_mips_decode_cop1x *opcode, unsigned char *buf);

#define mips_convert_format_priv mips_convert_format_cop2

int	asm_fetch_mips(asm_instr *, unsigned char *, unsigned int, asm_processor *);

int	asm_register_mips();
int	asm_register_mips_opcodes();
int	asm_register_mips_operands();
int     asm_register_mips_operand(unsigned int type, unsigned long func);

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
//struct s_mips_insn_imm
//{
//   uint32_t opcode:6;
//   uint32_t rs:5;
//   uint32_t rt:5; /* operation (sometimes) */
//   uint32_t immediate:16;
//};

/* This is a subtype of the type inmmediate, it's used
 * in some branch-conditional instructions*/
//struct s_mips_insn_imm_bc
//{
//   uint32_t opcode:6;
//   uint32_t rs:5; /* operation */
//   uint32_t cc:3; /* condition code*/
//   uint32_t nd:1; /* nullify delay slot*/ 
//   uint32_t tf:1; /* true/false*/
//   uint32_t offset:16;
//};

/* Jump/(J-type) instruction
 * used for jumps/branches */
//struct s_mips_insn_jump
//{
//   uint32_t opcode:6;
//   uint32_t instr_index:26;
//};

/* Register/(R-type) instruction
 * used to transfer data between registers */
//struct s_mips_insn_reg
//{
//   uint32_t opcode:6;
//   uint32_t rs:5;
//   uint32_t rt:5;
//   uint32_t rd:5;
//   uint32_t sa:5;
//   uint32_t function:6;
//};

/* This is a subtype of the register type, it's
 * used only for the BREAK and SYSCALL instructions */
//struct s_mips_insn_reg_break_syscall
//{
//   uint32_t opcode:6;
//   uint32_t code:20;
//   uint32_t function:6;
//};

/* This is a subtype of the register type, it's
 * used only for the DERET insn*/
//struct s_mips_insn_reg_deret
//{
//   uint32_t opcode:6;
//   uint32_t co:1;
//   uint32_t zero:19;
//   uint32_t function:6;
//};

#define MIPS_SPECIAL_FUNCTION_NUM 64
#define MIPS_REGIMM_FUNCTION_NUM 32
#define MIPS_SPECIAL2_FUNCTION_NUM 64
#define MIPS_SPECIAL3_FUNCTION_NUM 64

#define MIPS_LEVEL0_OPCODES_NUM 64
#define MIPS_LEVEL1_OPCODES_NUM 224
#define MIPS_LEVEL2_OPCODES_NUM 38



#define ASM_MIPS_TABLE_END -1

enum opcode_field_encoding {

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
   MIPS_OPCODE_SDC2, /* 3rd party */
   
   MIPS_OPCODE_DADDI = Ob(011000),
   MIPS_OPCODE_DADDIU,
   MIPS_OPCODE_LDL,//          011010
   MIPS_OPCODE_LDR,
   MIPS_OPCODE_LLD = Ob(110100),
   MIPS_OPCODE_LD = Ob(110111),
   MIPS_OPCODE_LWU = Ob(100111),
   MIPS_OPCODE_SCD = Ob(111100),
   MIPS_OPCODE_SD = Ob(111111),
   MIPS_OPCODE_SDL = Ob(101100),
   MIPS_OPCODE_SDR

};

enum SPECIAL_function_field_encoding {

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
   MIPS_OPCODE_TNE = Ob(110110),
   
   MIPS_OPCODE_DADD = Ob(101100),
   MIPS_OPCODE_DADDU,
   MIPS_OPCODE_DSUB,
   MIPS_OPCODE_DSUBU,
   MIPS_OPCODE_DMULT = Ob(011100),
   MIPS_OPCODE_DMULTU,
   MIPS_OPCODE_DDIV,
   MIPS_OPCODE_DDIVU,
   MIPS_OPCODE_DSLL = Ob(111000),
   MIPS_OPCODE_DSLL32 = Ob(111100),
   MIPS_OPCODE_DSLLV = Ob(010100),
   MIPS_OPCODE_DSRL = Ob(111010),
   MIPS_OPCODE_DSRA,
   MIPS_OPCODE_DSRL32 = Ob(111110),
   MIPS_OPCODE_DSRA32,
   MIPS_OPCODE_DSRLV = Ob(010110),
   MIPS_OPCODE_DSRAV

};

enum REGIMM_rt_field_encoding {

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

};

enum SPECIAL2_function_field_encoding {

   MIPS_OPCODE_MADD,
   MIPS_OPCODE_MADDU,
   MIPS_OPCODE_MUL,
   MIPS_OPCODE_MSUB = Ob(000100),
   MIPS_OPCODE_MSUBU,
   MIPS_OPCODE_CLZ = Ob(100000),
   MIPS_OPCODE_CLO,
   MIPS_OPCODE_SDBBP = Ob(111111), /* EJTAG */
   
   MIPS_OPCODE_DCLZ = Ob(100100),
   MIPS_OPCODE_DCLO

};

/* only for release 2*/
enum SPECIAL3_function_field_encoding {

   MIPS_OPCODE_EXT,
   MIPS_OPCODE_INS = Ob(000100),
   MIPS_OPCODE_BSHFL = Ob(100000), /* class */
   MIPS_OPCODE_RDHWR = Ob(111011),

};

enum MOVCI_tf_field_encoding {

   MIPS_OPCODE_MOVF,
   MIPS_OPCODE_MOVT

};

enum SRL_shiftrotate_field_encoding {

  // XXX: _42 was added to allow compiling because MIPS_OPCODE_SRL is already in an other enum.
  MIPS_OPCODE_SRL_42,
  MIPS_OPCODE_ROTR

};

enum SRLV_shiftrotate_field_ecoding {

  // XXX: _42 was added to allow compiling because MIPS_OPCODE_SRL is already in an other enum.
   MIPS_OPCODE_SRLV_42,
   MIPS_OPCODE_ROTRV

};

enum BSHFL_sa_field_encoding {

   MIPS_OPCODE_WSBH = Ob(00010),
   MIPS_OPCODE_SEB = Ob(10000),
   MIPS_OPCODE_SEH = Ob(11000)

};

enum COP2_func {

   MIPS_OPCODE_MFC2,
   MIPS_OPCODE_DMFC2,
   MIPS_OPCODE_CFC2,
   MIPS_OPCODE_MTC2 = Ob(00100),
   MIPS_OPCODE_DMTC2,
   MIPS_OPCODE_CTC2,
   MIPS_OPCODE_BCC2 = Ob(01000)

};

enum COP1X_func {

   MIPS_OPCODE_LWXC1 = Ob(000000),
   MIPS_OPCODE_LDXC1,
   MIPS_OPCODE_LUXC1 = Ob(000101),
   MIPS_OPCODE_SWXC1 = Ob(001000),
   MIPS_OPCODE_SDXC1,
   MIPS_OPCODE_SUXC1 = Ob(001101),
   MIPS_OPCODE_PREFX = Ob(001111),
   MIPS_OPCODE_ALNV_PS = Ob(011110),

   MIPS_OPCODE_MADD_S = Ob(100000),
   MIPS_OPCODE_MADD_D,
   MIPS_OPCODE_MADD_PS = Ob(100110),

   MIPS_OPCODE_MSUB_S = Ob(101000),
   MIPS_OPCODE_MSUB_D,
   MIPS_OPCODE_MSUB_PS = Ob(101110),
   
   MIPS_OPCODE_NMADD_S = Ob(110000),
   MIPS_OPCODE_NMADD_D,
   MIPS_OPCODE_NMADD_PS = Ob(110110),

   MIPS_OPCODE_NMSUB_S = Ob(111000),
   MIPS_OPCODE_NMSUB_D,
   MIPS_OPCODE_NMSUB_PS = Ob(111110)

};

enum COP0_func {

   MIPS_OPCODE_MFC0,
   MIPS_OPCODE_DMFC0,
   MIPS_OPCODE_TLBWI,
   MIPS_OPCODE_MTC0 = Ob(00100),
   MIPS_OPCODE_DMTC0,
   MIPS_OPCODE_TLBWR,
   MIPS_OPCODE_ERET = Ob(011000),
   MIPS_OPCODE_TLBP = Ob(001000),
   MIPS_OPCODE_TLBR = Ob(000001),
   MIPS_OPCODE_DERET = Ob(011111),
   MIPS_OPCODE_WAIT
};

enum COP1_func {

   MIPS_OPCODE_F_ADD,
   MIPS_OPCODE_F_SUB,
   MIPS_OPCODE_F_MUL,// = Ob(000010),
   MIPS_OPCODE_F_DIV,
   MIPS_OPCODE_F_SQRT,
   MIPS_OPCODE_ABS,// = Ob(000101),
   MIPS_OPCODE_F_MOV, //=Ob(000110)
   MIPS_OPCODE_F_NEG,// = Ob(000111),
   MIPS_OPCODE_F_ROUND_L,
   MIPS_OPCODE_F_TRUNC_L,
   MIPS_OPCODE_F_ROUND_W = Ob(001100),
   MIPS_OPCODE_F_TRUNC_W,
   MIPS_OPCODE_F_CEIL_L = Ob(001010),
   MIPS_OPCODE_F_FLOOR_L,
   MIPS_OPCODE_F_CEIL_W = Ob(001110),
   MIPS_OPCODE_F_FLOOR_W,
   MIPS_OPCODE_F_RECIP = Ob(010101),
   MIPS_OPCODE_F_RSQRT,
   MIPS_OPCODE_F_MOVCF = Ob(010001),
   MIPS_OPCODE_F_MOVZ,
   MIPS_OPCODE_F_MOVN,
   MIPS_OPCODE_F_CVT_S = Ob(100000),
   MIPS_OPCODE_F_CVT_D,// = Ob(100001),
   MIPS_OPCODE_F_CVT_W = Ob(100100),
   MIPS_OPCODE_F_CVT_L,// = Ob(100101),
   MIPS_OPCODE_F_CVT_PS_S,
   MIPS_OPCODE_F_CVT_S_PL = Ob(101000),
   MIPS_OPCODE_F_MFC1 = Ob(00000),
   MIPS_OPCODE_F_CFC1 = Ob(00010),
   MIPS_OPCODE_F_MTC1 = Ob(00100),
   MIPS_OPCODE_F_CTC1 = Ob(00110)

};

enum COP1_fmt {

   MIPS_OPCODE_FMT_S = Ob(10000),
   MIPS_OPCODE_FMT_D,
   MIPS_OPCODE_FMT_W = Ob(10100),
   MIPS_OPCODE_FMT_L,// = Ob(10101),
   MIPS_OPCODE_FMT_PS// = Ob(10110)

};
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
enum e_mips_instr_types
{
   /* arithmetics */
   ASM_MIPS_ADD,
   ASM_MIPS_ADDI,
   ASM_MIPS_ADDIU,
   ASM_MIPS_ADDU,
   ASM_MIPS_CLO,
   ASM_MIPS_CLZ,
   /* mips64v2 */
   ASM_MIPS_DADD,
   ASM_MIPS_DADDI,
   ASM_MIPS_DADDIU,
   ASM_MIPS_DADDU,
   ASM_MIPS_DCLO,
   ASM_MIPS_DCLZ,
   ASM_MIPS_DDIV,
   ASM_MIPS_DDIVU,
   /* mips64v2 */
   ASM_MIPS_DIV,
   ASM_MIPS_DIVU,
   /* mips64v2 */
   ASM_MIPS_DMULT,
   ASM_MIPS_DMULTU,
   ASM_MIPS_DSUB,
   ASM_MIPS_DSUBU,
   /* mips64v2 */
   ASM_MIPS_MADD,
   ASM_MIPS_MADDU,
   ASM_MIPS_MSUB,
   ASM_MIPS_MSUBU,
   ASM_MIPS_MUL,
   ASM_MIPS_MULT,
   ASM_MIPS_MULTU,
   ASM_MIPS_SLT,
   ASM_MIPS_SLTI,
   ASM_MIPS_SLTIU,
   ASM_MIPS_SLTU,
   ASM_MIPS_SUB,
   ASM_MIPS_SUBU,
   ASM_MIPS_SEB, /*r2*/
   ASM_MIPS_SEH, /*r2*/
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
   ASM_MIPS_JR,
   ASM_MIPS_JALR_HB,
   ASM_MIPS_JR_HB,
   /* cpu execution control*/
//   ASM_MIPS_EHB, /*r2*/
   ASM_MIPS_NOP,
   ASM_MIPS_SSNOP,
   /* memory */
   ASM_MIPS_LB,
   ASM_MIPS_LBU,
   /* mips64v2 */
   ASM_MIPS_LD,
   ASM_MIPS_LDL,
   ASM_MIPS_LDR,
   /* mips64v2 */
   ASM_MIPS_LH,
   ASM_MIPS_LHU,
   ASM_MIPS_LL,
   /* mips64v2 */
   ASM_MIPS_LLD,
   /* mips64v2 */
   ASM_MIPS_LW,
   ASM_MIPS_LWL,
   ASM_MIPS_LWR,
   /* mips64v2 */
   ASM_MIPS_LWU,
   /* mips64v2 */
   ASM_MIPS_PREF,
   ASM_MIPS_SB,
   ASM_MIPS_SC,
   /* mips64v2 */
   ASM_MIPS_SCD,
   ASM_MIPS_SD,
   ASM_MIPS_SDL,
   ASM_MIPS_SDR,
   /* mips64v2 */
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
   /* shift */
   /* mips64v2 */
   ASM_MIPS_DSLL,
   ASM_MIPS_DSLL32,
   ASM_MIPS_DSLLV,
   ASM_MIPS_DSRA,
   ASM_MIPS_DSRA32,
   ASM_MIPS_DSRAV,
   ASM_MIPS_DSRL,
   ASM_MIPS_DSRL32,
   ASM_MIPS_DSRLV,
   /* mips64v2 */
   ASM_MIPS_SLL,
   ASM_MIPS_SLLV,
   ASM_MIPS_SRA,
   ASM_MIPS_SRAV,
   ASM_MIPS_SRL,
   ASM_MIPS_SRLV,
   ASM_MIPS_RDHWR, /*r2*/
   ASM_MIPS_ROTR, /*r2*/
   ASM_MIPS_ROTRV, /*r2*/
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
   /* obsolete branch */
   ASM_MIPS_BEQL,
   ASM_MIPS_BGEZALL,
   ASM_MIPS_BGEZL,
   ASM_MIPS_BGTZL,
   ASM_MIPS_BLEZL,
   ASM_MIPS_BLTZALL,
   ASM_MIPS_BLTZL,
   ASM_MIPS_BNEL,

   /* COP2 Instructions */
   ASM_MIPS_BC2F,
   ASM_MIPS_BC2T,
   ASM_MIPS_COP2,
   ASM_MIPS_LDC2,
   ASM_MIPS_LWC2,
   ASM_MIPS_SDC2,
   ASM_MIPS_SWC2,
   ASM_MIPS_CFC2,
   ASM_MIPS_CTC2,
   ASM_MIPS_DMFC2,
   ASM_MIPS_DMTC2,
   ASM_MIPS_MFC2,
   ASM_MIPS_MTC2,
   ASM_MIPS_BC2FL,
   ASM_MIPS_BC2TL,

   /* COP1X Instructions */
   ASM_MIPS_LWXC1,
   ASM_MIPS_LDXC1,
   ASM_MIPS_LUXC1,
   ASM_MIPS_SWXC1,
   ASM_MIPS_SDXC1,
   ASM_MIPS_SUXC1,
   ASM_MIPS_PREFX,
   ASM_MIPS_ALNV_PS,
   ASM_MIPS_MADD_S,
   ASM_MIPS_MADD_D,
   ASM_MIPS_MADD_PS,
   ASM_MIPS_MSUB_S,
   ASM_MIPS_MSUB_D,
   ASM_MIPS_MSUB_PS,
   ASM_MIPS_NMADD_S,
   ASM_MIPS_NMADD_D,
   ASM_MIPS_NMADD_PS,
   ASM_MIPS_NMSUB_S,
   ASM_MIPS_NMSUB_D,
   ASM_MIPS_NMSUB_PS,
   
   /* Chuj doklikalem to bo sie wkurwilem, jebane gowno - zycie to bagno... */
   /* Privileged instructions... */
//   ASM_MIPS_CACHE,
   ASM_MIPS_MFC0,
   ASM_MIPS_DMFC0,
   ASM_MIPS_TLBWI,
   ASM_MIPS_MTC0,
   ASM_MIPS_DMTC0,
   ASM_MIPS_TLBWR,
   ASM_MIPS_ERET,
   ASM_MIPS_TLBP,
   ASM_MIPS_TLBR,
   ASM_MIPS_WAIT,

   /* EJTAG Instructions... */
   ASM_MIPS_DERET,
   ASM_MIPS_SDBBP,

   /*TODO:
    * - FPU insns
    * */

   /* FPU arithmetics */
   ASM_MIPS_ABS_S,
   ASM_MIPS_ABS_D,
   ASM_MIPS_ABS_PS,
   ASM_MIPS_ADD_S,
   ASM_MIPS_ADD_D,
   ASM_MIPS_ADD_PS,

   ASM_MIPS_DIV_S,
   ASM_MIPS_DIV_D,
   ASM_MIPS_MUL_S,
   ASM_MIPS_MUL_D,
   ASM_MIPS_MUL_PS,
   ASM_MIPS_NEG_S,
   ASM_MIPS_NEG_D,
   ASM_MIPS_NEG_PS,
   ASM_MIPS_RECIP_S,
   ASM_MIPS_RECIP_D,
   ASM_MIPS_RSQRT_S,
   ASM_MIPS_RSQRT_D,
   ASM_MIPS_SQRT_S,
   ASM_MIPS_SQRT_D,
   ASM_MIPS_SUB_S,
   ASM_MIPS_SUB_D,
   ASM_MIPS_SUB_PS,

   /* FPU Branch Instructions */

   ASM_MIPS_BC1F,
   ASM_MIPS_BC1T,

   /* FPU Compare Instruction */
//   ASM_MIPS_C.cond.fmt,

   /* FPU Convert Instructions */

   ASM_MIPS_CEIL_L_S,
   ASM_MIPS_CEIL_L_D,
   ASM_MIPS_CEIL_W_S,
   ASM_MIPS_CEIL_W_D,

   ASM_MIPS_CVT_D_S,
   ASM_MIPS_CVT_D_W,
   ASM_MIPS_CVT_D_L,
   ASM_MIPS_CVT_L_S,
   ASM_MIPS_CVT_L_D,
   ASM_MIPS_CVT_PS_S,
   ASM_MIPS_CVT_S_D,
   ASM_MIPS_CVT_S_W,
   ASM_MIPS_CVT_S_L,
   ASM_MIPS_CVT_S_PL,
   ASM_MIPS_CVT_S_PU,
   ASM_MIPS_CVT_W_S,
   ASM_MIPS_CVT_W_D,

   ASM_MIPS_FLOOR_L_S,
   ASM_MIPS_FLOOR_L_D,
   ASM_MIPS_FLOOR_W_S,
   ASM_MIPS_FLOOR_W_D,

   ASM_MIPS_ROUND_L_S,
   ASM_MIPS_ROUND_L_D,
   ASM_MIPS_ROUND_W_S,
   ASM_MIPS_ROUND_W_D,

   ASM_MIPS_TRUNC_L_S,
   ASM_MIPS_TRUNC_L_D,
   ASM_MIPS_TRUNC_W_S,
   ASM_MIPS_TRUNC_W_D,

   /* FPU Load, Store and Memory Control Instructions */

   ASM_MIPS_LDC1,
   ASM_MIPS_LWC1,
   ASM_MIPS_SDC1,
   ASM_MIPS_SWC1,

   /* FPU Move Instructions */

   ASM_MIPS_CFC1,
   ASM_MIPS_CTC1,
   ASM_MIPS_MFC1,
   ASM_MIPS_MTC1,

   ASM_MIPS_MOV_S,
   ASM_MIPS_MOV_D,
   ASM_MIPS_MOV_PS,

   ASM_MIPS_MOVF_S,
   ASM_MIPS_MOVF_D,
   ASM_MIPS_MOVF_PS,

   ASM_MIPS_MOVT_S,
   ASM_MIPS_MOVT_D,
   ASM_MIPS_MOVT_PS,

   ASM_MIPS_MOVN_S,
   ASM_MIPS_MOVN_D,
   ASM_MIPS_MOVN_PS,

   ASM_MIPS_MOVZ_S,
   ASM_MIPS_MOVZ_D,
   ASM_MIPS_MOVZ_PS,

   /* FPU Absolute Branch Instructions */

   ASM_MIPS_BC1FL,
   ASM_MIPS_BC1TL,

   /* BUGFIX */
   ASM_MIPS_DMTC1,
   ASM_MIPS_DMFC1,

   ASM_MIPS_BAD
};

typedef int	e_mips_register_type;

enum e_mips_register_types
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
};

/**
 *  operand types 
 */
enum e_mips_operand_type
{
   ASM_MIPS_OTYPE_NONE,
   ASM_MIPS_OTYPE_REGISTER,
   ASM_MIPS_OTYPE_IMMEDIATE,
   ASM_MIPS_OTYPE_JUMP,
   ASM_MIPS_OTYPE_NOOP,
   ASM_MIPS_OTYPE_BRANCH,
   ASM_MIPS_OTYPE_REGBASE,

   ASM_MIPS_OTYPE_LAST
};

#define ASM_MIPS_OTYPE_NUM ASM_MIPS_OTYPE_LAST

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
  int (*func_op)(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
};

/**
 * Structure to store register in the structure e_mips_register;
 */
struct e_mips_register 
{
  const char *ext_mnemonic;
  const char *mnemonic;
  const char *fpu_mnemonic;
  e_mips_register_type code;
};

/**
 * This enum contains MIPS_OPCODE_ values which
 * are not defined upper.
 * 
 *
 */
enum e_fix_compile_errors
  {
//    MIPS_OPCODE_EHB,
    MIPS_OPCODE_NOP = Ob(00000),
    MIPS_OPCODE_SSNOP
  };

/**
 * Those structures arrays are declared in src/arch/mips/tables_mips.c
 */
extern struct e_mips_instr e_mips_instrs[];
extern struct e_mips_register e_mips_registers[];

#endif


/* Operands fetch */

int asm_mips_operand_fetch(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);


/* Operands */

void    asm_mips_operand_none(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_i(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_j(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_r(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_t(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_noop(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_branch(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);
void    asm_mips_operand_regbase(asm_operand *op, unsigned char *opcode, int otype, asm_instr *ins);


/* Opcodes */

int asm_mips_add(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_addi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_addiu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_addu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_and(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_andi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_b(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bal(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_beq(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_beql(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bgez(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bgezal(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bgezall(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bgezl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bgtz(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bgtzl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_blez(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_blezl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bltz(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bltzal(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bltzall(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bltzl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bne(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bnel(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_break(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_clo(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_clz(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_div(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_divu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
//int asm_mips_ehb(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ext(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ins(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_j(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_jal(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_jalr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_jalr_hb(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_jr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_jr_hb(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lb(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lbu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lh(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lhu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ll(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lui(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lw(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lwl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lwr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_madd(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_maddu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mfhi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mflo(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movf(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movn(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movt(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movz(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_msub(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_msubu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mthi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mtlo(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mul(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mult(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_multu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nop(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nor(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_or(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ori(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_pref(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_rdhwr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_rotr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_rotrv(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sb(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sc(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_seb(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_seh(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sh(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sll(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sllv(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_slt(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_slti(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sltiu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sltu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sra(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_srav(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_srl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_srlv(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ssnop(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sub(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_subu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sw(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_swl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_swr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sync(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_synci(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_syscall(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_teq(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_teqi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tge(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tgei(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tgeiu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tgeu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tlt(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tlti(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tltiu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tltu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tne(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tnei(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_wsbh(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_xor(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_xori(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// Hehe... ;-)
int asm_mips_dadd(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_daddi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_daddiu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_daddu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dclo(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dclz(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ddiv(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ddivu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

int asm_mips_dmult(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dmultu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsub(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsubu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

int asm_mips_ld(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ldl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ldr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

int asm_mips_lld(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

int asm_mips_lwu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

int asm_mips_scd(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sd(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sdl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sdr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

int asm_mips_dsll(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsll32(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsllv(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsra(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsra32(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsrav(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsrl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsrl32(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dsrlv(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);


// Coprocesor 2 functions...

int asm_mips_bc2f(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bc2t(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cop2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ldc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lwc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sdc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_swc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cfc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ctc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dmfc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dmtc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mfc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mtc2(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bc2fl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bc2tl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);


// COP1X functions...

int asm_mips_lwxc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ldxc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_luxc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_swxc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sdxc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_suxc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_prefx(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_alnv_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_madd_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_madd_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_madd_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_msub_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_msub_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_msub_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nmadd_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nmadd_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nmadd_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nmsub_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nmsub_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_nmsub_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// Privilaged Instructions - COP0

int asm_mips_mfc0(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dmfc0(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tlbwi(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mtc0(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dmtc0(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tlbwr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_eret(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tlbp(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_tlbr(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_wait(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_deret(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sdbbp(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// FPU - arithmetic...
int asm_mips_abs_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_abs_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_abs_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_add_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_add_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_add_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_div_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_div_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mul_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mul_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mul_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_neg_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_neg_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_neg_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_recip_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_recip_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_rsqrt_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_rsqrt_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sqrt_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sqrt_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sub_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sub_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sub_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// FPU Branch Instructions
int asm_mips_bc1f(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bc1t(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// FPU Convert Instructions
int asm_mips_ceil_l_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ceil_l_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ceil_w_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ceil_w_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_d_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_d_w(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_d_l(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_l_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_l_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_ps_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_s_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_s_w(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_s_l(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_s_pl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_s_pu(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_w_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_cvt_w_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_floor_l_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_floor_l_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_floor_w_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_floor_w_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_round_l_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_round_l_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_round_w_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_round_w_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_trunc_l_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_trunc_l_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_trunc_w_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_trunc_w_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// FPU Load, Store and Memory Control Instructions
int asm_mips_ldc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_lwc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_sdc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_swc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// FPU Move Instructions
int asm_mips_cfc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_ctc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mfc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mtc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mov_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mov_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_mov_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movcf_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movcf_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movcf_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movn_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movn_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movn_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movz_s(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movz_d(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_movz_ps(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// FPU Absolute Branch Instructions
int asm_mips_bc1fl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_bc1tl(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);

// BUGFIX
int asm_mips_dmtc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
int asm_mips_dmfc1(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc);
