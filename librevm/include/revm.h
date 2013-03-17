/**
 * @defgroup ERESI The ERESI runtime environment
 */
/**
 * @file librevm/include/revm.h
 * @ingroup librevm
 *
 * Started on  Thu Feb 22 07:19:04 2001 jfv
 * $Id$
 */
#ifndef __REVM_H_
 #define __REVM_H_

/* Debug variables */
#include "revm-debug.h"

/* User defined configuration */
#include "revm-vars.h"

/* Do not put all those headers when compiling for kernel */
#ifndef __KERNEL__
#define _GNU_SOURCE

#if defined(__linux__)
#include <features.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <termios.h>

#ifdef __BSD__
#include <util.h>
#elif defined(__linux__)
#include <pty.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>
#include <setjmp.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>

#if defined(__BEOS__)
 #include <ucontext.h>
#elif !defined(__OpenBSD__)
 #include <sys/ucontext.h>
#endif

#include <pthread.h>
#include <stdarg.h> 
#include <regex.h>
#else
 #include "../../libregex/regex.h"
#endif

#include "libelfsh.h"
#include "libedfmt.h"
#include "libaspect.h"

/* Disassembling engine */
#include <libasm.h>

#if defined(ERESI_NET)
 #include <libdump.h>
#endif

#if defined(USE_READLN)
 #include <libui.h>
#else
 #define RL_PROMPT_START_IGNORE '\001'
 #define RL_PROMPT_END_IGNORE   '\002'
#endif

#include "revm-macros.h"
#include "revm-objects.h"
#include "revm-data.h"

/* Registration handlers for options from opt.c */
int		revm_getoption(u_int index, u_int argc, char **argv);
int		revm_getoption2(u_int index, u_int argc, char **argv);
int		revm_getoption3(u_int index, u_int argc, char **argv);
int		revm_getregxoption(u_int index, u_int argc, char **argv);
int		revm_getinput(u_int index, u_int argc, char **argv);
int		revm_getoutput(u_int index, u_int argc, char **argv);
int		revm_getdisasm(u_int index, u_int argc, char **argv);
int		revm_gethexa(u_int index, u_int argc, char **argv);
int		revm_getvarparams(u_int index, u_int argc, char **argv);
int		revm_getforparams(u_int index, u_int argc,  char **argv);
int		revm_getmatchparams(u_int index, u_int argc, char **argv);
int		revm_getcaseparams(u_int index, u_int argc, char **argv);

/* Libasm resolve handlers */
void		asm_do_resolve(void *data, eresi_Addr vaddr, char *, u_int);
char		*revm_resolve(elfshobj_t *file, eresi_Addr addr, elfsh_SAddr *roff);

/* General VM functions */
revmexpr_t	*revm_lookup_param(char *param, u_char existing);
revmobj_t	*revm_check_object(revmobj_t *pobj);
void		revm_destroy_object(revmobj_t *pobj, u_char datafree);
revmobj_t	 *revm_copy_object(revmobj_t *pobj);
elfshobj_t	*revm_getfile(u_int index);
revmmod_t	*revm_getmod(u_int index);
char		*revm_reverse(elfshobj_t *file, u_int vaddr);

/* Lookup functions */
revmobj_t	*revm_lookup_immed(char *param, u_char existing);
revmexpr_t	*revm_lookup_var(char *param);
char		*revm_lookup_key(char *param);
elfshobj_t	*revm_lookup_file(char *param);
u_int		revm_lookup_index(char *param);
char		*revm_lookup_string(char *param);
eresi_Addr	revm_lookup_addr(char *param);

/* Lazy Abstract Type system functions */
int		revm_convert2str(revmobj_t *obj);
int		revm_convert2int(revmobj_t *obj);
int		revm_convert2long(revmobj_t *obj);
int		revm_convert2raw(revmobj_t *obj);
int		revm_convert2byte(revmobj_t *obj);
int		revm_convert2short(revmobj_t *obj);
int		revm_convert2daddr(revmobj_t *obj);
int		revm_convert2caddr(revmobj_t *obj);

