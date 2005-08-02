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


/*
  typedef
*/

typedef struct s_asm_i386_processor	asm_i386_processor;
typedef struct s_asm_processor		asm_processor;
typedef struct s_asm_instr		asm_instr;
typedef struct s_asm_op			asm_operand;
typedef struct s_asm_i386_table		asm_i386_table;

/*
  structures
*/

/*
 * struct s_asm_op
 * 
 * 
 */

struct s_asm_op {
  /* operand length
   * operands expressed in R byte from ModRM byte have a null size.	*/
  u_int		len;
  /* pointer to operand in buffer					*/
  u_char	*ptr;
  /* operand type
   * may contain flag listed below to enumerate operand components	*/
  u_int		type;
  u_int		size;
  u_int		content;
  /* register set: 8/16/32 bits general registers, segment registers ..	
   full list following */
  int		regset;
  
  int		prefix;
  asm_processor *proc;
  
  /*	immediate value extracted from operand	*/
  int		imm; 
  /*	base register				*/
  int		base_reg;
  /*	index register				*/
  int		index_reg;
  /*	scale factor				*/
  unsigned int	scale;
 
};

/** 
 * operand type
 * 
 */

#define ASM_OP_VALUE		1	/* immediate value present	*/
#define ASM_OP_BASE		2	/* base register present	*/
#define ASM_OP_INDEX		4	/* index register present	*/
#define ASM_OP_SCALE		8	/* scale factor present		*/

#define ASM_OP_FIXED		16
#define ASM_OP_REFERENCE	32	/* reference			*/
#define ASM_OP_ADDRESS		64	/* reference to a reference	*/
#define ASM_OP_FPU		128	/* operand is a FPU reference	*/

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

/*
 * struct s_asm_table
 *
 * this structure is internally used to
 * map instruction refering to sandpile.org
 * documentation.
 *
 */

struct s_optable {
    int	coded;
    int	size;
};

struct s_asm_i386_table {
  u_char	id;
  int		instr;
  int		type;
  struct s_optable	op1;
  struct s_optable	op2;
  struct s_optable	op3;
};

enum {
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
} e_asm_enc;

#define ASM_ENC_ANY	(  ASM_ENC_ADDRESS | ASM_ENC_CONTROL | ASM_ENC_DEBUG | \
  ASM_ENC_ENCODED | ASM_ENC_FLAGS | ASM_ENC_GENERAL | ASM_ENC_IMMEDIATE | \
  ASM_ENC_JUMP | ASM_ENC_MEMORY | ASM_ENC_OFFSET | ASM_ENC_P | ASM_ENC_Q | \
  ASM_ENC_R | ASM_ENC_SEGREG | ASM_ENC_TEST | ASM_ENC_VREGPACKEDFP | ASM_ENC_WMOD | \
  ASM_ENC_SRC | ASM_ENC_DST | \
  ASM_ENC_REG0 | ASM_ENC_REG1 | ASM_ENC_REG2 | ASM_ENC_REG3 | ASM_ENC_REG4 | \
  ASM_ENC_REG5 | ASM_ENC_REG6 | ASM_ENC_REG7)

enum {
  ASM_SIZE_BYTE,
  ASM_SIZE_WORD,
  ASM_SIZE_DWORD,
  ASM_SIZE_QWORD,
  ASM_SIZE_OWORD,
  ASM_SIZE_CWORD,
  ASM_SIZE_VECTOR,
  ASM_SIZE_30BITS
} e_asm_size;

#define ASM_SIZE_ANY	(ASM_SIZE_BYTE | ASM_SIZE_WORD | ASM_SIZE_DWORD |\
 ASM_SIZE_QWORD | ASM_SIZE_CWORD | ASM_SIZE_VECTOR)



/*
 * instruction structure.
 *
 *
 */

struct s_asm_instr {
  /*	pointer to instruction buffer	*/
  u_char                *ptr_instr;  
  void			*ptr_prefix;
  /*	instruction/operands full lengh	*/
  u_int                 len;
  /*	internal processor structure	*/
  asm_processor		*proc;
  /*	instruction			*/
  // asm_i386_table	i386table;
  /*					*/
  int                   instr;
  /*	instruction type		*/
  int                   type;
  /*	instruction prefix		*/
  int                   prefix;
  /*	number of operand		*/
  int			nb_op;
  /*	operands			*/
  asm_operand		op1;
  asm_operand		op2;
  asm_operand		op3;  
};

