/**
 * @file libmjollnir-objects.h
 *
 * 2001-2007 Devhell Labs, Asgardlabs
 * 
 * @brief All structures of libmjollnir
 *
 * $Id: libmjollnir-objects.h,v 1.2 2007-08-03 11:50:59 heroine Exp $
 *
 */
#if !defined(__MJR_BLOCKS__)
 #define __MJR_BLOCKS__ 1

#include "libelfsh.h"

#define MJR_BLOCK_GET_STRICT 0
#define MJR_BLOCK_GET_FUZZY  1

/* XXX: Change size in libaspect/types.c if this struct is changed */
/* XXX: Always let the vaddr and size field in first for the next 2 structures !!!! */

/**
 * @brief Structure used to describe blocks in memory 
 */
typedef struct	s_iblock 
{
  elfsh_Addr	vaddr;		/* !< @brief block starting virtual address    */
  u_int		size;		/* !< @brief block code size                   */
  u_int		symoff;		/* !< @brief block name offset in string table */
}		mjrblock_t;

/** 
 * @brief Structure used to described functions in memory 
 */
typedef struct	s_function 
{
  elfsh_Addr	vaddr;		/* !< @brief Function virtual address */
  u_int		size;		/* !< @brief Function size */
  char		name[64];	/* !< @brief Function name */
  mjrblock_t	*first;		/* !< @brief First function block */
  char		md5[34];	/* !< @brief MD5 Checksum */
}		mjrfunc_t;
 
/** 
 * @brief An history entry 
 */
typedef struct		s_history
{
  elfsh_Addr  		vaddr;
  asm_instr		instr;
}			mjrhistory_t;

/**
 * @brief The context of a single session 
*/
typedef struct		_mjrContext 
{
  elfshobj_t		*obj;			/* !< @brief ELF binary object */
  asm_processor		proc;			/* !< @brief ASM processor */
  mjrcontainer_t	*curblock;		/* !< @brief Current working block */
  mjrcontainer_t	*curfunc;		/* !< @brief Current working function */
  mjrcontainer_t	**reg_containers;	/* !< @brief Registered containers */
  btree_t		*block_btree;		/* !< @brief Blocks Binary tree (speedup parent search) */
  u_int			cntnrs_size;		/* !< @brief Size of current containers */
  u_int			next_id;		/* !< @brief Next free container id */

#define			MJR_HISTORY_LEN		5
#define			MJR_HISTORY_PPREV	(MJR_HISTORY_LEN - 3)
#define			MJR_HISTORY_PREV	(MJR_HISTORY_LEN - 2)
#define			MJR_HISTORY_CUR		(MJR_HISTORY_LEN - 1)
  mjrhistory_t		hist[MJR_HISTORY_LEN];  /* !< @brief History of instructions */

  hash_t		funchash;		/* !< @brief functions hash table */
  hash_t		blkhash;		/* !< @brief blocks hash table for this obj */
  unsigned char		analysed;		/* !< @brief do we analysed it */
  u_int			calls_seen;		/* !< @brief how many CALL we have seen */
  u_int			calls_found;		/* !< @brief how many dest has beed resolved */
}			mjrcontext_t;


/** 
 * @brief The session structure. Yes, libmjollnir is multisession 
 */
typedef struct	s_session
{
  mjrcontext_t	*cur;
  hash_t	ctx;
}		mjrsession_t;


/**
 * @brief Control flow dump options 
 */
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
