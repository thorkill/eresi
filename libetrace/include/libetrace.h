/*
** @file libetrace.h
**
** Libetrace header file in the ERESI project
**
** Started on  Sat Nov 17 12:10:12 2007 eau
** $Id: libetrace.h,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/

#ifndef __LIBETRACE_H_
#define __LIBETRACE_H_

#include "libvars.h"

#ifndef __KERNEL__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <regex.h>

#include <sys/time.h>
#include <time.h>

#if !defined(__OpenBSD__)
#include <elf.h>
#endif

#ifdef __BEOS__
#include <bsd_mem.h>
#endif

#if !defined(__USE_GNU)
#define __USE_GNU
#endif

#ifdef __BEOS__
 #include <ucontext.h>
#elif !defined(__OpenBSD__)
 #include <sys/ucontext.h>
#endif

#if !defined(sgi) && !defined(__OpenBSD__) && !defined(__FreeBSD__) && !defined(__BEOS__)
#include <sys/user.h>
#endif

/*
#if defined(__NetBSD__)
#include <miscfs/procfs/procfs.h>
#elif !defined(__OpenBSD__)
#include <sys/procfs.h>
#endif
*/

#endif /* __KERNEL__ */

#include "libaspect.h"
#include "libelfsh.h"
#include "libasm.h"
#include "libedfmt.h"

#define TRACES_CFLOW			1
#define TRACES_PLT			2
#define TRACES_GOT			3

#define TRACE_USED_longjmp		0
#define TRACE_USED__setjmp		1
#define TRACE_USED_sigaction		2
#define TRACE_USED_snprintf		3
#define TRACE_USED_memset		4
#define TRACE_USED_gettimeofday		5
#define TRACE_USED_write		6
#define TRACE_USED_exit			7
#define TRACE_USED_MAX			8

#define	TRACE_MATCH_ALLOCSTEP		10

#define TRACE_UNTRACABLE_NAME		"untracable"
#define TRACE_ADDR_TABLE		"addrtable"
#define TRACE_PRE_FUNC			"func_"
#define ETRACE_TABLE_NAME 	"etrace_table"
#define ETRACE_EXCLUDE_TABLE_NAME "etrace_exclude_table"
#define ETRACE_PATTERN 		"traces_%s"
#define	REVM_TRACE_REP			".etrace"		/* Traces directory */


typedef struct	s_trace_used
{
  char		*name;
  u_char	exist;
}		trace_used;

/**
 * Trace structure 
 */
typedef struct 	s_traces_args
{
  char		*name;
  char		*typename;
  int		size;
}		traceargs_t;


/**
 * Documentation missing.
 */
typedef struct 	s_traces
{
#define ETRACE_TYPE_DEFAULT "global"
#define ETRACE_FUNC_SIZE 256
  char	       	funcname[ETRACE_FUNC_SIZE];
  elfshobj_t	*file;
  u_char	enable;
  u_int		offset;

#define ELFSH_ARG_INTERN 0
#define ELFSH_ARG_EXTERN 1
  u_char	scope;

#define	ELFSH_ARG_SIZE_BASED 0
#define ELFSH_ARG_TYPE_BASED 1
  u_char	type;

  eresi_Addr	vaddr;

#define ETRACE_MAX_ARGS 20
  traceargs_t arguments[ETRACE_MAX_ARGS];
  u_int		argc;
}		trace_t;


/* Global variables for libetrace */
extern hash_t		traces_table;
extern hash_t		exclude_table;
extern char		*last_parsed_function;
extern trace_used	trace_functions[];
extern int		trace_enabled_count;
extern char		trace_file_base[];
extern hash_t		traces_cmd_hash;
extern hash_t		cmd_hash;

#define TRACE_FUNCTIONS_ADD(_name)		{ #_name, 0 }
#define FUNC_BEGIN(nam)				(trace_functions[TRACE_USED_##nam].exist ? "old_" : "")
#define TRACE_GET_FUNC_NAME(_buf, _size, _addr) snprintf(_buf, _size, TRACE_PRE_FUNC "%s", _addr + 2)
#define TRACE_MATCH_ALL(_funcname)		(!strcmp(_funcname, ".*"))



/*
trace_used trace_functions[] = 
  { 
    TRACE_FUNCTIONS_ADD(longjmp),
    TRACE_FUNCTIONS_ADD(_setjmp),
    TRACE_FUNCTIONS_ADD(sigaction),
    TRACE_FUNCTIONS_ADD(snprintf),
    TRACE_FUNCTIONS_ADD(memset),
    TRACE_FUNCTIONS_ADD(gettimeofday),
    TRACE_FUNCTIONS_ADD(write),
    TRACE_FUNCTIONS_ADD(exit),
    { "", 0 }
  };
  */

/**
 * @brief Whole active elements 
 */

/* trace_create.c */
int		*etrace_init_trace();
hash_t		*etrace_create_trace(char *trace);
hash_t		*etrace_get(char *trace);
char		*etrace_start_tracing(elfshobj_t *file);
trace_t		*trace_param_create(elfshobj_t *file, char *name,
				    edfmtfunc_t *func, eresi_Addr vaddr,
				    u_char external);
edfmtfunc_t 	*trace_func_debug_get(elfshobj_t *file, char *func_name, u_char external);


/* check.c */
int		etrace_valid_faddr(elfshobj_t *file, eresi_Addr addr, eresi_Addr *vaddr, u_char *dynsym);
int 		etrace_tracable(elfshobj_t *file, char *name,
				      eresi_Addr *vaddr, u_char *dynsym);
/* check_untracable.c */
int		etrace_untracable(elfshobj_t *file, char *name);

/* func_add.c */
trace_t 	*etrace_func_add(char *trace, char *name, trace_t *newtrace);
int		traces_add(elfshobj_t *file, char *name, char **optarg);

/* func_remove.c */
int		etrace_funcrm(char *trace, char *name);
int		etrace_funcrmall(char *trace);

/* func_exclude.c */
int		etrace_funcexclude(char *regstr);
int		etrace_funcrmexclude(char *regstr);

/* func_enable.c */
int		etrace_funcenable(char *trace, char *name);
int		etrace_funcenableall(char *trace);

/* func_disable.c */
int		etrace_funcdisable(char *trace, char *name);
int		etrace_funcdisableall(char *trace);

/* func_status.c */
int		etrace_funcsetstatus(hash_t *table, int status);

/* func_match.c */
int		trace_match_funcname(elfshobj_t *file, char *funcname, char ***func_list);
int		trace_match_addrtable(elfshobj_t *file, char ***func_list, u_int *count);

/* func_search.c */
edfmtfunc_t    	*trace_search_uni(elfshobj_t *file, char *name);
edfmtfunc_t	*trace_search_unifile(edfmtfile_t *files, char *name);

/* delete.c */
int		etrace_deletetrace(char *trace);

/* save.c */
int		etrace_save_tracing(elfshobj_t *file);
int		etrace_save_obj(elfshobj_t *file, char *name);

/* search.c */
elfshobj_t   	*elfsh_symbol_search(elfshobj_t *file, char *name);

/* search_addr.c */
int		elfsh_addr_get_func_list(elfshobj_t *file, eresi_Addr **addr);
int		elfsh_addr_is_called(elfshobj_t *file, eresi_Addr addr);

/* errfunc.c */
char		*etrace_geterrfunc();

/* trace_run.c */
int		traces_run(elfshobj_t *file, char **argv, int argc);
int		trace_param_add(int argc, char **argv);

/* trace_list.c */
int		traces_list_detail(hash_t *table, char *name);

#endif
