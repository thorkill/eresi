/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-blocks.h,v 1.9 2006-12-31 05:07:12 may Exp $
 *
 */
#include <libelfsh.h>
#include <libasm.h>

/* Control flow dump options */
typedef struct	s_disopt
{
  elfshobj_t	*file;
  u_int		counter;
  int		level;
}		elfshopt_t;

/* ELFsh buffer */
typedef struct	s_buf
{
  char		*data;
  u_int		maxlen;
  u_int		allocated;
  u_int		block_counter;
  elfshobj_t	*obj;
}		elfshbuf_t;

/* Abstract representation for a condition */
typedef struct	s_condition 
{
#define CONDITION_SIGNED
#define CONDITION_UNSIGNED
  int		sign;
  char		*dst;
  char		*src;
  char		*cmp;
}		elfshcond_t;


/*
 * struct s_caller is to reference blocks calling block
 * field type may help us to analyse blocks to build higher
 * logical structure. It contains :
 * - a type field to know how the block is invoked
 * - a vaddr field to know address of instruction which has invoked it.
 */
typedef			struct s_caller 
{
#define   CALLER_CONT   0	/* block is invoked from previous block	*/
#define   CALLER_JUMP	1	/* block is invoked by some JUMP,JCC	*/
#define   CALLER_CALL	2	/* block is invoked by a call		*/
#define	  CALLER_RET	3	/* block is ended by a RET		*/
#define   CALLER_UNKN	4	/* block was not analysed		*/
  int			type;
  u_int			vaddr;
  struct s_caller	*next;
}			elfshcaller_t;


/* Structure used to describe blocks in memory */
/* This format is never saved ondisk */
typedef struct		s_iblock 
{
  unsigned int		symoff;	 /* block name offset in string table   */
  elfsh_Addr		vaddr;	 /* block starting virtual address	*/
  elfsh_Addr		true;    /* vaddr of next block if continuing	*/
  elfsh_Addr		false;	 /* vaddr of alternate path		*/
  u_int			type;	 /* Invokation type			*/
  u_int			size;	 /* block size				*/
  struct s_caller	*caller; /* MEMONLY : linked list of callers    */
}			elfshiblock_t;



/* Abstract function representation */
typedef struct		s_function 
{
  u_int			vaddr;
  u_int			size;
  char			*symstr;
  struct s_caller	*blocks;
  struct s_function	*next;
}			elfshfunc_t;
