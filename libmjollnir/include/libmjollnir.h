/*
** (C) Devhell Labs / Asgard Labs 2001-2006
**
** $id: Libmjollnir.H,V 1.8 2006/07/27 16:50:48 thor Exp $
**
** December 2006 : Merged the modflow API and the mjollnir API to make a unified
** component. Adapted the data structures. Unified prefixing. -may
**
**/
#if !defined(__MJOLLNIR__)
  #define __MJOLLNIR__ 1

#include <stddef.h>
#include <assert.h>
#include <openssl/md5.h>
#include "libelfsh.h"
#include "libmjollnir-btree.h"
#include "libmjollnir-blocks.h"
#include "libmjollnir-int.h"
#include "libmjollnir-fprint.h"


/* An history entry */
typedef struct		s_history
{
  elfsh_Addr		vaddr;
  asm_instr		instr;
}			mjrhistory_t;

/* The context of a single session */
typedef struct		_mjrContext 
{
  elfshobj_t		*obj;        /* elfsh object */
  asm_processor		proc;  	     /* proc */
  elfshiblock_t	        *curblock;   /* current working block */

#define			MJR_HISTORY_LEN		5
#define			MJR_HISTORY_PPREV	(MJR_HISTORY_LEN - 3)
#define			MJR_HISTORY_PREV	(MJR_HISTORY_LEN - 2)
#define			MJR_HISTORY_CUR		(MJR_HISTORY_LEN - 1)
  mjrhistory_t		hist[MJR_HISTORY_LEN];     /* History of instructions */

  hash_t		blkhash;     /* blocks hash table for this obj */
  elfshiblock_t		*blklist;    /* blocks array with on-disk format */
  unsigned char		analysed;    /* do we analysed it */
  u_int			calls_seen;  /* FIXME */
  u_int			calls_found; /* FIXME */
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

/* init.c */
int		mjr_init_session(mjrsession_t *);
int		mjr_create_context_as_current(mjrsession_t *, elfshobj_t *);
mjrcontext_t	*mjr_create_context(elfshobj_t *);
int		mjr_setup_processor(mjrsession_t *);

/* internal.c */
char		*_vaddr2string(u_int);

/* control.c	*/
u_int		mjr_trace_start(mjrcontext_t *c, elfshobj_t *, unsigned char *, 
				u_int, u_int, elfshiblock_t **);
int		mjr_trace_control(mjrcontext_t *c, elfshobj_t *, 
				  asm_instr *, elfsh_Addr a);

/* core.c */
int		mjr_analyse(mjrsession_t *sess, int flags);
int		mjr_analyse_section(mjrsession_t *s, char *sectname);
elfsh_Addr	mjr_compute_fctptr(mjrcontext_t	*context);
int		mjr_insert_destaddr(mjrcontext_t *c);
int		mjr_block_point(elfshiblock_t **blklist, asm_instr *ins,
				elfsh_Addr vaddr, elfsh_Addr dest);
elfshiblock_t*	mjr_get_blocks(elfshobj_t *file);

/* blocks.c */
elfshiblock_t	*mjr_block_create(u_int, u_int);
int		mjr_display_blocks(elfshobj_t *, elfshiblock_t *, int);
void		mjr_dump_block(elfshiblock_t *b);
int		mjr_free_blocks(elfshiblock_t *blks);
int		mjr_load_blocks(elfshobj_t *, elfshiblock_t **);
int		mjr_store_blocks(elfshobj_t *, elfshiblock_t *, int);
elfshiblock_t  *mjr_block_get_by_vaddr(elfshiblock_t *, u_int, int);
void		mjr_block_clean_passed(elfshiblock_t *);
void		mjr_block_add_caller(elfshiblock_t *, u_int, int);
void		mjr_block_add_list(elfshiblock_t **, elfshiblock_t *);
int		mjr_block_is_funcstart(elfshiblock_t *);

/* typed.c */
int            mjr_asm_flow(mjrcontext_t *c);


/* symtab.c */
int		mjr_symtab_rebuild(mjrsession_t *);
int		mjr_symbol_add(mjrsession_t *, char *, u_int, char *);
int		mjr_symbol_delete_by_name(mjrsession_t *, char *);
int		mjr_symbol_rename(mjrsession_t *,char *,char *);

/* function.c */
mjrfunction_t   *mjr_function_create(u_int);
void		*mjr_fingerprint_function(mjrcontext_t *, elfsh_Addr addr, int);

/* history.c */
int		mjr_history_update(mjrcontext_t *cur, asm_instr i);
void		mjr_history_shift(mjrcontext_t *cur, asm_instr i, elfsh_Addr a);
void		mjr_history_write(mjrcontext_t *c, asm_instr*, elfsh_Addr a, int i);
#endif
