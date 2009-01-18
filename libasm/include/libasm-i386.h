/**
 * @file libasm-i386.h
 * @brief Contains structures,prototypes and defines/enum related to ia32.
 * $Id$
 */

#ifndef LIBASM_I386_H_
#define LIBASM_I386_H_

#include <libasm-ia32-hdl.h>

typedef struct s_asm_i386_processor	asm_i386_processor;
typedef struct s_asm_i386_table		asm_i386_table;

/* Prototypes for IA32 specific API */
void		asm_resolve_ia32(void *d, eresi_Addr, char *, u_int);
int		asm_ia32_get_mode(asm_processor *proc);
eresi_Addr	asm_dest_resolve(asm_processor *proc, eresi_Addr, u_int shift);
int		asm_ia32_switch_mode(asm_processor *proc, int mode);

/** Are we in protected mode or real mode ? */
enum e_asm_proc_mode 
{
  INTEL_REAL,
  INTEL_PROT
};


/** IA32 related functions */
int	asm_content_pack(asm_operand *, int, int);
int	asm_fixed_pack(int, int, int, int);


/**
 * this structure is internal and may not be accessed by user
 * directly. it contains reference to each table describing i386 opcodes
 * refer to sandpile.org
 */
struct		s_asm_proc_i386 
{
  int		mode;		/*!< processor state: opsize actived or not	*/  
  int		vect_size;
  int		addsize;	/*!< WIPcurrent state of the processor addsize prefix */
  int		opsize;		/*!< WIPcurrent state of the processor addsize prefix */
  int		type;		/*!< WIPcurrent state of the processor addsize prefix */
};


/** 
 * Operand type : Those flags are stored in asm_operand.type field
 *
 * FIXME: MUST BE UNIFIED WITH libasm.h:156 e_op_types
 * WARNING: used for pretty printing in libasm/ia32/
 * Never used on others.
 */
#define ASM_OP_VALUE		1	/*!< immediate value present	*/
#define ASM_OP_BASE		2	/*!< base register present	*/
#define ASM_OP_INDEX		4	/*!< index register present	*/
#define ASM_OP_SCALE		8	/*!< scale factor present	*/

#define ASM_OP_REFERENCE	16	/*!< reference			*/
#define ASM_OP_ADDRESS		32	/*!< reference to a reference	*/
#define ASM_OP_FPU		64	/*!< operand is a FPU reference	*/


// address + reference
// base + address
// value + address
// reference + value
// reference + value + address



/**
 * prefix
 * |F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0| 
 * 
 * 0: rep
 * 1: repne
 * 2345: 
 * 0001: DS
 * 0010: ES
 * 0011: SS
 * 0100: CS
 * 0101: FS
 * 0110: GS
 * 6: lock
 * 7: opsize
 * 8: addsize
 */

/**
 * Enum of the intel available prefixes.
 */ 

enum e_ia32_prefix
  {
    ASM_PREFIX_REP	 =	1,	/*!< rep prefix	*/
    ASM_PREFIX_REPNE	 =      2,	/*!< repne prefix	*/

    ASM_PREFIX_SEG	=	60,	/*!< Segment prefix mask	*/

    ASM_PREFIX_DS	=	4,	/*!< ds prefix	*/
    ASM_PREFIX_ES	=	8,	/*!< es prefix	*/
    ASM_PREFIX_SS	=	12,	/*!< ss prefix	*/
    ASM_PREFIX_CS       =       16,	/*!< cs prefix	*/
    ASM_PREFIX_FS	=	20,	/*!< fs prefix	*/
    ASM_PREFIX_GS	=	24,

    ASM_PREFIX_MASK	=	60,

    ASM_PREFIX_LOCK	=	64,
    ASM_PREFIX_OPSIZE	=	128,
    ASM_PREFIX_ADDSIZE	=	256,
    ASM_PREFIX_FWAIT	=	512,
  };

/**
 * Control register bit flags
 *
 */
