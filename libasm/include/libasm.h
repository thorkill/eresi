/*
** 
** Author  : <sk at devhell dot org>
** Started : Sat Oct 26 01:18:46 2002
** Updated : Thu Apr  8 00:45:41 2004
*/

#ifndef LIBASM_H_
#define LIBASM_H_


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __FreeBSD__
#include <sys/endian.h>
#else
#include <endian.h>
#endif

/*
  typedef
*/

typedef struct s_asm_processor		asm_processor;
typedef struct s_asm_instr		asm_instr;
typedef struct s_asm_op			asm_operand;

typedef struct s_asm_processor		asm_processor_t;
typedef struct s_asm_instr		asm_instr_t;
typedef struct s_asm_op			asm_operand_t;

/*
  structures
*/


enum {
  ASM_TYPE_NONE,	//! 
  ASM_TYPE_IMPBRANCH,	//! Branching instruction which always branch (jump).
  ASM_TYPE_CONDBRANCH,	//! Conditionnal branching instruction.
  ASM_TYPE_CALLPROC,	//! Sub Procedure calling instruction.
  ASM_TYPE_RETPROC,	//! Return instruction
  ASM_TYPE_MEM_READ,	//! Memory read access instruction.
  ASM_TYPE_MEM_WRITE,	//! Memory write access instruction.
  ASM_TYPE_ARITH,	//! Arithmetic instruction.
  ASM_TYPE_LOAD,	//! Memory data loading instruction.
  ASM_TYPE_STORE,	//! Memory data storing instruction.
  ASM_TYPE_ARCH,	//! Architecture dependant instruction.
} e_asm_instr_type;


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



enum {
  LIBASM_ERROR_SUCCESS,
#define LIBASM_MSG_SUCCESS		"success"
  LIBASM_ERROR_NSUCH_CONTENT
#define LIBASM_MSG_NSUCH_CONTENT	"no such content"
} e_libasm_errorcode;

#include <libasm-structs.h>
#include <libasm-i386.h>
#include <libasm-sparc.h>

#endif




