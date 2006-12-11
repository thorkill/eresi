/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: libmjollnir-blocks.h,v 1.5 2006-12-11 13:57:34 may Exp $
 *
 */
#include <stdio.h>
#include <libelfsh.h>
#include <libasm.h>
#include <sys/types.h>

/* Structure s_list used for arguments enumeration */

#define EXEC_MISSED	0
#define EXEC_PASSED	1

struct s_arglist 
{
  int   num;		/* argument number		*/
  int	used;		/* number of access to argument	*/
  struct s_arglist	*next;
};

struct s_disopt
{
  elfshobj_t	*file;
  u_int		counter;
  int		level;
};



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

//asm_instr		vaddr_hist[5];

/* this covers at this moment only calls */

#define MJR_TYPE_SECT_START 0
#define MJR_TYPE_FUNCT		1

/* Original block format for libmjollnir */
typedef struct		_mjrBlock 
{
 u_int			vaddr;
 u_int			type;
 char			*name;
 u_int			flowTo;
 u_int			flowRet;
 char			*section;
 struct mjrBlock	*listNext;
 struct mjrBlock	*listPrev;
}			mjrblock_t;


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


/* Structure used to describe blocks */
typedef struct		s_iblock 
{
  char			*symstr;
  u_int			vaddr;	/* block starting virtual address	*/
  u_int			size;	/* block size				*/
  struct s_caller	*caller;/* linked list of invvoking instr.      */
  u_int			altern;	/* vaddr of alternate path		*/
  u_int			altype; /* alternative path invokation type	*/
  u_int			contig; /* vaddr of next block if continuing	*/
  struct s_iblock	*next;
  btree_t		*btree;
  int			passed;
}			elfshiblock_t;

/* Concrete block representation as it is stored in ".control" section */
typedef struct	s_elfshblock 
{
  u_int		vaddr;		/* virtual address of block  */
  u_int		size;		/* size of block	     */
  u_int		contig;		/* contiguous execution path */
  u_int		altern;		/* alternate execution path  */
  u_int		altype;		/* execution breaking instr  */
}		elfshblk_t;

/* Abstract block reference representation */
typedef struct s_elfshblref 
{
  u_int		vaddr;		/* address of instruction invoking block	*/
  u_int		type;		/* type of invokation				*/
}		elfshblkref_t;

/* Abstract function representation */
typedef struct		s_function 
{
  u_int			vaddr;
  u_int			size;
  char			*symstr;
  struct s_caller	*blocks;
  struct s_function	*next;
}			elfshfunc_t;

/* Some settings */
/*
struct s_elfshstng 
{
  int	rec_ptr_fnc; 		** automagic ptr-function recognition **
  int  use_goto_hash; 	        ** use s_goto hash/addgoto function ** 
  int  graph_verbose_level;
};
*/