enum e_ia32_flags
  {
    ASM_FLAG_CF = 1 << 0,	/*!< carry flag	*/
    ASM_FLAG_PF = 1 << 2,	/*!< parity flag	*/
    ASM_FLAG_AF = 1 << 4,	/*!< ?? flag	*/
    ASM_FLAG_ZF = 1 << 6,	/*!< zero flag	*/
    ASM_FLAG_SF = 1 << 7,	/*!< signed flag	*/
    ASM_FLAG_TF = 1 << 8,	/*!< ?? flag	*/
    ASM_FLAG_IF = 1 << 9,	/*!< interrupt flag	*/
    ASM_FLAG_DF = 1 << 10,	/*!< debug flag ?	*/
    ASM_FLAG_OF = 1 << 11,
    ASM_FLAG_IOPL = 1 << 12,
    ASM_FLAG_NT = 1 << 14,

    ASM_FLAG_RF = 1 << 16,
    ASM_FLAG_VM = 1 << 17,
    ASM_FLAG_AC = 1 << 18,
    ASM_FLAG_VIF = 1 << 19,
    ASM_FLAG_VIP = 1 << 20,
    ASM_FLAG_ID = 1 << 21
  };


/**
 * Content of the struct s_operand content field
 */
enum e_asm_operand_content 
{
  /* no operand				
   */
  ASM_CONTENT_NONE,	
  /* operand is fixed in instruction coded by this opcode;
   */
  ASM_CONTENT_FIXED,	
  /* register is coded in mod field of instruction opcode.
   */
  ASM_CONTENT_OPMOD,	
  /* direct address; no mod R/M byte; 
   * address of operand is encoded in instruction;
   * no base register, index register, or scaling factor can be applied
   */
  ASM_CONTENT_ADDRESS,	
  /* reg field of mod R/M byte selects a control register
   */
  ASM_CONTENT_CONTROL,	
  /* reg field of mod R/M byte selects a debug register
   */
  ASM_CONTENT_DEBUG,	
  /* mod R/M byte follows opcode and specifies operand; 
   * operand is either a general register or a memory address;
   * if it is a memory address, the address is computed from 
   * a segment register and any of the following values:
   * a base register, an index register, a scaling factor, a displacement
   */
  ASM_CONTENT_ENCODED,	
  ASM_CONTENT_ENCODEDBYTE,	
  /* flags registers
   */
  ASM_CONTENT_FLAGS,	
  /* reg field of mod R/M byte selelcts a general register. 
   */
  ASM_CONTENT_GENERAL,	
  ASM_CONTENT_GENERALBYTE,	
  /* immediate data; value of operand is encoded in subsequent bytes 
   * of instruction
   */
  ASM_CONTENT_IMMEDIATE,	
  ASM_CONTENT_IMMEDIATEWORD,	
  /* immediate data; value of operand is encoded in subsequent bytes 
   * of instruction
   */
  ASM_CONTENT_IMMEDIATEBYTE,	
  /* instruction contains a relative offset to be added 
   * to the instruction pointer register
   */
  ASM_CONTENT_SHORTJUMP,	
  /* instruction contains a relative one byte offset to be added 
   * to the instruction pointer register
   */
  ASM_CONTENT_JUMP,	
  /* mod R/M only refer to memory		
   */
  ASM_CONTENT_MEMORY,	
  /* instruction has no mod R/M byte;                                                                                                            |   
   * offset of operand is coded as a word or double word (depending on 
   * address size attribute) in instruction;                                   |   
   * no base register, index register, or scaling factor can be applied; 
   * eg. MOV (A0..A3h)                          
   */
  ASM_CONTENT_OFFSET,	
  /* reg field of mod R/M byte selects a packed quadword MMX register
   */
  ASM_CONTENT_PMMX,	
  /* mod R/M byte follows opcode and specifies operand; operand is 
   * either an MMX register or a memory address;
   * if it is a memory address, the address is computed 
   * from a segment register and any of the following values:
   * |a base register, an index register, a scaling factor, a displacement
   */
  ASM_CONTENT_QMMX,	
  /*
   * mod field of mod R/M byte may refer only to a general register
   */
  ASM_CONTENT_REGISTER,
  /* reg field of mod R/M byte selects a segment register
   */
  ASM_CONTENT_SEGMENT,	
  /* no operand				
   */
  ASM_CONTENT_TEST,	
  /* no operand				
   */
  ASM_CONTENT_VSFP,	
  /* no operand				
   */
  ASM_CONTENT_WSFP,	
  /* memory addressed by ds:si		
   */
  ASM_CONTENT_XSRC,	
  /* memory addressed by es:di		
   */
  ASM_CONTENT_YDEST,
  /* immediate value encoded in instruction
   */
  ASM_CONTENT_VALUE,
  ASM_CONTENT_REG0,
  ASM_CONTENT_REG1,
  ASM_CONTENT_REG2,
  ASM_CONTENT_REG3,
  ASM_CONTENT_REG4,
  ASM_CONTENT_REG5,
  ASM_CONTENT_REG6,
  ASM_CONTENT_REG7,
  
