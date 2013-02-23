/**
 * @file libasm-structs.h
 * 
 * Started on  Tue Jun 14 05:00:05 2005 Sebastien Roy
 * $Id$
 */

/**
 * Operand structure.
 */
struct s_asm_op 
{  

  /** operand length. (usefull on ia32 only).
   * operands expressed in R byte from ModRM byte have a null size.	*/
  u_int	len;			
  /* pointer to operand in buffer */
  u_char *ptr;
  /* operand type: ASM_OPTYPE_* enum */
  u_int	type;
  /* memory operand type: ASM_OP_* flags (for mem operands only) */
  u_int memtype;
  /* a pointer to the operand name in string format */
  char *name;
  /* contain operand size flags	*/
  u_int	size;
  /* Operand content flags */
  u_int	content;
  /* register set: 8/16/32 bits general registers, segment registers .. (IA32 only) */
  int regset;
  /* operand prefix (ia32 only)	*/
  int prefix;  
  /* immediate value extracted from operand	*/
  int imm; 
  /* base register: primary register extracted	*/
  int baser;
  /* index register: auxiliary register */
  int indexr;
  /* String for base register */
  char *sbaser;
  /* String for index register */
  char *sindex;
  /* address space (sparc only) 
   * 
   * has to be different than ASM_SP_ASI_P for ASM_SP_OTYPE_IMM_ADDRESS
   * operands on "alternate space" instructions so the output can be correct
   */
  int address_space;
  /* scale factor (ia32 only) */
  unsigned int scale;
};


/**
 * Instruction structure. 
 *
 * This contains all the fields related to an instruction: type, mnemonic,
 * operand types, contents.
 */

struct s_asm_instr 
{
  /* pointer to instruction buffer -- please let this field in first if using containers */
  u_char *ptr_instr;
  /* internal processor structure */
  asm_processor	*proc;
  /* instruction name */
  char *name;
  /* instruction id */
  int instr;
  /* instruction type */
  int type;
  /* instruction prefix (ia32 only) */
  int prefix;
  /* stack offset difference. push/pop/ret/call related */
  int	spdiff;
  /* bitfield of flags which could have been modified */
  int	flagswritten;
  /* bitfield of flags which were read */
  int	flagsread;
  /* Pointer to instruction prefix (ia32 only)	*/
  void *ptr_prefix;  
  /* annul bit (sparc only) */
  int annul;
  /* prediction bit (sparc only) */
  int prediction;    
  /* number of operand */
  int nb_op;
  /* Array of operands */
  asm_operand	op[4];
  /* instruction/operands full lengh */
  u_int len;   
};


/**
 * Processor structure.
 * Contains architecture dependant handler.
 *
 * This structure contains several functions pointers which are used
 * to as handlers to manage each architecture.
 *
 * NOTE: A vector should be implemented to replace thoses handlers
 * by provided functions. 
 */

struct s_asm_processor 
{
  /* handler to resolving function */
  void (*resolve_immediate)(void *, eresi_Addr, char *, u_int); 

  /* handler data pointer */
  void *resolve_data;

  /* processor type */
  int type;

  /* array to instruction memonic by instr field of asm_instr */
  char **instr_table;

  /* fetching instruction. points to processor fetching function. */
  LIBASM_HANDLER_FETCH(fetch);

  /* output handler. print instruction in a readable string	*/
  char *(*display_handle)(asm_instr *instr, eresi_Addr addr);

  /* pointer to an internal structure. */
  void *internals;

  /* Last operation error code */
  int	error_code;
};
