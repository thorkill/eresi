/**
 * @file libmjollnir-objects.h
 *
 * 2001-2007 Devhell Labs, Asgardlabs
 * 
 * All structures of libmjollnir
 *
 * $Id: libmjollnir-objects.h,v 1.1 2007-07-31 03:28:47 may Exp $
 *
 */
#if !defined(__MJR_BLOCKS__)
 #define __MJR_BLOCKS__ 1

#include "libelfsh.h"

#define MJR_BLOCK_GET_STRICT 0
#define MJR_BLOCK_GET_FUZZY  1

/* XXX: Change size in libaspect/types.c if this struct is changed */
/* XXX: Always let the vaddr and size field in first for the next 2 structures !!!! */

/* Structure used to describe blocks in memory */
typedef struct	s_iblock 
{
  elfsh_Addr	vaddr;		/* block starting virtual address    */
  u_int		size;		/* block code size                   */
  u_int		symoff;		/* block name offset in string table */
}		mjrblock_t;

/* Structure used to described functions in memory */
typedef struct	s_function 
{
  elfsh_Addr	vaddr;		/* Function virtual address */
  u_int		size;		/* Function size */
  char		name[64];	/* Function name */
  mjrblock_t	*first;		/* First function block */
  char		md5[34];	/* MD5 Checksum */
}		mjrfunc_t;
 
/* An history entry */
typedef struct		s_history
{
  elfsh_Addr  		vaddr;
  asm_instr		instr;
}			mjrhistory_t;

/* The context of a single session */
typedef struct		_mjrContext 
{
  elfshobj_t		*obj;			/* ELF binary object */
  asm_processor		proc;			/* ASM processor */
  mjrcontainer_t	*curblock;		/* Current working block */
  mjrcontainer_t	*curfunc;		/* Current working function */
  mjrcontainer_t	**reg_containers;	/* Registered containers */
  btree_t		*block_btree;		/* Blocks Binary tree (speedup parent search) */
  u_int			cntnrs_size;		/* Size of current containers */
  u_int			next_id;		/* Next free container id */

#define			MJR_HISTORY_LEN		5
#define			MJR_HISTORY_PPREV	(MJR_HISTORY_LEN - 3)
#define			MJR_HISTORY_PREV	(MJR_HISTORY_LEN - 2)
#define			MJR_HISTORY_CUR		(MJR_HISTORY_LEN - 1)
  mjrhistory_t		hist[MJR_HISTORY_LEN];  /* History of instructions */

  hash_t		funchash;		/* functions hash table */
  hash_t		blkhash;		/* blocks hash table for this obj */
  unsigned char		analysed;		/* do we analysed it */
  u_int			calls_seen;		/* how many CALL we have seen */
  u_int			calls_found;		/* how many dest has beed resolved */
}			mjrcontext_t;


/* The session structure. Yes, libmjollnir is multisession */
typedef struct	s_session
{
  mjrcontext_t	*cur;
  hash_t	ctx;
}		mjrsession_t;


/* Control flow dump options */
typedef struct	s_disopt
{
  elfshobj_t	*file;
  u_int		counter;
  int		level;
}		mjropt_t;

/* Mjollnir buffer */
typedef struct	s_buf
{
  char		*data;
  u_int		maxlen;
  u_int		allocated;
  u_int		counter;
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