/* Command API */
int		revm_command_set(char *cmd, void *exec, void *reg, u_int needcur);
int		revm_command_add(char *cmd, int (*exec)(void*,void*), void *reg, 
				 u_int needfile, char *help);
int		revm_command_del(char *cmd);

/* Default grammar handlers */
revmobj_t       *parse_lookup3_index(char *param, char *fmt, u_int sepnbr);
revmobj_t       *parse_lookup3(char *param, char *fmt, u_int sepnbr);
revmobj_t       *parse_lookup4(char *param, char *fmt, u_int sepnbr);
revmobj_t       *parse_lookup5_index(char *param, char *fmt, u_int sepnbr);
revmobj_t	*parse_vector(char *param, char *fmt);
revmobj_t	*parse_hash(char *param, char *fmt);
revmobj_t	*parse_hash_field(char *param, char *fmt);
revmobj_t	*parse_list(char *param, char *fmt);

/* Versions functions */
int             revm_version_pdef(hashdef_t *p, u_int ai, u_int i, char *id, 
				char *n, char *t, regex_t *r);
int             revm_version_pneed(hashneed_t *p, u_int ai, u_int i, char *id, 
				 char *n, char *t, regex_t *r);
int             revm_version_unk(u_int ai, u_int i, char *id, char *n, char *t);

/* Disassembling and hexadecimal view functions */
int		revm_array_display(elfshsect_t *parent, elfsh_Sym *sym, char *buf, eresi_Addr vaddr,
				   char *name, u_int foffset);
int		revm_instr_display(int, eresi_Addr, u_int, u_int, char *, u_int, char *);
int		revm_section_display(elfshsect_t *s, char *name, revmlist_t *re);
int		revm_match_sht(elfshsect_t *l, revmlist_t *actual);
int		revm_match_symtab(elfshobj_t *file, elfshsect_t *symtab, 
				revmlist_t *actual, int flag);
int		revm_match_special(elfshobj_t *file, eresi_Addr vaddr, revmlist_t*);
int             revm_object_display(elfshsect_t *parent, elfsh_Sym *sym, int size, 
				    u_int off, u_int symoff, u_int foffset, eresi_Addr vaddr, 
				    char *name, char otype, u_char addbase);
int		revm_hexa_display(elfshsect_t *parent, char *name, eresi_Addr vaddr,
				  u_int size, u_int symoff, char *buff, u_int fileoff);

/* Parsing / Scanning functions */
char		*revm_filter_param(char *buf, char *ptr);
char		*revm_build_unknown(char *buf, const char *str, u_long type);
void		revm_filter_zero(char *str);
int		revm_parseopt(int argc, char **argv);
void            revm_findhex(u_int argc, char **argv);
int		revm_trans_speblank(const char *in, char ***av, u_int *ac);
void		revm_replace_speblanks(u_int argc, char **argv);
u_int           revm_findblanks(char *buf);
char            **revm_doargv(u_int nbr, u_int *argc, char *buf);

/* String functions */
int		revm_strtable_add(char *string);

int		revm_traces_add_arguments(int argc, char **argv);
edfmtfunc_t 	*revm_traces_tracable_with_type(elfshobj_t *file, char *func_name, u_char external);

/* Hash functions */
int             revm_hashunk(int i);
int             revm_hashbucket_print(int, int, int, char *, int, int, int);
int             revm_hashchain_print(int i, int s, char *n, int r, int h);

