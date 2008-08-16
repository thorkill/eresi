/**
 * @file libmjollnir.h
 * 
 * (C) Devhell Labs / Asgard Labs 2001-2007
 *
 * $id: Libmjollnir.H,V 1.8 2006/07/27 16:50:48 thor Exp $
 *
 * December 2006 : Merged the modflow API and the mjollnir API to make a 
 * unified component. Adapted the data structures. Unified prefixing. -may
 *
 */

#if !defined(__MJOLLNIR__)
  #define __MJOLLNIR__ 1

#ifndef __KERNEL__
#include <stddef.h>
#include <assert.h>
#include <openssl/md5.h>
#endif

#include "libelfsh.h"
#include "libasm.h"
#include "libmjollnir-objects.h"
#include "libmjollnir-int.h"

/**
 * @brief Some defined  values for containers array 
 */
#define MJR_CNTNRS_INCREMENT 	200
#define MJR_MAX_INCREMENTS	200

/**
 * @brief Input and Output links 
 */
#define CONTAINER_LINK_IN	0
#define CONTAINER_LINK_OUT	1

/**
 * @brief Size of temp buffers 
 */
#define BSIZE_SMALL		32
#define BSIZE			4096 

/**
 * @brief Names used in config hash 
 */
#define MJR_CONFIG_BLOC_PREFIX	"mjr.bloc_prefix"
#define MJR_CONFIG_FUNC_PREFIX	"mjr.func_prefix"
#define	MJR_CONFIG_LINK_PREFIX	"mjr.link_prefix"

#define	MJR_CONFIG_BLOC_POSTFIX	"mjr.bloc_postfix"

/**
 * @brief Default subroutines prefix in symbol recovery 
 */
#define	MJR_FUNC_PREFIX		"func_"
#define	MJR_BLOC_PREFIX		"bloc_"
#define	MJR_LINK_PREFIX		"link_"
#define	MJR_BLOC_POSTFIX	"_unseen"

/**
 * @brief OS for which we are able to match the entry point 
 */
#define	MJR_BIN_LINUX		0
#define	MJR_BIN_FREEBSD		1

/**
 * @brief Fingerprinting types 
 */
#define	MJR_FPRINT_TYPE_MD5	0

/**
 * @brief The hash declared in VM for gotos 
 */
extern hash_t	goto_hash;

/* init.c */
int		mjr_init_session(mjrsession_t *);
int		mjr_create_context_as_current(mjrsession_t *, elfshobj_t *);
mjrcontext_t	*mjr_create_context(elfshobj_t *);
int		mjr_setup_processor(mjrsession_t *);

/* core.c */
/**
 * @brief Analysis of unlimited depth
 */
#define		MJR_MAX_DEPTH		-1
#define		MJR_BLOCK_INVALID	((eresi_Addr) -1)
#define		MJR_BLOCK_EXIST		((eresi_Addr) -2)

int		mjr_analyse(mjrsession_t *sess, eresi_Addr addr, int maxdepth, int flags);
int		mjr_analyse_section(mjrsession_t *s, char *sectname);
int		mjr_analyse_addr(mjrsession_t *sess, eresi_Addr addr, int maxdepth, int flags);
int		mjr_analyse_code(mjrsession_t *sess, unsigned char *ptr, unsigned int offset, eresi_Addr vaddr, int len, int curdepth, int maxdepth);
int		mjr_analyse_finished(mjrsession_t *sess);

/* blocks.c */
int		mjr_blocks_get(mjrcontext_t *ctxt);
container_t*	mjr_block_get_by_vaddr(mjrcontext_t *ctxt, eresi_Addr, int);
int		mjr_block_point(mjrcontext_t*, asm_instr*, eresi_Addr, eresi_Addr);
char 		*_vaddr2str(eresi_Addr);
int		mjr_block_relink_cond_always(container_t *, container_t *, int);
int		mjr_block_dump(mjrcontext_t*, container_t *);
int		mjr_block_symbol(mjrcontext_t*, container_t*);

/* fingerprint.c */
int		mjr_block_funcstart(container_t *cntnr);
int		mjr_fingerprint(mjrcontext_t*, container_t*, int, int, 
				int, int, int, int (*fprint)(container_t*));