  ASM_CONTENT_ST,
  ASM_CONTENT_ST_0,
  ASM_CONTENT_ST_1,
  ASM_CONTENT_ST_2,
  ASM_CONTENT_ST_3,
  ASM_CONTENT_ST_4,
  ASM_CONTENT_ST_5,
  ASM_CONTENT_ST_6,
  ASM_CONTENT_ST_7,
  ASM_CONTENT_NUM
};


/**
 * Content of the struct s_operand size field
 *
 */
enum e_asm_operand_size {
  ASM_OSIZE_NONE,
  ASM_OSIZE_BYTE,
  ASM_OSIZE_WORD,
  ASM_OSIZE_DWORD,
  ASM_OSIZE_QWORD,
  ASM_OSIZE_OWORD,
  ASM_OSIZE_CHAR,
  ASM_OSIZE_VECTOR,
  ASM_OSIZE_POINTER,
  ASM_OSIZE_ADDRESS,
  ASM_OSIZE_6BYTES
};


/**
 * Currently unsupported.
 */
enum e_asm_enc {
  ASM_ENC_NONE,
  ASM_ENC_ADDRESS,
  ASM_ENC_CONTROL,
  ASM_ENC_DEBUG,
  ASM_ENC_ENCODED,
  ASM_ENC_FLAGS,
  ASM_ENC_GENERAL,
  ASM_ENC_IMMEDIATE,
  ASM_ENC_JUMP,
  ASM_ENC_MEMORY,
  ASM_ENC_OFFSET,
  ASM_ENC_P,
  ASM_ENC_Q,
  ASM_ENC_R,
  ASM_ENC_SEGREG,
  ASM_ENC_TEST,
  ASM_ENC_VREGPACKEDFP,
  ASM_ENC_WMOD,
  ASM_ENC_SRC,
  ASM_ENC_DST,
  /*	*/
  ASM_ENC_REG0,
  ASM_ENC_REG1,
  ASM_ENC_REG2,
  ASM_ENC_REG3,
  ASM_ENC_REG4,
  ASM_ENC_REG5,
  ASM_ENC_REG6,
  ASM_ENC_REG7
};

#define ASM_ENC_ANY	(  ASM_ENC_ADDRESS | ASM_ENC_CONTROL | ASM_ENC_DEBUG | \
  ASM_ENC_ENCODED | ASM_ENC_FLAGS | ASM_ENC_GENERAL | ASM_ENC_IMMEDIATE | \
  ASM_ENC_JUMP | ASM_ENC_MEMORY | ASM_ENC_OFFSET | ASM_ENC_P | ASM_ENC_Q | \
  ASM_ENC_R | ASM_ENC_SEGREG | ASM_ENC_TEST | ASM_ENC_VREGPACKEDFP | ASM_ENC_WMOD | \
  ASM_ENC_SRC | ASM_ENC_DST | \
  ASM_ENC_REG0 | ASM_ENC_REG1 | ASM_ENC_REG2 | ASM_ENC_REG3 | ASM_ENC_REG4 | \
  ASM_ENC_REG5 | ASM_ENC_REG6 | ASM_ENC_REG7)

enum e_asm_size {
  ASM_SIZE_BYTE,
  ASM_SIZE_WORD,
  ASM_SIZE_DWORD,
  ASM_SIZE_QWORD,
  ASM_SIZE_OWORD,
  ASM_SIZE_CWORD,
  ASM_SIZE_VECTOR,
  ASM_SIZE_30BITS
};

