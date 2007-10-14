/*
** $Id: libasm.h,v 1.27 2007-10-14 09:45:06 heroine Exp $
** 
** Author  : <sk at devhell dot org>
** Started : Sat Oct 26 01:18:46 2002
** Updated : Thu Apr  8 00:45:41 2004
** 
*/

/**
 * @file libasm.h
 * @brief It contains the main define types and prototypes.
 * @todo Reorganize include files content.
 */

/**
 * @defgroup operands Operands related API
 */

/**
 * @defgroup handlers_ia32 IA32 Fechting handlers.
 */

#ifndef LIBASM_H_
#define LIBASM_H_


//#include <config.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__FreeBSD__) || defined(sgi) || defined(__NetBSD__)
#include <sys/endian.h>
#elif !defined(sun)
#include <endian.h>
#else
#include <db.h>
#endif

#include <libaspect.h>


#define LIBASM_SUPPORT_SPARC		1
#define LIBASM_SUPPORT_IA32		1
#define LIBASM_SUPPORT_MIPS		0	/*<! XXX: Wanted	*/
#define LIBASM_SUPPORT_POWERPC		0	/*<! XXX: Wanted	*/
#define LIBASM_SUPPORT_ARM		0	/*<! XXX: Wanted	*/
#define	LIBASM_VECTOR_OPCODE_IA32	"opcode-ia32"
#define	LIBASM_VECTOR_OPCODE_SPARC	"opcode-sparc"
#define	LIBASM_VECTOR_OPERAND_IA32	"operand-ia32"
#define	LIBASM_VECTOR_OPERAND_SPARC	"operand-sparc"


/**
 * Set to 1 to use libasm vector.
 * @obselete
 */
#define	LIBASM_USE_VECTOR		1
/**
 * Set to 1 to use libasm operand vector.
 * @obselete
 */
#define LIBASM_USE_OPERAND_VECTOR	1
/**
 * Set to 1 to use profiling.
 * @todo Profiling - Work in progress
 */
#define LIBASM_USE_PROFILE		0

#if LIBASM_USE_PROFILE
#define LIBASM_PROFILE_FIN()			\
  fprintf(stderr, " ENTER %s\n", __FUNCTION__);


#define LIBASM_PROFILE_IN()			\
  fprintf(stderr, " ENTER %s\n", __FUNCTION__);

#define LIBASM_PROFILE_FOUT(val)		\
  fprintf(stderr, " LEAVE %s\n", __FUNCTION__);	\
  return (val);

#define LIBASM_PROFILE_VOUT()		\
  fprintf(stderr, " LEAVE %s\n", __FUNCTION__);	\
  return;

#define LIBASM_PROFILE_ROUT(val)		\
  fprintf(stderr, " LEAVE %s\n", __FUNCTION__);	\
  return (val);


#else
//
// Profile disabled.
//
#define LIBASM_PROFILE_FIN()
#define LIBASM_PROFILE_IN()
#define LIBASM_PROFILE_FOUT(val) return (val);

#define LIBASM_PROFILE_VOUT() return;

#define LIBASM_PROFILE_ROUT(val) return (val);
#endif


/**
 * Architecture-independant instruction types 
 *
 */
