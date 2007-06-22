/*
** (C) Devhell Labs / Asgard Labs 2001-2007
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
#include "libasm.h"
#include "libmjollnir-blocks.h"
#include "libmjollnir-container.h"
#include "libmjollnir-int.h"
#include "libmjollnir-fprint.h"
 
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
typedef struct		s_session
{
 mjrcontext_t		*cur;
 hash_t			ctx;
}			mjrsession_t;


/* Size of temp buffers */
#define BSIZE_SMALL	32
#define BSIZE		4096 

/* Names used in config hash */
#define MJR_COFING_CALL_PREFIX "mjr.callsprefix"

/* Default subroutines prefix in symbol recovery */
#define	MJR_CALL_PREFIX	"sub_"

/* OS for which we are able to match the entry point */
#define	MJR_BIN_LINUX	0
#define	MJR_BIN_FREEBSD	1

/* The hash declared in VM for gotos */
extern hash_t	goto_hash;

/* init.c */
int		mjr_init_session(mjrsession_t *);
int		mjr_create_context_as_current(mjrsession_t *, elfshobj_t *);
mjrcontext_t	*mjr_create_context(elfshobj_t *);
int		mjr_setup_processor(mjrsession_t *);

/* control.c	*/
elfsh_Addr	mjr_find_main(elfshobj_t	*obj,
			      asm_processor	*proc,
			      u_char		*buf,
			      u_int		len,
			      elfsh_Addr	vaddr,
			      u_int		*dis);
elfsh_Addr	mjr_trace_start(mjrcontext_t *c, u_char *, u_int, elfsh_Addr);
int		mjr_trace_control(mjrcontext_t *c, elfshobj_t *, 
				  asm_instr *, elfsh_Addr a);

/* core.c */
int		mjr_analyse(mjrsession_t *sess, int flags);
int		mjr_analyse_section(mjrsession_t *s, char *sectname);

/* blocks.c */
int		mjr_blocks_get(mjrcontext_t *ctxt);
int		mjr_blocks_load(mjrcontext_t *c);
mjrcontainer_t* mjr_block_get_by_vaddr(mjrcontext_t *ctxt, elfsh_Addr, int);
int		mjr_blocks_store(mjrcontext_t *c);
u_int		mjr_block_flow_save(mjrcontainer_t *c, u_int type, mjrbuf_t *buf);
int		mjr_block_point(mjrcontext_t*, asm_instr*, elfsh_Addr, elfsh_Addr);
char 		*_vaddr2str(elfsh_Addr);
int		mjr_block_relink_cond_always(mjrcontainer_t *, mjrcontainer_t *, int);
int		mjr_blocks_link_call(mjrcontext_t *, elfsh_Addr, elfsh_Addr, elfsh_Addr);
int		mjr_blocks_link_jmp(mjrcontext_t *, elfsh_Addr, elfsh_Addr, elfsh_Addr);

mjrcontainer_t	*mjr_split_block(mjrcontext_t *,elfsh_Addr, u_int);
int		mjr_block_dump(mjrcontext_t*, mjrcontainer_t *);

/* fingerprint.c */
int		mjr_block_funcstart(mjrcontainer_t *cntnr);
int		mjr_fprint(mjrcontext_t 	*c,
			   mjrcontainer_t   	*start, 
			   int			type,
			   int   		weight,
			   int	    		curd,
			   int	    		mind,
			   int	    		maxd,
			   int  		(*fprint)(mjrcontainer_t *));

/* display.c */
//void		mjr_block_dump(mjrblock_t *b);
int		mjr_blocks_display(mjrcontext_t *c, int);
int		mjr_block_display(mjrcontext_t *, mjrcontainer_t *c, mjropt_t *opt);
void		mjr_function_display(mjrfunc_t *func);
void		mjr_funcs_display(mjrcontext_t *c);

/* types.c */
int		mjr_asm_flow(mjrcontext_t *c);
elfsh_Addr	mjr_compute_fctptr(mjrcontext_t	*context);
int		mjr_get_jmp_destaddr(mjrcontext_t *context);
int		mjr_get_call_destaddr(mjrcontext_t *context);
int		mjr_asm_check_function_start(mjrcontext_t *ctxt);

/* symtab.c */
int		mjr_symtab_rebuild(mjrsession_t *);
int		mjr_symbol_add(mjrsession_t *, elfsh_Addr, char *);
int		mjr_symbol_delete_by_name(mjrsession_t *, char *);
int		mjr_symbol_rename(mjrsession_t *,char *,char *);

/* function.c */
void		*mjr_fingerprint_function(mjrcontext_t *, elfsh_Addr addr, int);
void		mjr_function_dump(mjrcontext_t*, char *,mjrcontainer_t *);
u_int		mjr_function_flow_save(mjrcontainer_t *, u_int, mjrbuf_t *);
int		mjr_functions_load(mjrcontext_t *);
int		mjr_functions_get(mjrcontext_t *);
int		mjr_functions_store(mjrcontext_t *);
int		mjr_functions_link_call(mjrcontext_t *ctxt, 
					elfsh_Addr src, 
					elfsh_Addr dst, 
					elfsh_Addr ret);
int		mjr_function_register(mjrcontext_t *, 
				      u_int,
				      mjrcontainer_t *);
mjrcontainer_t *mjr_function_get_by_vaddr(mjrcontext_t *, 
					  u_int);

/* history.c */
void		mjr_history_shift(mjrcontext_t *cur, asm_instr i, elfsh_Addr a);
void		mjr_history_write(mjrcontext_t*, asm_instr*, elfsh_Addr a, int i);

/* container.c */
void		mjr_init_containers(mjrcontext_t*);
void		mjr_resize_containers(mjrcontext_t*);
unsigned int	mjr_register_container (mjrcontext_t*, mjrcontainer_t *cntnr);
unsigned int	mjr_register_container_id (mjrcontext_t*, mjrcontainer_t *cntnr);
void		mjr_unregister_container (mjrcontext_t*, u_int id);
mjrcontainer_t *mjr_lookup_container (mjrcontext_t*,u_int id);
mjrlink_t	*mjr_container_add_link(mjrcontext_t *,
					mjrcontainer_t *cntnr, 
					u_int id, 
					int link_type, 
					int link_direction);

mjrlink_t	*mjr_get_link_of_type(mjrlink_t *link, int link_type);

mjrcontainer_t	*mjr_create_block_container(mjrcontext_t*,
					    u_int 	symoff,
					    elfsh_Addr	vaddr,
					    u_int	size);

mjrcontainer_t	*mjr_create_function_container(mjrcontext_t*,
					       elfsh_Addr	vaddr,
					       u_int		size,
					       char		*name,
					       mjrblock_t	*first,
					       char		*md5);

mjrcontainer_t	*mjr_get_container_by_vaddr(mjrcontext_t*,elfsh_Addr vaddr,int type);
int		mjr_container_link_cleanup(mjrcontainer_t *c,int direction);
mjrlink_t	*mjr_link_get_by_direction(mjrcontainer_t *c,int dir);

#endif