#define ASM_SIZE_ANY	(ASM_SIZE_BYTE | ASM_SIZE_WORD | ASM_SIZE_DWORD |\
 ASM_SIZE_QWORD | ASM_SIZE_CWORD | ASM_SIZE_VECTOR)

/**
 * regset
 * 
 */

#define ASM_REGSET_R8		256		/* al,cl,dl...		*/
#define ASM_REGSET_R16		512		/* ax,cx,dx...		*/
#define ASM_REGSET_R32		1024		/* eax,ecx,edx...	*/
#define ASM_REGSET_MM		2048		/* mm0, mm1, mm2	*/
#define ASM_REGSET_XMM		4096		/* xmm0, xmm1...	*/
#define ASM_REGSET_SREG		8192		/* es, cs, ss ...	*/
#define ASM_REGSET_CREG		16384		/* cr0, cr1 ...		*/
#define ASM_REGSET_DREG		32768		/* dr0, dr1, ...	*/


/**
 * enum
 *
 * here follows enums for each regsets.
 *
 **/

/**
 * 8 bits registers set.
 */

enum e_asm_reg8 {
  ASM_REG_AL,	/* 000	*/
  ASM_REG_CL,	/* 001	*/
  ASM_REG_DL,	/* 010	*/
  ASM_REG_BL,	/* 011	*/
  ASM_REG_AH,	/* 100	*/
  ASM_REG_CH,	/* 101	*/
  ASM_REG_DH,	/* 110	*/
  ASM_REG_BH	/* 111	*/
};

/**
 * 16 bits registers set.
 */

enum e_asm_reg16 {
  ASM_REG_AX,	/* 000	*/
  ASM_REG_CX,	/* 001	*/
  ASM_REG_DX,	/* 010	*/
  ASM_REG_BX,	/* 011	*/
  ASM_REG_SP,	/* 100	*/
  ASM_REG_BP,	/* 101	*/
  ASM_REG_SI,	/* 110	*/
  ASM_REG_DI	/* 111	*/
};

/**
 * @brief 32 bits registers set.
 */

enum e_regset_r32 {
  ASM_REG_EAX,	/* 000	*/
  ASM_REG_ECX,	/* 001	*/
  ASM_REG_EDX,	/* 010	*/
  ASM_REG_EBX,	/* 011	*/
  ASM_REG_ESP,	/* 100	*/
  ASM_REG_EBP,	/* 101	*/
  ASM_REG_ESI,	/* 110	*/
  ASM_REG_EDI	/* 111	*/
 };

/**
 * @brief MM registers set.
 */

enum e_asm_regmm {
  ASM_REG_MM0,	/* 110	*/
  ASM_REG_MM1,	/* 110	*/
  ASM_REG_MM2,	/* 110	*/
  ASM_REG_MM3,	/* 110	*/
  ASM_REG_MM4,	/* 110	*/
  ASM_REG_MM6,	/* 110	*/
  ASM_REG_MM7	/* 110	*/
};

/**
 * @brief XMMS registers set.
 */
enum e_asm_regxmm {
  ASM_REG_XMM0,	/* 110	*/
  ASM_REG_XMM1,	/* 110	*/
  ASM_REG_XMM2,	/* 110	*/
  ASM_REG_XMM3,	/* 110	*/
  ASM_REG_XMM4,	/* 110	*/
  ASM_REG_XMM5,	/* 110	*/
  ASM_REG_XMM6,	/* 110	*/
  ASM_REG_XMM7	/* 110	*/
};

/**
 * @brief Segment registers set.
 */

enum e_asm_sreg {
  ASM_REG_ES,	/* 000	*/
  ASM_REG_CS,	/* 001	*/
  ASM_REG_SS,	/* 010	*/
  ASM_REG_DS,	/* 011	*/
  ASM_REG_FS,	/* 100	*/
  ASM_REG_GS,	/* 101	*/
  ASM_REG_SREGRES1,
  ASM_REG_SREGRES2

};

/**
 * @brief Control registers set.
 */

