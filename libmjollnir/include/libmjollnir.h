/*
** Authors : 
** (C) 2006 Asgard Labs, thorolf
**     2001-2006 Devhell Labs : sk, may
**
** Bsd License
**
** $id: Libmjollnir.H,V 1.8 2006/07/27 16:50:48 thor Exp $
**
** December 2006 : Merged the modflow API and the mjollnir API to make a unified
** component. Adapted the data structures. Unified prefixing. -may
**
**/
#include <stddef.h>
#include <assert.h>
#include <openssl/md5.h>
#include "libelfsh.h"
#include "libmjollnir-btree.h"
#include "libmjollnir-blocks.h"
#include "libmjollnir-int.h"
#include "libmjollnir-fprint.h"


/* The context of a single session */
typedef struct		_mjrContext 
{
  elfshobj_t		*obj;     	/* elfsh object */
  asm_processor		proc;  		/* proc */
  u_int			curVaddr;      	/* currently analysed vaddr */
  hash_t		blocks;	  	/* blocks */
  asm_instr		ihist[4]; 	/* instruction history */
  unsigned char		analysed_calls; /* do we analysed it */
  u_int			st_calls_seen;
  u_int			st_calls_found;

  /* Original modflow fields - to be merged ! */
  hash_t		block_hash;
  asm_instr		vaddr_hist[5];
}			mjrcontext_t;


/* The session structure. Yes, libmjollnir is multisession */
typedef struct		s_session
{
 mjrcontext_t		*cur;
 hash_t			ctx;
}			mjrsession_t;

/* Size of temp buffers */
#define BSIZE		4096 

/* Names used in config hash */
#define MJR_COFING_CALL_PREFIX "mjr.callsprefix"

/* Default subroutines prefix in symbol recovery */
#define MJR_CALL_PREFIX	"sub_"

/* OS for which we are able to match the entry point */
#define			MJR_BIN_LINUX	0
#define			MJR_BIN_FREEBSD	1



/* The hash declared in VM for gotos */
extern hash_t	goto_hash;

/* dstCall.c */
int		mjr_get_call_dst(mjrsession_t *,int *);

/* function.c */
mjrfunction_t   *mjr_function_create(u_int);
void		*mjr_fingerprint_function(mjrsession_t *, unsigned char *, int );

/* history.c */
int		mjr_history_update(mjrsession_t *, asm_instr);

/* init.c */
int		mjr_init_session(mjrsession_t *);
int		mjr_create_context_as_current(mjrsession_t *, elfshobj_t *);
mjrcontext_t	*mjr_create_context(elfshobj_t *);
int		mjr_setup_processor(mjrsession_t *);

/* internal.c */
char		*_vaddr2string(u_int);

/* symtab.c */
int		mjr_symtab_rebuild(mjrsession_t *);
int		mjr_symbol_add(mjrsession_t *, char *, u_int, char *);
int		mjr_symbol_delete_by_name(mjrsession_t *, char *);
int		mjr_symbol_rename(mjrsession_t *,char *,char *);


/* ----- MODFLOW API --------- */

/* control.c	*/
u_int		mjr_trace_start(mjrcontext_t *c, elfshobj_t *, char *, 
				u_int, u_int, elfshiblock_t **);

/* blocks.c */
int		mjr_display_blocks(elfshobj_t *, elfshiblock_t *, int);
void		mjr_dump_block(elfshiblock_t *b);
void            mjr_history_shift(mjrcontext_t *cur, asm_instr vaddr);
int		mjr_free_blocks(elfshiblock_t *blks);
int		mjr_load_blocks(elfshobj_t *, elfshiblock_t **);
int		mjr_store_blocks(elfshobj_t *, elfshiblock_t *, int);
elfshiblock_t  *mjr_block_get_by_vaddr(elfshiblock_t *, u_int, int);
void		mjr_block_clean_passed(elfshiblock_t *);
void		mjr_block_add_caller(elfshiblock_t *, u_int, int);
void		mjr_block_add_list(elfshiblock_t **, elfshiblock_t *);
int		mjr_block_is_funcstart(elfshiblock_t *);
int		mjr_insert_destaddr(mjrcontext_t *c, elfshobj_t *, 
				    asm_instr *, u_int, elfshiblock_t **);
int		mjr_block_point(elfshiblock_t **blklist, asm_instr *ins,
				elfsh_Addr vaddr, elfsh_Addr dest);

/* MERGE !!! core.c / control.c */
int		mjr_find_calls(mjrsession_t *,char *);				
void		mjr_trace_control(mjrcontext_t *c, elfshobj_t *, 
				  asm_instr *, u_int, elfshiblock_t **);
// Use this function in cmd_flow ! !!!
int		mjr_analyse(mjrsession_t *,int);

// MERGE !!!
elfshiblock_t	*mjr_block_create(u_int, u_int);
mjrblock_t	*mjr_create_block(u_int,char *,u_int);

				    


/*	sk_functions.c	*/
/*
struct s_function	*function_get_by_vaddr(struct s_function *, u_int);
u_int	function_get_parent(elfshiblock_t *, elfshiblock_t *, int);
void	display_functions(elfshobj_t *, struct s_function *);
void	trace_functions(elfshobj_t *, struct s_function **, elfshiblock_t *);
void	functions_find(struct s_function **, elfshiblock_t *);
void	function_add_list(struct s_function **, struct s_function *);
struct s_function	*function_create(u_int, char *);
*/

