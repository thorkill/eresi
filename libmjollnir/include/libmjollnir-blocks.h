/*
 * 2001-2006 Devhell Labs
 * 2006 Asgard Labs, thorolf
 * 
 * Basic blocks related structures for libmjollnir
 *
 * $Id: libmjollnir-blocks.h,v 1.11 2007-01-02 06:21:23 may Exp $
 *
 */
#if !defined(__MJR_BLOCKS__)
 #define __MJR_BLOCKS__ 1

#include "libelfsh.h"

/*
 * struct s_caller is to reference blocks calling block
 * field type may help us to analyse blocks to build higher
 * logical structure. It contains :
 * - a type field to know how the block is invoked
 * - a vaddr field to know address of instruction which has invoked it.
 */
typedef	struct		s_caller 
{
#define   CALLER_CONT   0	/* block is invoked from previous block	*/
#define   CALLER_JUMP	1	/* block is invoked by some JUMP,JCC	*/
#define   CALLER_CALL	2	/* block is invoked by a call		*/
#define	  CALLER_RET	3	/* block is ended by a RET		*/
#define   CALLER_UNKN	4	/* block was not analysed		*/
  int			type;
  elfsh_Addr		vaddr;
  struct s_caller	*next;
}			mjrcaller_t;


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
}			mjrblock_t;



/***** Misplaced structures : should find another header */



/* Control flow dump options */
typedef struct	s_disopt
{
  elfshobj_t	*file;
  u_int		counter;
  int		level;
}		mjropt_t;

/* ELFsh buffer */
typedef struct	s_buf
{
  char		*data;
  u_int		maxlen;
  u_int		allocated;
  u_int		block_counter;
  elfshobj_t	*obj;
}		mjrbuf_t;


/***** That structure is not used yet ******/



/* Abstract representation for a condition */
typedef struct	s_condition 
{
#define CONDITION_SIGNED
#define CONDITION_UNSIGNED
  int		sign;
  char		*dst;
  char		*src;
  char		*cmp;
}		mjrcond_t;


#endif