enum e_asm_creg {
  ASM_REG_CR0,	/* 000	*/
  ASM_REG_CR1,	/* 001	*/
  ASM_REG_CR2,	/* 010	*/
  ASM_REG_CR3,	/* 011	*/
  ASM_REG_CR4,	/* 100	*/
  ASM_REG_CR5,	/* 101	*/
  ASM_REG_CR6,	/* 110	*/
  ASM_REG_CR7	/* 111	*/
};

/**
 * @brief Debug registers set
 */

enum e_asm_dreg {
  ASM_REG_DR0,	/* 000	*/
  ASM_REG_DR1,	/* 001	*/
  ASM_REG_DR2,	/* 010	*/
  ASM_REG_DR3,	/* 011	*/
  ASM_REG_DR4,	/* 100	*/
  ASM_REG_DR5,	/* 101	*/
  ASM_REG_DR6,	/* 110	*/
  ASM_REG_DR7	/* 111	*/
};

/**
 * @brief Instruction list.
 * Last instruction must be ASM_BAD
 * If NOT, this may produce allocation error as ASM_BAD is used to allocate 
 * size of the instruction label array.
 * Refer to init_instr_table in tables_i386.c
 */

enum asm_instr {
  ASM_NONE,  
  /* special instr id	*/
  ASM_IPREFIX_MIN,
  ASM_IPREFIX_CS,
  ASM_IPREFIX_DS,
  ASM_IPREFIX_ES,
  ASM_IPREFIX_FS,
  ASM_IPREFIX_GS,
  ASM_IPREFIX_SS,
  ASM_IPREFIX_OPSIZE,
  ASM_IPREFIX_ADSIZE,
  ASM_IPREFIX_MAX,
  ASM_GROUP_MIN,
  ASM_GROUP1,
  ASM_GROUP2,
  ASM_GROUP3,
  ASM_GROUP4,
  ASM_GROUP5,
  ASM_GROUP6,
  ASM_GROUP7,
  ASM_GROUP8,
  ASM_GROUP9,
  ASM_GROUP10,
  ASM_GROUP11,
  ASM_GROUP12,
  ASM_GROUP13,
  ASM_GROUP14,
  ASM_GROUP15,
  ASM_GROUP16,
  ASM_GROUP17,
  ASM_ESC0,
  ASM_ESC1,
  ASM_ESC2,
  ASM_ESC3,
  ASM_ESC4,
  ASM_ESC5,
  ASM_ESC6,
  ASM_ESC7,
  ASM_GROUP_MAX,
  ASM_2BYTES,
  
  /* instructions	*/
  