/* Internal functions */
void		revm_proc_init();
revmmod_t	*revm_modprobe();
void		revm_tables_setup();
int		revm_doerror(void (*fct)(char *str), char *str);
void		revm_error(char *label, char *param);
void		revm_badparam(char *str);
void		revm_unknown(char *str);
void		revm_exit(int err);
void		revm_banner_print();
void		revm_dynentinfo(elfshobj_t *f, elfsh_Dyn *ent, char *info);
int		revm_usage(char *str);
int		revm_modlist();
int		revm_isnbr(char *string);
void		revm_workfiles_load();
int		revm_implicit(revmcmd_t *actual);
int	        revm_workfiles_unload();
int		dprintf(int fd, const char *format, ...);
void	        revm_pht_print(elfsh_Phdr *phdr, uint16_t num, eresi_Addr base);
char		*revm_fetch_sht_typedesc(elfsh_Word typenum);
int             revm_sht_print(elfsh_Shdr *shdr, u_int num, char rtflag);
int		revm_load_init_dephash(elfshobj_t *file, char *name);
int		revm_file_load(char *name, eresi_Addr base, elfshlinkmap_t *lm);
int		revm_is_loaded(char *name);
int		revm_doswitch(int nbr);
char		*revm_ascii_type(hash_t *cur);
char		*revm_ascii_ltype(list_t *cur);
char		*revm_modename_get();
char            *revm_basename(char *str);
void            revm_quitmsg_set(char *msg);
int		revm_fifo_io(revmjob_t *job);
void		revm_object_print(revmobj_t *obj);
char		*revm_string_get(char **params);
int		revm_source(char **params);
int		revm_help(char *command);
void		revm_print_actual(revmargv_t *cur);
int		revm_printscript(revmargv_t *start);
char            *revm_get_cur_job_parameter(uint8_t p);
char            *revm_get_cur_job_parameter_with_job(revmjob_t *job, uint8_t p);
uint8_t         revm_get_argc(void);
int             revm_create_new_workspace(char *ws_name);

/* Vector related functions */
int		revm_vectors_getdims(char *str, unsigned int *dims);
char		*revm_ascii_vtype(vector_t *cur);
int		revm_vectors_getdimnbr(char *str);
int		revm_vector_bad_dims(vector_t *v, unsigned int *dims, u_int dimnbr);

/* Dependences related information : deps.c */
int		revm_load_enumdep(elfshobj_t *obj);
int		revm_load_dep(elfshobj_t *p, char *n, eresi_Addr b, elfshlinkmap_t *, hash_t*);
int		revm_unload_dep(elfshobj_t *obj, elfshobj_t *root);
char	     	*revm_load_searchlib(char *name);
elfshobj_t	*revm_is_dep(elfshobj_t *obj, char *path);
elfshobj_t	*revm_is_depid(elfshobj_t *obj, int id);


/* Top skeleton functions */
int		revm_init() __attribute__((constructor));
int		revm_loop(int argc, char **argv);
int		revm_setup(int ac, char **av, char mode, char side);
elfshobj_t	*revm_run(int ac, char **av);
elfshobj_t	*revm_run_no_handler(int ac, char **av);
int		revm_config(char *config);
void		revm_postexec(int retval);
void		revm_cleanup();
void		revm_clean();

/* Scripting flow functions */
int		revm_execscript();
int		revm_execmd();
int		revm_move_pc(char *idx);
int		revm_openscript(char **av);
int		revm_testscript(int ac, char **av);
int		revm_exec_str(char *str);
int	        revm_context_restore(int, char, revmargv_t*, void*, char**, char*);

/* ERESI variables related functions */
int             revm_setvar_str(char *varname, char *value);
int             revm_setvar_raw(char *varname, char *value, u_int len);
int             revm_setvar_byte(char *varname, u_char byte);
int             revm_setvar_short(char *varname, u_short val);
int             revm_setvar_int(char *varname, u_int val);
int             revm_setvar_long(char *varname, u_long val);
char		*revm_tmpvar_create();
int		revm_variable_istemp(revmexpr_t *e);

/* ERESI types related functions */
int		revm_type_prints();
int		revm_type_print_regex(char *regex);
int		revm_type_copy(char *from, char *to);
int		revm_type_hashcreate(char *name);
int		revm_type_reflect(hash_t *hash, char *typename);