enum {
  ASM_TYPE_NONE,
  ASM_TYPE_IMPBRANCH,
  ASM_TYPE_CONDBRANCH,
  ASM_TYPE_CALLPROC,
  ASM_TYPE_RETPROC,
  ASM_TYPE_MEM_READ,
  ASM_TYPE_MEM_WRITE,
  ASM_TYPE_ARITH,
  ASM_TYPE_ARCH
  
  /*  j aime pas qu on lise mon ecran...  */

} e_asm_instr_type;

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

#define	ASM_PREFIX_REP		1
#define ASM_PREFIX_REPNE	2

#define ASM_PREFIX_SEG		60

#define ASM_PREFIX_DS		4
#define	ASM_PREFIX_ES		8
#define ASM_PREFIX_SS		12
#define ASM_PREFIX_CS           16
#define ASM_PREFIX_FS		20
#define ASM_PREFIX_GS		24

#define ASM_PREFIX_MASK		60

#define ASM_PREFIX_LOCK		64
#define ASM_PREFIX_OPSIZE	128
#define ASM_PREFIX_ADDSIZE	256

enum {
  /* no operand				
   */
  ASM_OTYPE_NONE,	
  /* operand is fixed in instruction coded by this opcode;
   */
  ASM_OTYPE_FIXED,	
  /* register is coded in mod field of instruction opcode.
   */
  ASM_OTYPE_OPMOD,	
  /* direct address; no mod R/M byte; 
   * address of operand is encoded in instruction;
   * no base register, index register, or scaling factor can be applied
   */
  ASM_OTYPE_ADDRESS,	
  /* reg field of mod R/M byte selects a control register
   */
  ASM_OTYPE_CONTROL,	
  /* reg field of mod R/M byte selects a debug register
   */
  ASM_OTYPE_DEBUG,	
  /* mod R/M byte follows opcode and specifies operand; 
   * operand is either a general register or a memory address;
   * if it is a memory address, the address is computed from 
   * a segment register and any of the following values:
   * a base register, an index register, a scaling factor, a displacement
   */
  ASM_OTYPE_ENCODED,	
  /* flags registers
   */
  ASM_OTYPE_FLAGS,	
  /* reg field of mod R/M byte selelcts a general register. 
   */
  ASM_OTYPE_GENERAL,	
  /* immediate data; value of operand is encoded in subsequent bytes 
   * of instruction
   */
  ASM_OTYPE_IMMEDIATE,	
  /* instruction contains a relative offset to be added 
   * to the instruction pointer register
   */
  ASM_OTYPE_JUMP,	
  /* mod R/M only refer to memory		
   */
  ASM_OTYPE_MEMORY,	
  /* instruction has no mod R/M byte;                                                                                                            |   
   * offset of operand is coded as a word or double word (depending on 
   * address size attribute) in instruction;                                   |   
   * no base register, index register, or scaling factor can be applied; 
   * eg. MOV (A0..A3h)                          
   */
  ASM_OTYPE_OFFSET,	
  /* reg field of mod R/M byte selects a packed quadword MMX register
   */
  ASM_OTYPE_PMMX,	
  /* mod R/M byte follows opcode and specifies operand; operand is 
   * either an MMX register or a memory address;
   * if it is a memory address, the address is computed 
   * from a segment register and any of the following values:
   * |a base register, an index register, a scaling factor, a displacement
   */
  ASM_OTYPE_QMMX,	
  /*
   * mod field of mod R/M byte may refer only to a general register
   */
  ASM_OTYPE_REGISTER,
  /* reg field of mod R/M byte selects a segment register
   */
  ASM_OTYPE_SEGMENT,	
  /* no operand				
   */
  ASM_OTYPE_TEST,	
  /* no operand				
   */
  ASM_OTYPE_VSFP,	
  /* no operand				
   */
  ASM_OTYPE_WSFP,	
  /* memory addressed by ds:si		
   */
  ASM_OTYPE_XSRC,	
  /* memory addressed by es:di		
   */
  ASM_OTYPE_YDEST,
  /* immediate value encoded in instruction
   */
  ASM_OTYPE_VALUE,
  ASM_OTYPE_REG0,
  ASM_OTYPE_REG1,
  ASM_OTYPE_REG2,
  ASM_OTYPE_REG3,
  ASM_OTYPE_REG4,
  ASM_OTYPE_REG5,
  ASM_OTYPE_REG6,
  ASM_OTYPE_REG7,
  