  ASM_CALL,
  ASM_BRANCH,
  ASM_BRANCH_U_LESS,
  ASM_BRANCH_U_LESS_EQUAL,
  ASM_BRANCH_S_LESS,
  ASM_BRANCH_S_LESS_EQUAL,
  ASM_BRANCH_U_GREATER,
  ASM_BRANCH_U_GREATER_EQUAL,
  ASM_BRANCH_S_GREATER,
  ASM_BRANCH_S_GREATER_EQUAL,
  /* 10 */
  ASM_BRANCH_EQUAL,
  ASM_BRANCH_NOT_EQUAL,
  /* x86 specific */
  ASM_BRANCH_PARITY,
  ASM_BRANCH_NOT_PARITY,
  ASM_BRANCH_OVERFLOW,
  /* 15 */
  ASM_BRANCH_NOT_OVERFLOW,
  ASM_BRANCH_SIGNED,
  ASM_BRANCH_NOT_SIGNED,
  ASM_BRANCH_CXZ,
  /* */
  ASM_JO,
  ASM_JNO,
  ASM_JB,
  ASM_JNB,
  ASM_JZ,
  ASM_JNZ,
  ASM_JBE,
  ASM_JNBE,
  ASM_JS,
  ASM_JNS,
  ASM_JP,
  ASM_JNP,
  ASM_JL,
  ASM_JNL,
  ASM_JLE,
  ASM_JNLE,
  /* generic instruction */
  ASM_ADD,
  ASM_CBW,
  ASM_CWD,
  /* 20 */
  ASM_LEA,
  ASM_OR,
  ASM_ADC,
  ASM_SBB,
  ASM_AND,
  /* 25 */
  ASM_SUB,
  ASM_XOR,
  ASM_XADD,
  ASM_CLFLUSH,
  ASM_CMP,
  ASM_IN,
  ASM_INC,
  /* 30 */
  ASM_DEC,
  ASM_PUSH,
  ASM_PUSHF,
  ASM_CWTD,
  ASM_CBTW,
  ASM_POP,
  ASM_POPF,
  /* 35 */
  ASM_TEST,
  ASM_NOP,
  ASM_LOAD,
  ASM_STORE,
  ASM_MOV,
  ASM_MOVW,
  /* 40 */
  ASM_INT,
  ASM_RET,
  ASM_XCHG,
  ASM_INSB,
  ASM_INSW,
  ASM_INSD,
  ASM_PUSHA,
  ASM_POPA,
  /* 45 */
  ASM_AAM,
  ASM_AAD,
  ASM_LOCK,
  ASM_LOOP,
  ASM_LOOPE,
  /* 50 */
  ASM_LOOPNE,
  ASM_JECX,
  ASM_SALC,
  ASM_LEAVE,
  ASM_HLT,
  ASM_SHR,
  ASM_DIV,
  /* 55 */
  ASM_IDIV,
  ASM_MUL,
  ASM_IMUL,
  ASM_NOT,
  ASM_NEG,
  /* 60 */
  ASM_SAHF,
  ASM_LAHF,
  ASM_MOVSB,
  ASM_MOVSBW,
  ASM_MOVSD,
  ASM_CMPSB,
  ASM_CMPSW,
  /* 65 */
  ASM_CMPSD,
  ASM_STOSB,
  ASM_STOSW,
  ASM_STOSD,
  ASM_LODSB,
  ASM_LODSD,
  /* 70 */
  ASM_SCASB,
  ASM_SCASD,
  ASM_INTO,
  ASM_IRET,
  ASM_SHIFT,
  /* 75 */
  ASM_XLATB,
  ASM_XLAT,
  ASM_ORB,
  /* 80 */
  ASM_MOVZWL,
  ASM_CLD,
  ASM_SMSW,
  ASM_LMSW,
  /* 85 */
  ASM_STD,
  ASM_OUT,
  ASM_SHL,
  ASM_RETF,
  /* 90 */
  ASM_ENTER,
  ASM_MOVZBL,
  ASM_REPNZ,
  ASM_REPZ,
  ASM_SET_OVERFLOW,
  /* 95 */
  ASM_SET_NOT_OVERFLOW,
  ASM_SET_PARITY,
  ASM_SET_NOT_PARITY,
  ASM_SET_SIGNED,
  ASM_SET_NOT_SIGNED,
  ASM_SET_EQUAL,
  ASM_SET_NOT_EQUAL,
  ASM_SET_U_LESS,
  ASM_SET_U_LESS_EQUAL,
  ASM_SET_S_LESS,
  ASM_SET_S_LESS_EQUAL,
  ASM_SET_U_GREATER,
  ASM_SET_U_GREATER_EQUAL,
  ASM_SET_S_GREATER,
  ASM_SET_S_GREATER_EQUAL,
  ASM_CPUID,
  ASM_SHRD,
  ASM_SHLD,
  ASM_CLTD,
  ASM_SAR,
  ASM_ROR,
  ASM_BT,
  ASM_BTS,
  ASM_INT3,
  ASM_CMC,
  ASM_RCL,
  ASM_SAL,
  ASM_ROL,
  ASM_SCASW,
  ASM_WAIT,
  ASM_FWAIT,
  ASM_BSR,
  ASM_BSF,
  ASM_RCR,
  ASM_CLI,
  ASM_CWTL,
  ASM_OUTSB,
  ASM_OUTSW,
  ASM_OUTSD,
  ASM_STI,
  ASM_CMOVNE,
  ASM_LSS,
  ASM_MOVSW,
  ASM_MOVZBW,
  ASM_BOUND,
  ASM_LES,
  ASM_LDS,
  ASM_AAA,
  ASM_DAA,
  ASM_ARPL,
  ASM_CLC,
  ASM_STC,
  ASM_INT1,

  ASM_VERR,
  ASM_VERW,
  ASM_BRANCHE,