/* Data access related functions */
revmobj_t	*revm_object_lookup_real(aspectype_t *type, char *objname, char *objpath, char trans);
revmobj_t	*revm_object_lookup(char *str);
revmobj_t	*revm_object_create(aspectype_t *type, void *data, char transaddr, u_char perm);

/* Generic handlers for data accesses */
char		*revm_generic_getname(void *type, void *data);
int		revm_generic_setname(void *type, void *data, void *newdata);
int		revm_addr_setobj(void *data, eresi_Addr value);
eresi_Addr	revm_addr_getobj(void *data);
eresi_Addr	revm_hash_getobj(void *data);
eresi_Addr	revm_byte_getobj(void *data);
int		revm_byte_setobj(void *data, eresi_Addr value);
eresi_Addr	revm_short_getobj(void *data);
int		revm_short_setobj(void *data, eresi_Addr value);
eresi_Addr	revm_int_getobj(void *data);
int		revm_int_setobj(void *data, eresi_Addr value);
int		revm_long_setobj(void *data, eresi_Addr value);
eresi_Addr	revm_long_getobj(void *data);
char		*revm_generic_getdata(void *data, int off, int sizelm);
int		revm_generic_setdata(void *d, int off, void *ndat, int sz, int szlm);

/* Object creation/verification functions */
int		revm_convert_object(revmexpr_t *e, u_int objtype);
revmL1_t	*revm_create_L1ENT(void	*get_obj,
				 void	*get_obj_idx,
				 void	*get_obj_nam,
				 hash_t	*l2_hash,
				 void	*get_entptr,
				 void	*get_entval,
				 void	*set_entval,
				 u_int	elem_size);
revmL2_t	*revm_create_L2ENT(void	*get_obj,
				 void	*set_obj,
				 char	type,
				 void	*get_name,
				 void	*set_name,
				 void	*get_data,
				 void	*set_data);
revmcmd_t	*revm_create_CMDENT(int	(*exec)(void *file, void *av),
				  int	(*reg)(u_int i, u_int ac, char **av),
				  int	flags, char *help);
elfshredir_t	*revm_create_REDIR(u_char type, char *sname, char *dname, 
				 eresi_Addr saddr, eresi_Addr daddr);
revmobj_t	*revm_create_IMMED(char type, char perm, u_int val);
revmobj_t	*revm_create_IMMEDSTR(char perm, char *str);
revmobj_t	*revm_create_LONG(char perm, eresi_Addr val);
revmobj_t	*revm_create_CADDR(char perm, eresi_Addr val);
revmobj_t	*revm_create_DADDR(char perm, eresi_Addr val);
revmobj_t	*revm_create_ptr(char perm, eresi_Addr val, u_int type);

/* Interface related functions */
int		revm_system(char *cmd);
void		revm_dbgid_set(u_int pid);
u_int		revm_dbgid_get();

/* Atomic operations */
int             revm_preconds_atomics(revmexpr_t **o1, revmexpr_t **o2);
int		revm_nextconds_atomics(revmexpr_t *o1, revmexpr_t *o2);
int		revm_arithmetics(revmexpr_t *dst, revmexpr_t *e1, revmexpr_t *e2, u_char op);
int		revm_hash_add(hash_t *h, revmexpr_t *e);
int		revm_hash_del(hash_t *h, revmexpr_t *e);
int		revm_elist_add(list_t *h, revmexpr_t *e);
int		revm_elist_del(list_t *h, revmexpr_t *e);
int		revm_hash_set(char *tab, char *elm, void *obj, u_int type);
int		revm_elist_set(char *tab, char *elm, void *obj, u_int type);
int		revm_testbit(revmexpr_t *o1, revmexpr_t *o2, u_int *result);
int		revm_object_compare(revmexpr_t *e1, revmexpr_t *e2, eresi_Addr *val);
int		revm_object_set(revmexpr_t *e1, revmexpr_t *e2);

