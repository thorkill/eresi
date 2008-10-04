/**
 * @file libmjollnir-objects.h
 *
 * 2001-2007 Devhell Labs, Asgardlabs
 * 
 * @brief All structures of libmjollnir
 *
 * $Id: libmjollnir-objects.h,v 1.4 2008-02-16 12:32:27 thor Exp $
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
  eresi_Addr	vaddr;		/* !< @brief block starting virtual address    */
  unsigned int		size;		/* !< @brief block code size                   */
  unsigned int		symoff;		/* !< @brief block name offset in string table */
  unsigned char	seen;		/* !< @brief block live status (0 or 1)        */
}		mjrblock_t;

/** 
 * @brief Structure used to described functions in memory 
 */
typedef struct	s_function 
{
  eresi_Addr	vaddr;		/* !< @brief Function virtual address */
  unsigned int		size;		/* !< @brief Function size */
  char		name[64];	/* !< @brief Function name */
  mjrblock_t	*first;		/* !< @brief First function block */
  char		md5[34];	/* !< @brief MD5 Checksum */
}		mjrfunc_t;


/*
 * struct s_link is to reference links between functions or blocks
 * field type may help us to analyse blocks to build higher
 * logical structures. It contains :
 *
 * - a type field specifying which kind of link this is
 * - an id field to reference the destination container of this link
 * - a pointer to the next link in this list of links
 */

/* XXX: the type field has to be changed for a real eresi type id if 
   we hope to do any shape analysis in ERESI */
typedef	struct	s_link 
{
  unsigned int	id;
#define MJR_LINK_FUNC_CALL		0 /* !< @brief A call between functions	*/
#define MJR_LINK_FUNC_RET		1 /* !< @brief A returning control */
#define	MJR_LINK_FUNC_SLIDE		2 /* !< @brief A new stack frame happening in the middle */
#define MJR_LINK_BLOCK_COND_TRUE	3 /* !< @brief 'true' condition of a branch */
#define MJR_LINK_BLOCK_COND_FALSE	4 /* !< @brief 'false' condition of a branch */
#define MJR_LINK_BLOCK_COND_ALWAYS	5 /* !< @brief uncoditional branch */
#define MJR_LINK_TYPE_DELAY		6 /* !< @brief generally ignored but useful */
#define	MJR_LINK_UNKNOWN		7 /* !< @brief unknown type */
  unsigned char	type;
#define MJR_LINK_SCOPE_UNKNOWN		0 /* !< @brief Link type is unknown */
#define MJR_LINK_SCOPE_LOCAL	        1 /* !< @brief Link within the current parent object */
#define MJR_LINK_SCOPE_GLOBAL		2 /* !< @brief Link outside the current parent object */
  unsigned char	scope;
}		mjrlink_t;


 
/** 
 * @brief An history entry 
 */
typedef struct		s_history
{
  eresi_Addr  		vaddr;
  asm_instr		instr;
}			mjrhistory_t;

/**
 * @brief The context of a single session 
*/
typedef struct		_mjrContext 
{
  elfshobj_t		*obj;			/* !< @brief ELF binary object */
  asm_processor		proc;			/* !< @brief ASM processor */

  elfshsect_t		*cursct;		/* !< @brief Current analysed section */
  container_t		*curblock;		/* !< @brief Current working block */
  container_t		*curfunc;		/* !< @brief Current working function */
  list_t		*func_stack;		/* !< @brief Stack of analyzed functions */

  container_t		**reg_containers;	/* !< @brief Registered containers */
  btree_t		*block_btree;		/* !< @brief Blocks Binary tree (speedup parent search) */
  unsigned int			cntnrs_size;		/* !< @brief Size of current containers */
  unsigned int			next_id;		/* !< @brief Next free container id */

#define			MJR_HISTORY_LEN		5
#define			MJR_HISTORY_PPPREV	(MJR_HISTORY_LEN - 4)
#define			MJR_HISTORY_PPREV	(MJR_HISTORY_LEN - 3)
#define			MJR_HISTORY_PREV	(MJR_HISTORY_LEN - 2)
#define			MJR_HISTORY_CUR		(MJR_HISTORY_LEN - 1)
  mjrhistory_t		hist[MJR_HISTORY_LEN];  /* !< @brief History of instructions */

  hash_t		funchash;		/* !< @brief functions hash table */
  hash_t		blkhash;		/* !< @brief blocks hash table for this obj */
  hash_t		linkhash;		/* !< @brief links hash table */

  unsigned char		analysed;		/* !< @brief do we analysed it */
  unsigned int			calls_seen;		/* !< @brief how many CALL we have seen */
  unsigned int			calls_found;		/* !< @brief how many dest has beed resolved */
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
  unsigned int		counter;
  int		level;
}		mjropt_t;

/* Mjollnir buffer */
typedef struct	s_buf
{
  char		*data;
  unsigned int		maxlen;
  unsigned int		allocated;
  unsigned int		counter;
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