  ASM_AAS,
  ASM_DAS,
  ASM_UD2A,
  ASM_UD2,
  ASM_BTC,
  ASM_CMPXCHG,
  ASM_LIDT,
  ASM_LGDT,
  
  ASM_FXSAVE,
  ASM_FXRSTOR,
  ASM_LDMXCSR,
  ASM_STMXCSR,
  ASM_LFENCE,
  ASM_MLENCE,
  ASM_SLENCE,

  ASM_WBINVD,
  ASM_RDMSR,
  ASM_XSTORERNG,
  ASM_XCRYPTCBC,
  ASM_XCRYPTCFB,
  ASM_XCRYPTOFB,
  ASM_BTRL,

  ASM_PREFETCH_NTA,
  ASM_PREFETCH_T0,
  ASM_PREFETCH_T1,
  ASM_PREFETCH_T2,
  ASM_HINT_NOP,
  
  ASM_SGDT,
  ASM_SIDT,
  ASM_STR,
  ASM_BTR,
  ASM_LTR,
  ASM_SLDT,
  ASM_INVLPG,
  /* 150 */
  ASM_RDTSC,
  ASM_LLDT,
  ASM_LBRANCH,
  ASM_CMOVA,
  ASM_CMOVE,
  ASM_CMOVAE,
  ASM_CMOVO,
  ASM_CMOVNO,
  ASM_CMOVB,
  ASM_CMOVBE,
  ASM_CMOVS,
  ASM_CMOVNS,
  ASM_CMOVP,
  ASM_CMOVNP,
  ASM_CMOVL,
  ASM_CMOVNL,
  ASM_CMOVLE,
  ASM_CMOVNLE,
  ASM_MOVSBL,
  ASM_MOVSWL,
  ASM_MOVD,
  ASM_MOVQ,
  ASM_BSWAP,
  ASM_PAND,
  ASM_POR,
  ASM_PXOR,
  ASM_PSLLQ,
  ASM_PSRLQ,
  ASM_PSRLW,
  ASM_PSRAW,
  ASM_PSLLW,
  ASM_PMULLW,
  ASM_PADDUSW,
  ASM_PADDUSB,
  ASM_PUNPCKLBW,
  ASM_PUNPCKHBW,
  ASM_PACKUSWB,
  ASM_EMMS,

  /*
   *
   */

  ASM_FXRSTORE,
  ASM_MFENCE,
  ASM_SFENCE,