enum 
  {
    ASM_TYPE_NONE	        = 0x0, //!< Undefined instruction type.
    ASM_TYPE_IMPBRANCH    = 0x1, //!< Branching instruction which always branch (jump).
    ASM_TYPE_CONDBRANCH	  = 0x2, //!< Conditionnal branching instruction.
    ASM_TYPE_CALLPROC	    = 0x4, //!< Sub Procedure calling instruction.
    ASM_TYPE_RETPROC      = 0x8, //!< Return instruction
    ASM_TYPE_ARITH        = 0x10, //!< Arithmetic (or logic) instruction.
    ASM_TYPE_LOAD	        = 0x20, //!< Instruction that reads from memory.
    ASM_TYPE_STORE        = 0x40, //!< Instruction that writes in memory.
    ASM_TYPE_ARCH	        = 0x80, //!< Architecture dependent instruction.
    ASM_TYPE_WRITEFLAG    = 0x100, //!< Flag-modifier instruction.
    ASM_TYPE_READFLAG     = 0x200, //!< Flag-reader instruction.
    ASM_TYPE_INT          = 0x400, //!< Interrupt/call-gate instruction.
    ASM_TYPE_ASSIGN	      = 0x800, //!< Assignment instruction.
    ASM_TYPE_COMPARISON	  = 0x1000, //!< Instruction that performs comparison or test.
    ASM_TYPE_CONTROL      = 0x2000, //!< Instruction modifies control registers.
    ASM_TYPE_NOP          = 0x4000, //!< Instruction that does nothing.
    ASM_TYPE_OTHER        = 0x8000, //!< Type that doesn't fit the ones above.
    ASM_TYPE_TOUCHSP      = 0x10000, //!< Instruction modifies stack pointer.
    ASM_TYPE_BITTEST      = 0x20000, //!< Instruction investigates values of bits in the operands.
    ASM_TYPE_BITSET	      = 0x40000, //!< Instruction modifies values of bits in the operands.
    ASM_TYPE_INCDEC       = 0x80000, //!< Instruction does an increment or decrement
    ASM_TYPE_PROLOG       = 0x100000, //!< Instruction creates a new function prolog
    ASM_TYPE_EPILOG       = 0x200000, //!< Instruction creates a new function epilog
    ASM_TYPE_STOP         = 0x400000, //!< Instruction stops the program
    ASM_TYPE_IO           = 0x80000 //!< Instruction accesses I/O locations (e.g. ports).
  } e_instr_types;


/**
 * Architecture-independant operand types 
 *
 */
enum
  {
    ASM_OPTYPE_NONE = 0x0, //!< Undefined operand type
    ASM_OPTYPE_IMM  = 0x1, //!< Immediate operand type
    ASM_OPTYPE_REG  = 0x2, //!< Register operand type
    ASM_OPTYPE_MEM  = 0x4  //!< Memory access operand type
  } e_op_types;

/* this is needed for mips implementation */
#define ASM_CONFIG_ENDIAN_FLAG "libasm.endian.flag"
#define ASM_CONFIG_LITTLE_ENDIAN 0
#define ASM_CONFIG_BIG_ENDIAN 1

#define ASM_CONFIG_ATT_MARGIN_DEFAULT	14
#define ASM_CONFIG_ATT_MARGIN_FLAG "libasm.output.att.margin"



/*
  typedef's
*/

typedef struct s_asm_processor  asm_processor;
typedef struct s_asm_instr      asm_instr;
typedef struct s_asm_op         asm_operand;

enum 
{
  LIBASM_VECTOR_IA32,
  LIBASM_VECTOR_SPARC,
  LIBASM_VECTOR_MIPS,
  LIBASM_VECTOR_ARCHNUM
} e_vector_arch;


/**
 * List of architecture.
 */
enum 
{
  ASM_PROC_IA32,	//!< Architecture IA32
  ASM_PROC_SPARC,	//!< Architecture SPARC
  ASM_PROC_MIPS		//!< Architecture MIPS
} e_proc_type;


/*
  prototypes.
*/

/**
 * Initialize an asm_processor structure to disassemble
 */

int	asm_init_ia32(asm_processor *proc);
int	asm_init_i386(asm_processor *proc); /*!< XXX:Obsolete */
int	asm_init_sparc(asm_processor *proc);
int	asm_init_mips(asm_processor *proc);

/**
 * return build date.
 */
char	*asm_get_build(void);

/**
 * Initialize an asm_processor structure to disassemble
 * sparc instructions.
 *\param proc Pointer to an asm_processor structure
 */

/**
 * Disassemble instruction 
 */
int	asm_read_instr(asm_instr *instr, u_char *buffer, u_int len, asm_processor *proc);

/**
 * Return instruction length.
 * 
 */
int	asm_instr_len(asm_instr *);

/**
 * Return instruction length.
 *
 */
int		asm_instruction_get_len(asm_instr *, int, void *);

/**
 * Return instruction mnemonic.
 */

char	*asm_instr_get_memonic(asm_instr *, asm_processor *);

/**
 * Return a pointer
 */

char	*asm_display_instr_att(asm_instr *, int);

void	asm_clear_instr(asm_instr *);

/**
 * Set libasm error
 */
void	asm_set_error(asm_instr *, int errorcode, char *error_msg);
int	asm_get_error_code();
char*	asm_get_error_message();

/**
 * operand field access methods.
 * use this to keep compatibility.
 **/

/**
 * Return instruction number of operands.
 *
 */

int		asm_operand_get_count(asm_instr *instr, int num, int, void *);

/**
 * Return operand content.
 */