/* Job related functions */
int		revm_own_job(revmjob_t *job);
int		revm_valid_workspace(char *name);
void		revm_switch_job(revmjob_t *job);
revmjob_t	*revm_clone_job(char *name, revmjob_t *job);
int		revm_add_script_cmd(char *dirstr);
revmjob_t	*revm_localjob_get();

#ifdef __KERNEL__
revmjob_t	*revm_socket_add(int socket, void *addr);
#else
revmjob_t	*revm_socket_add(int socket, struct sockaddr_in *addr);
#endif

int              revm_screen_switch();
int              revm_screen_clear(int i, char c);
int              revm_screen_update(u_short isnew, u_short prompt_display);
int		 revm_workspace_next();

/* libedfmt related functions */
int		revm_edfmt_parse(elfshobj_t *file);
int		revm_edfmt_uni_print(elfshobj_t *file);

/* Type/Inform related functions */
revmexpr_t	*revm_inform_type(char *type, char *name, eresi_Addr, revmexpr_t *e, u_char p, u_char r);
revmexpr_t	*revm_inform_toplevel(char *type, char *varname, 
				      char *straddr, revmexpr_t *expr,
				      u_char print, u_char rec);
revmexpr_t	*revm_inform_type_addr(char *t, char *n, eresi_Addr a, revmexpr_t *, u_char p, u_char r);
int		revm_check_addr(elfshobj_t *obj, eresi_Addr add);
int             revm_informed_print(char *name);
int             revm_uninform_type(char *type, char *varname, u_char print);
int		revm_type_copy(char *from, char *to);
int		revm_type_hashcreate(char *name);
int		revm_type_print(char *type, char mode);
revmannot_t	*revm_annot_get(char *name);

/* Expression related functions */
revmexpr_t	*revm_simple_expr_create(aspectype_t *datatype, char *name, char *value);
revmexpr_t	*revm_expr_create(aspectype_t *type, char *name, char *val);
revmexpr_t	*revm_expr_get(char *pathname);
int		revm_expr_compare(revmexpr_t *orig, revmexpr_t *candid, eresi_Addr *val);
int		revm_expr_match(revmexpr_t *candid, revmexpr_t *orig);
int		revm_expr_set(revmexpr_t *adst, revmexpr_t *asrc);
int		revm_expr_print(revmexpr_t *expr, u_char quiet);
int		revm_expr_print_by_name(char *pathname, u_char quiet);
int		revm_expr_match_by_name(char *original, char *candidate);
int		revm_expr_compare_by_name(char *original, char *candidate, eresi_Addr *val);
int		revm_expr_set_by_name(char *dest, char *source);
aspectype_t	*revm_exprtype_get(char *exprvalue);
int		revm_expr_unlink_by_name(char *e, u_char exprfree, u_char datafree);
revmexpr_t	*revm_expr_create_from_object(revmobj_t *copyme, char *name, u_char force);
revmexpr_t	*revm_expr_copy(revmexpr_t *source, char *dstname, u_char isfield);
int		revm_expr_destroy_by_name(char *ename);
int		revm_expr_destroy(revmexpr_t *expr);
int		revm_expr_hide(char *ename);
int		revm_expr_clean(char *ename);
revmexpr_t	*revm_expr_lookup(u_int oid);
revmexpr_t	*revm_compute(char *str);
revmexpr_t	*revm_expr_extend(char *dstname, char *srcvalue);

/* May not be defined */
#ifndef __KERNEL__

#ifdef __BSD__
extern int vsscanf(const char * restrict str, const char * restrict format,
		     va_list ap);
#elif defined(__linux__)
extern int vsscanf (__const char *__restrict __s,
                    __const char *__restrict __format, _G_va_list __arg);
#endif

void		wait4exit(void *);

#endif /* __KERNEL__ */

#endif /* __REVM_H_ */