  ASM_OTYPE_ST,
  ASM_OTYPE_ST_0,
  ASM_OTYPE_ST_1,
  ASM_OTYPE_ST_2,
  ASM_OTYPE_ST_3,
  ASM_OTYPE_ST_4,
  ASM_OTYPE_ST_5,
  ASM_OTYPE_ST_6,
  ASM_OTYPE_ST_7
} e_asm_operand_type;

enum {
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
} e_asm_operand_size;


/*
  prototypes.
*/


/**
 * asm_init_arch init an asm_processor structure to handle
 * a specific architecture.
 * i386 support only.
 * 
 */

void	asm_init_i386(asm_processor *);
void	asm_init_sparc(asm_processor *);

int	asm_read_instr(asm_instr *, u_char *, u_int, asm_processor *);

int	asm_instr_len(asm_instr *);
char	*asm_instr_get_memonic(asm_instr *, asm_processor *);
char	*asm_display_instr_att(asm_instr *, int);

void	asm_clear_instr(asm_instr *);

void	asm_set_error(asm_instr *, int);
void	asm_set_errormsg(asm_instr *, char *);
void    asm_resolve_immediate(void *, u_int, char *, u_int);

/**
 *
 *
 **/

int		asm_instruction_get_len(asm_instr *, int, void *);

/**
 * operand field access methods.
 * use this to keep compatibility.
 **/

int		asm_operand_get_count(asm_instr *, int, int, void *);
int		asm_operand_get_content(asm_instr *, int, int, void *);
int		asm_operand_get_type(asm_instr *, int, int, void *);
int		asm_operand_get_size(asm_instr *, int, int, void *);
int		asm_operand_get_len(asm_instr *, int, int, void *);
int		asm_operand_set_value(asm_instr *, int, int, void *);
int		asm_operand_get_value(asm_instr *, int, int, void *);

/**
 * immediate field may be a 1,2,4,8 ... byte long value
 * to set it,
 */

int		asm_operand_debug(asm_instr *, int, int, void *);
int		asm_operand_get_immediate(asm_instr *, int, int, void *);
int		asm_operand_set_immediate(asm_instr *, int, int, void *);
int		asm_operand_get_basereg(asm_instr *, int, int, void *);
int		asm_operand_set_basereg(asm_instr *, int, int, void *);
int		asm_operand_get_indexreg(asm_instr *, int, int, void *);
int		asm_operand_set_indexreg(asm_instr *, int, int, void *);
int		asm_operand_get_scale(asm_instr *, int num, int opt, void *);
int		asm_operand_set_scale(asm_instr *, int num, int opt, void *);

/**
 * used to init a resolving handler for output
 */


void		asm_set_resolve_handler(asm_processor *,
					void (*)(void *, u_int, char *, u_int), 
					void *);



/**
 *  asm
 *
 *
 *
 */

struct s_asm_processor {
  /*	handler to resolving function					*/
  void                  (*resolve_immediate)(void *, u_int, char *, u_int);
  /*	handler data pointer						*/
  void                  *resolve_data;
  
  /*	processor type	. I386 supports only				*/
  int                   type;
  
  /*	array to instruction memonic by instr field of asm_instr	*/
  char                  **instr_table;
  
  /*	fetching instruction. points to processor fetching function.	*/
  int			(*fetch)(asm_instr *, u_char *, u_int, 
				 asm_processor *);
  
  /*	pointer to an internal structure.	*/
  void                  *internals;
};

enum {
  LIBASM_ERROR_SUCCESS,
#define LIBASM_MSG_SUCCESS		"success"
  LIBASM_ERROR_NSUCH_CONTENT
#define LIBASM_MSG_NSUCH_CONTENT	"no such content"
} e_libasm_errorcode;

#include <libasm-i386.h>

#endif




