/*
** $Id: libasm.h,v 1.18 2007-05-29 00:40:27 heroine Exp $
** 
** Author  : <sk at devhell dot org>
** Started : Sat Oct 26 01:18:46 2002
** Updated : Thu Apr  8 00:45:41 2004
** 
*/

/**
 * @file libasm.h
 * It contains the main define types and prototypes.
 */

#ifndef LIBASM_H_
#define LIBASM_H_


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__FreeBSD__) || defined(sgi)
#include <sys/endian.h>
#elif !defined(sun)
#include <endian.h>
#else
#include <db.h>
#endif

#include <libaspect.h>


#define	LIBASM_USE_VECTOR		1
#define LIBASM_USE_OPERAND_VECTOR	1
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




/* Instruction types */
enum 
  {
    ASM_TYPE_NONE		=    0x0, //! Undefined instruction type.
    ASM_TYPE_IMPBRANCH		=    0x1, //! Branching instruction which always branch (jump).
    ASM_TYPE_CONDBRANCH		=    0x2, //! Conditionnal branching instruction.
    ASM_TYPE_CALLPROC		=    0x4, //! Sub Procedure calling instruction.
    ASM_TYPE_RETPROC		=    0x8, //! Return instruction
    ASM_TYPE_ARITH	  	=   0x10, //! Arithmetic (or logic) instruction.
    ASM_TYPE_LOAD		=   0x20, //! Instruction that reads from memory.
    ASM_TYPE_STORE		=   0x40, //! Instruction that writes in memory.
    ASM_TYPE_ARCH		=   0x80, //! Architecture dependent instruction.
    ASM_TYPE_FLAG		=  0x100, //! Flag-modifier instruction.
    ASM_TYPE_INT		=  0x200, //! Interrupt/call-gate instruction.
    ASM_TYPE_ASSIGN		=  0x400, //! Assignment instruction.
    ASM_TYPE_TEST		=  0x800, //! Instruction that performs comparison or test.
    ASM_TYPE_CONTROL		= 0x1000, //! Instruction modifies control registers.
    ASM_TYPE_NOP		= 0x2000, //! Instruction that does nothing.
    ASM_TYPE_OTHER		= 0x4000, //! Type that doesn't fit the ones above.
    ASM_TYPE_TOUCHSP		= 0x8000  //! Instruction modify stack pointer.
  } e_instr_types;

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
  
enum 
{
  ASM_PROC_IA32,
  ASM_PROC_SPARC,
  ASM_PROC_MIPS
} e_proc_type;


/*
  prototypes.
*/

/**
 * Initialize an asm_processor structure to disassemble
 * i386 instructions.
 *\param proc Pointer to an asm_processor structure
 */

void	asm_init_i386(asm_processor *);

/**
 * return build date.
 */
char	*asm_get_build(void);

/**
 * Initialize an asm_processor structure to disassemble
 * sparc instructions.
 *\param proc Pointer to an asm_processor structure
 */

void	asm_init_sparc(asm_processor *proc);

/**
 * Disassemble instruction 
 *
 *
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
 *
 */

int asm_register_ia32_opcode(int opcode, unsigned long fcn);
int asm_register_sparc_opcode(int opcode, int opcode2, int fpop,
			      unsigned long fcn);
int asm_arch_register(asm_processor *proc, int machine);
int asm_operand_register();
int asm_init_vectors(asm_processor *proc);

int	asm_operand_fetch(asm_operand *operand, u_char *opcode, int type, 
			  asm_instr *ins);
int	asm_operand_fetch_default(asm_operand *operand, u_char *opcode, int type, 
				  asm_instr *ins);
int	asm_operand_fetch_opmod(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_encoded(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_encodedbyte(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_general(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_jump(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_shortjump(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_offset(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_fixed(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_immediate(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_address(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_register(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_control(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_debug(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_xsrc(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_ydest(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_pmmx(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_memory(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_immediateword(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_segment(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);

enum 
  {
    LIBASM_ERROR_SUCCESS,
#define LIBASM_MSG_SUCCESS		"success"
    LIBASM_ERROR_NSUCH_CONTENT
#define LIBASM_MSG_NSUCH_CONTENT	"no such content"
  } e_libasm_errorcode;

#include <libasm-structs.h>
#include <libasm-i386.h>
#include <libasm-sparc.h>
#include <libasm-mips.h>

#endif