/* display.c */
int		mjr_blocks_display(mjrcontext_t *c, int);
int		mjr_block_display(mjrcontext_t *, container_t *c, mjropt_t *opt);
void		mjr_function_display(mjrfunc_t *func);
void		mjr_funcs_display(mjrcontext_t *c);

/* cfg.c */
int       	mjr_trace_control(mjrcontext_t *context, container_t *curblock,
				  elfshobj_t *obj, asm_instr *curins, eresi_Addr curvaddr, 
				  eresi_Addr *dstaddr, eresi_Addr *retaddr);
eresi_Addr	mjr_compute_fctptr(mjrcontext_t	*context);
eresi_Addr	mjr_get_jmp_destaddr(mjrcontext_t *context);
eresi_Addr	mjr_get_call_destaddr(mjrcontext_t *context);
int		mjr_asm_check_function_start(mjrcontext_t *ctxt);

/* symtab.c */
int		mjr_symtab_rebuild(mjrsession_t *);
int		mjr_symbol_add(mjrsession_t *, eresi_Addr, char *);
int		mjr_symbol_delete_by_name(mjrsession_t *, char *);
int		mjr_symbol_rename(mjrsession_t *,char *,char *);

/* function.c */
void		*mjr_fingerprint_function(mjrcontext_t *, eresi_Addr addr, int);
void		mjr_function_dump(mjrcontext_t*, char *,container_t *);
int		mjr_functions_get(mjrcontext_t *);
int		mjr_function_register(mjrcontext_t *, u_int, container_t *);
container_t	*mjr_function_get_by_vaddr(mjrcontext_t *, u_int);
int		mjr_function_symbol(mjrcontext_t*, container_t *);

/* ondisk.c */
int		mjr_flow_load(mjrcontext_t *c, u_int datatypeid);
int		mjr_flow_store(mjrcontext_t *c, u_int datatypeid);

/* link.c */
int		mjr_link_block_call(mjrcontext_t *, eresi_Addr, eresi_Addr, eresi_Addr);
int		mjr_link_block_jump(mjrcontext_t *, eresi_Addr, eresi_Addr, eresi_Addr);
int		mjr_link_func_call(mjrcontext_t *ctxt, eresi_Addr src, eresi_Addr dst, eresi_Addr ret);
container_t	*mjr_block_split(mjrcontext_t *ctxt, eresi_Addr	dst, u_char link_with);

/* history.c */
void		mjr_history_shift(mjrcontext_t *cur, asm_instr i, eresi_Addr a);
void		mjr_history_write(mjrcontext_t*, asm_instr*, eresi_Addr a, int i);

/* container.c */
int		mjr_init_containers(mjrcontext_t*);
int		mjr_resize_containers(mjrcontext_t*, u_int resize);
unsigned int	mjr_register_container (mjrcontext_t*, container_t *cntnr);
unsigned int	mjr_register_container_id (mjrcontext_t*, container_t *cntnr);
void		mjr_unregister_container(mjrcontext_t*, u_int id);
container_t *mjr_lookup_container (mjrcontext_t*,u_int id);
container_t	*mjr_get_container_by_vaddr(mjrcontext_t*, eresi_Addr vaddr, int type);
list_t		*mjr_link_get_by_direction(container_t *c, int dir);
mjrlink_t	*mjr_get_link_by_type(list_t *listlink, int link_type);
int		mjr_create_container_linklist(container_t *cur, u_int linktype);
container_t	*mjr_create_block_container(mjrcontext_t*, u_int symoff, eresi_Addr v, u_int s, u_char);
container_t	*mjr_create_function_container(mjrcontext_t*,
					       eresi_Addr	vaddr,
					       u_int		size,
					       char		*name,
					       mjrblock_t	*first,
					       char		*md5);
mjrlink_t	*mjr_container_add_link(mjrcontext_t *ctxt,
					container_t *cntnr, 
					u_int id, 
					u_char link_type, 
					u_char link_scope,
					int link_direction);

/* findentry.c	*/
eresi_Addr	mjr_trace_start(mjrcontext_t *c, u_char *, u_int, eresi_Addr);
eresi_Addr	mjr_find_main(elfshobj_t	*obj,
			      asm_processor	*proc,
			      u_char		*buf,
			      u_int		len,
			      eresi_Addr	vaddr,
			      u_int		*dis);


#endif