  /*
   * FPU INSTRUCTIONS
   */
  ASM_F2XM1,		/* Function 2^X Minus 1 */
  ASM_FABS,		/* Function ABSolute value */
  ASM_FADD,		/* Function ADD */
  ASM_FADDP,		/* Function ADD and Pop */
  ASM_FBLD,		/* Function BCD LoaD */
  ASM_FBSTP,		/* Function BCD STore and Pop */
  ASM_FCHS,		/* Function CHange Sign */
  ASM_FCLEX,		/* Function CLear EXceptions with wait */
  ASM_FCMOVB,
  ASM_FCMOVU,
  ASM_FCOM,		/* Functioon COMpare real */
  ASM_FCOMP,		/* Function COMpare reald and Pop */
  ASM_FCOMPP,		/* Function COMpare reald and PoP twice */
  ASM_FCOMPS,		/*						*/
  ASM_FDECSTP,		/* Function DECrement STack Pointer		*/
  ASM_FDISI,		/* Function DISable Interrupts with wait	*/
  ASM_FDIV,		/* Function real DIVide				*/
  ASM_FDIVP,		/* Function real DIVide and Pop			*/
  ASM_FDIVR,		/* Function real DIVide Reversed		*/
  ASM_DIVRP,		/* Function real DIVide Reversed and Pop	*/
  ASM_FENI,		/* Function ENable Interrupts with wait		*/
  ASM_FFREE,		/* Function FREE register			*/
  ASM_FIADD,		/* Function Integer ADD				*/
  ASM_FICOM,		/* Function Integer COMpare			*/
  ASM_FICOMP,		/* Function Integer COMpare and Pop		*/
  ASM_FIDIV,		/* Function Integer DIVide			*/
  ASM_FIDIVR,		/* Function Integer DIVide Reversed		*/
  ASM_FILD,		/* Function Integer LoaD			*/
  ASM_FIMUL,		/* Function Integer Multiply			*/
  ASM_FINCSTP,		/* Function INCrement STac Pointer		*/
  ASM_FINIT,		/* Function INITialize coprocessor with wait	*/
  ASM_FIST,		/* Function Integer STore			*/
  ASM_FISTP,		/* Function Integer STore and Pop		*/
  ASM_FISUB,		/*						*/
  ASM_FISUBR,		/*						*/
  ASM_FUCOM,		/*						*/
  ASM_FUCOMP,		/*						*/
  ASM_FUCOMPP,		/*						*/
  ASM_FLDLN2,
  ASM_FTST,
  ASM_FPREM,
  ASM_FRNDINT,
  ASM_FNOP,
  ASM_FCMOVNE,
  ASM_FSTP1,
  ASM_FSCALE,
  ASM_FSQRT,
  ASM_FSIN,
  ASM_FCOS,
  ASM_FISTTP,
  ASM_FDIVRP,		/*						*/
  ASM_FREEP,
  ASM_FSUBP,		/*						*/
  ASM_FSUBRP,		/*						*/
  ASM_FCMOVBE,
  ASM_FMUL,		/*						*/
  ASM_FMULP,		/*						*/
  ASM_FRSTOR,		/*						*/
  ASM_FSAVE,		/*						*/
  ASM_FNSAVE,		/*						*/
  ASM_FSTP,		/* Function STore real and Pop - 8087		*/
  ASM_FSUB,		/*						*/
  ASM_FSUBR,		/*						*/
  ASM_FPATAN,
  ASM_FST,		/*						*/
  ASM_FXAM,
  ASM_FCMOVNBE,
  ASM_FCMOVNU,
  ASM_FXCH,
  ASM_FSINCOS,
  ASM_FYL2X,
  ASM_FSTENV,
  ASM_FLDL2T,
  ASM_FLDL2E,
  ASM_FLDPI,
  ASM_FPTAN,
  ASM_FXTRACT,
  ASM_FYL2XP1,
  ASM_FLD,		/* Function LoaD real - 8087			*/
  ASM_FLDZ,		/*						*/
  ASM_FLD1,
  ASM_FSTSW,		/*						*/
  ASM_FNSTSW,		/*						*/
  AM_FNSTSW,		/* Function (No wait) STore Status Word - 8087	*/
  ASM_FCMOVE,
  ASM_FCMOVNB,
  ASM_FLDLG2,
  ASM_FSORT,
  ASM_FXCH4,
  ASM_FTSTP,
  ASM_FPEM1,
  ASM_FLDCW,		/*						*/
  ASM_FNSTENV,
  ASM_FSTCW,
  ASM_FNSTCW,
  ASM_FLDENV,
  ASM_FSETPM,
  ASM_FRSTPM,
  ASM_FUCOMI,
  ASM_FRICHOP,
  ASM_FCOMI,
  ASM_FCOMP5,
  ASM_FRINT2,
  ASM_FCOM2,
  ASM_FCOMP3,
  ASM_FXCH7,
  ASM_FSTP8,
  ASM_FSTP9,
  ASM_FSTDW,
  ASM_FSTSG,
  ASM_FUCOMIP,
  ASM_FRINEAR,
  ASM_FCOMIP,
  /**
   * keep this in last position unless 
   * you know what you are doing 
   **/
  ASM_BAD
};
/*
 * specialisation asm_processor for i386
 */

struct s_asm_i386_processor {
  /*	handler to resolving function		*/
  void                  (*resolve_immediate)(void *, u_int, char *, u_int);
  
  /*	handler data pointer	*/
  void                  *resolve_data;
  
  /*	processor type	. I386 supports only	*/
  int                   type;
  
  /*						*/
  char                  **instr_table;
  
  int			(*fetch)(asm_instr *, u_char *, u_int, 
				 asm_processor *);
  /*    output handler. print instruction in a readable string		*/
  char			*(*display_handle)(asm_instr *instr, int addr);
  
  /*	pointer to an internal structure.	*/
  struct s_asm_proc_i386                  *internals;
};

#endif