int		asm_operand_get_content(asm_instr *, int, int, void *);

/******************
 *
 */

void		*asm_opcode_fetch(const char *vector_name, int opcode);

/**
 * Return operand type.
 */

int		asm_operand_get_type(asm_instr *, int, int, void *);

/**
 * Return operand size
 */

int		asm_operand_get_size(asm_instr *, int, int, void *);

int		asm_operand_get_len(asm_instr *, int, int, void *);
int		asm_operand_set_value(asm_instr *, int, int, void *);
int		asm_operand_get_value(asm_instr *, int, int, void *);

/**
 * immediate field may be a 1,2,4,8 ... byte long value
 * to set it,
 */

int		asm_operand_debug(asm_instr *, int, int, void *);

/**
 * Dump content of instruction
 *
 */
void		asm_instruction_debug(asm_instr *ins, FILE *out);

/**
 * Free asm processor internal structures
 */
void		asm_free_i386(asm_processor *proc);

/**
 * Get immediate value stored in operand.
 */

int		asm_operand_get_immediate(asm_instr *, int, int, void *);

/**
 * Set immediate value stored in operand.
 */

int		asm_operand_set_immediate(asm_instr *, int, int, void *);
/**
 * Get base register stored in operand.
 */
int		asm_operand_get_basereg(asm_instr *, int, int, void *);
int		asm_operand_set_basereg(asm_instr *, int, int, void *);
/**
 * Get index register stored in operand (IA32 only).
 */

int		asm_operand_get_indexreg(asm_instr *, int, int, void *);

/**
 * Set index register stored in operand (IA32 only).
 */

int		asm_operand_set_indexreg(asm_instr *, int, int, void *);

/**
 * Get index scale stored in operand (IA32 only).
 */

int		asm_operand_get_scale(asm_instr *, int num, int opt, void *);

/**
 * Set index scale stored in operand (IA32 only).
 */
int		asm_operand_set_scale(asm_instr *, int num, int opt, void *);

/**
 *
 */
int		asm_operand_is_reference(asm_operand *op);
int		asm_instruction_is_prefixed(asm_instr *ins, int prefix);

/**
 * Set resolve handler used by asm_display_instr() to resolve immediate value.
 */
void		asm_set_resolve_handler(asm_processor *,
					void (*)(void *, u_int, char *, u_int), 
					void *);

/**
 *
 */
char    *asm_operand_type_string(int type);

/**
 * IA32 related : should be moved in libasm-ia32.h
 */
int asm_register_ia32_opcode(int opcode, unsigned long fcn);
int asm_register_sparc_opcode(int opcode, int opcode2, int fpop,
			      unsigned long fcn);

/**
 * XXX: This is obsolete.
 */
int asm_arch_register(asm_processor *proc, int machine);


int asm_operand_register(asm_processor *proc);


int asm_init_vectors(asm_processor *proc);


/**
 * One dimension vector facilities.
 */

int	asm_register_opcode_create(const char *name, int num);
int	asm_register_operand_create(const char *name, int num);
int	asm_register_opcode(const char *, int, unsigned long fcn);
int	asm_register_operand(const char *, int, unsigned long fcn);

/**
 * Default handlers.
 *
 */
int	asm_fetch_default(asm_instr *, u_char *opcode, u_int len, asm_processor *proc);



/**
 * Return an ascii static string corresponding to a register.
 * @param reg Register
 * @param regset Register set (ie: ASM_REGSET_R32)
 * @return An ascii string.
 */
char	*get_reg_intel(int reg, int regset);


/**
 * @brief enum of the error codes available in asm_processor->error_code.
 * XXX: Being implemented. Currently not accurate.
 */
enum 
  {
#define LIBASM_MSG_ERRORNOTIMPLEMENTED	"error message not implemented"
    LIBASM_ERROR_SUCCESS,
#define LIBASM_MSG_SUCCESS		"success"
    LIBASM_ERROR_NSUCH_CONTENT,
#define LIBASM_MSG_NSUCH_CONTENT	"no such content"
    LIBASM_ERROR_ILLEGAL,
#define LIBASM_MSG_ILLEGAL		"illegal instruction"
    LIBASM_ERROR_TOOSHORT,
#define LIBASM_MSG_TOOSHORT		"data length too short"
  } e_libasm_errorcode;

#include <libasm-structs.h>
#include <libasm-i386.h>
#include <libasm-sparc.h>
#include <libasm-mips.h>

#endif




