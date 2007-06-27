/**
 * @file libmjollnir-blocks.h
 * 2001-2007 Devhell Labs
 * 2006-2007 Asgard Labs, thorolf
 * 
 * Basic blocks related structures for libmjollnir
 *
 * $Id: libmjollnir-blocks.h,v 1.20 2007-06-27 11:25:12 heroine Exp $
 *
 */
#if !defined(__MJR_BLOCKS__)
 #define __MJR_BLOCKS__ 1

#include "libelfsh.h"

#define MJR_BLOCK_GET_STRICT 0
#define MJR_BLOCK_GET_FUZZY  1

/* Structure used to describe blocks in memory */
/* This format is never saved ondisk */
typedef struct		s_iblock 
{
  u_int			symoff;	 /* block name offset in string table   */
  elfsh_Addr		vaddr;	 /* block starting virtual address	*/
  u_int			size;
}			mjrblock_t;


/***** Misplaced structures : should find another header */



/* Control flow dump options */
typedef struct	s_disopt
{
  elfshobj_t	*file;
  u_int				counter;
  int					level;
}		mjropt_t;

/* ELFsh buffer */
typedef struct	s_buf
{
  char				*data;
  u_int				maxlen;
  u_int				allocated;
  u_int				block_counter;
  elfshobj_t	*obj;
}		mjrbuf_t;


/***** That structure is not used yet ******/



/* Abstract representation for a condition */
typedef struct	s_condition 
{
#define CONDITION_SIGNED
#define CONDITION_UNSIGNED
  int			sign;
  char		*dst;
  char		*src;
  char		*cmp;
}		mjrcond_t;


#endif
