/*
**
** libe2dbg.h for The Embedded ELF debugger 
**
** Started on Sun 05 Jun 2005 17:54:01 mayhem
**
**
** $Id: libe2dbg.h,v 1.11 2007-04-02 18:00:45 may Exp $
**
*/
#ifndef __E2DBG_H__
 #define __E2DBG_H__

#include <sys/resource.h>
#include <sys/types.h>
#include "revm.h"
#include "aproxy.h"

#if defined(__FreeBSD__)
 #include <pthread_np.h>
#endif

#define		__DEBUG_E2DBG__		0
#define		__DEBUG_BP__		1
#define		__DEBUG_EMALLOC__	0
#define		__DEBUG_LINKMAP__	0
#define		__DEBUG_THREADS__	0

#define		E2DBG_DYNAMIC_LINKMAP	((elfshlinkmap_t *) 1)
#define		E2DBG_PROFILER_BUFLEN	256

#define		E2DBG_HOOK_GETREGS	"hook_getregs"
#define		E2DBG_HOOK_SETREGS	"hook_setregs"
#define		E2DBG_HOOK_GETPC	"hook_getpc"
#define		E2DBG_HOOK_SETSTEP	"hook_setstep"
#define		E2DBG_HOOK_RESETSTEP	"hook_resetstep"
#define		E2DBG_HOOK_GETFP	"hook_getfp"
#define		E2DBG_HOOK_NEXTFP	"hook_nextfp"
#define		E2DBG_HOOK_GETRET	"hook_getret"

/* Kernel related defines */
#define		E2DBG_VSYSCALL_RETADDR	(0xFFFFE420)
#define		E2DBG_SIGTRAMP_RETADDR	(0xFFFFE440)
#define		E2DBG_KERNELBASE	((elfsh_Addr) 0xC0000000)

/* Generic register names */
#define	        ELFSH_SSPVAR		"ssp"	/* Saved stack ptr */
#define		ELFSH_SPVAR		"sp"
#define		ELFSH_FPVAR		"fp"
#define	        ELFSH_PCVAR		"pc"

/* IA32 registers names */
#define		ELFSH_EAXVAR		"eax"
#define		ELFSH_EBXVAR		"ebx"
#define		ELFSH_ECXVAR		"ecx"
#define		ELFSH_EDXVAR		"edx"
#define	        ELFSH_ESIVAR		"esi"
#define	        ELFSH_EDIVAR		"edi"

/* ia32 under solaris */
#if (defined(__i386) && defined(sun))
#define		REG_UESP	UESP
#define		REG_EIP		EIP
#define		REG_EAX		EAX
#define		REG_EFL		EFL
#define		REG_ECX		ECX
#define		REG_EDX		EDX
#define		REG_EBX		EBX
#define		REG_ESP		ESP
#define		REG_EBP		EBP
#define		REG_ESI		ESI
#define		REG_EDI		EDI
#endif

/* Debugger commands */
#define		CMD_MODE		"mode"
#define		CMD_LINKMAP		"linkmap"
#define		CMD_BT			"bt"
#define		CMD_BT2			"backtrace"
#define		CMD_BP			"b"
#define		CMD_WATCH		"watch"
#define		CMD_BP2			"break"
#define		CMD_STACK		"stack"
#define		CMD_DBGSTACK		"dbgstack"
#define		CMD_DUMPREGS		"dumpregs"
#define		CMD_STEP		"step"
#define		CMD_ITRACE		"itrace"
#define		CMD_START		"start"
#define		CMD_DELETE		"delete"
#define		CMD_RUN			"run"
#define		CMD_DISPLAY		"display"
#define		CMD_UNDISPLAY		"undisplay"
#define		CMD_RSHT		"rsht"
#define		CMD_RPHT		"rpht"
#define		CMD_THREADS		"threads"


/* Signal handling for debugger */
#define		CLRSIG	do {				\
 struct sigaction ac;					\
							\
 memset(&ac, 0x00, sizeof(ac));				\
 ac.sa_flags       = SA_SIGINFO;			\
 ac.sa_sigaction   = e2dbg_internal_sigsegv_handler;	\
 signal(SIGINT, SIG_IGN);				\
 signal(SIGTRAP, SIG_IGN);				\
 signal(SIGUSR2, SIG_IGN);				\
}		while (0);

#define		SETSIG					\
do							\
{							\
 struct sigaction ac;					\
							\
 memset(&ac, 0x00, sizeof(ac));				\
 ac.sa_flags       = SA_SIGINFO | SA_ONSTACK;		\
 ac.sa_sigaction   = e2dbg_generic_breakpoint;		\
 sigaction(SIGTRAP, &ac, NULL);				\
 ac.sa_sigaction   = e2dbg_sigint_handler;		\
 sigaction(SIGINT, &ac, NULL);				\
 ac.sa_sigaction   = e2dbg_thread_sigusr2;		\
 sigaction(SIGUSR2, &ac, NULL);				\
 ac.sa_sigaction   = e2dbg_sigsegv_handler;      	\
 sigaction(SIGSEGV, &ac, NULL);			        \
} while (0)


/* Create variable from register value */
#define			E2DBG_GETREG(name, val)		\
do							\
{							\
  revmobj_t		*r;				\
  r = hash_get(&vars_hash, name);			\
  if (!r)						\
    {							\
      r = vm_create_LONG(1, val);			\
      hash_add(&vars_hash, name, r);			\
    }							\
  else							\
    r->immed_val.ent = val;				\
}							\
while (0)

/* Set the register value */
#define			E2DBG_SETREG(name, val)		\
do							\
{							\
  revmobj_t		*r;				\
  r = hash_get(&vars_hash, name);			\
  if (!r)						\
    {							\
      r = vm_create_LONG(1, val);			\
      hash_add(&vars_hash, name, r);			\
    }							\
  else							\
   val = r->immed_val.ent;	                        \
}							\
while (0)



/* A mutex is just an unsigned char */
typedef u_char elfshmutex_t;


/* The internal object descriptor for e2dbg when resolving 
   symbols before malloc is available */
typedef	struct	s_eobj
{
  int		fd;
  elfsh_Ehdr	e;
  elfsh_Off	dynoff;
  elfsh_Off	symoff;
  elfsh_Off	stroff;
  elfsh_Off	strsz;
}		e2dbgobj_t;

/* The parameter structure for e2dbg_entry in a dedicated thread */
typedef struct		s_e2dbgparams
{
  char			**av;
  u_int			ac;
}			e2dbgparams_t;


/* The structure representing a thread */
typedef struct		s_thread
{
  pthread_t		tid;			/* Key identification of that thread */
  char			initial;		/* Is this thread the initial one ? */

#define			E2DBG_THREAD_INIT	0
#define			E2DBG_THREAD_STARTED	1
#define			E2DBG_THREAD_BREAKING	2
#define			E2DBG_THREAD_SIGUSR2	3
#define			E2DBG_THREAD_BREAKUSR2	4
#define			E2DBG_THREAD_STOPPING	5
#define			E2DBG_THREAD_RUNNING	6
#define			E2DBG_THREAD_FINISHED	7
  char			state;			/* Initiliazing, Running, Finished */

#define			E2DBG_BREAK_NONE	0
#define			E2DBG_BREAK_HIT		1
#define			E2DBG_BREAK_EXEC	2
#define			E2DBG_BREAK_FINISHED	3
#define			E2DBG_BREAK_MAX		3
  int			count;			/* State when breakpointing */

  elfsh_Addr		past;			/* Previous opcode instead of break */
  u_char		step;			/* Is this thread beeing stepped ? */
  u_char		was_step;		/* Is this thread was just beeing stepped ? */
  u_char		trace;			/* Is the thread beeing itraced ? */
  void			*(*entry)(void *);	/* Entry point */
  ucontext_t		*context;		/* Thread context on signal */

  time_t		stime;			/* Creation time */
  time_t		etime;			/* Ending time */

  elfsh_Addr		tlsaddr;		/* Address of TLS data */
  unsigned int		tlsize;			/* Size of TLS data */
  elfsh_Addr		stackaddr;		/* Address of stack */
  unsigned int		stacksize;		/* Size of stack */

}			e2dbgthread_t;


/* Hold all resolved symbols that we need in pre-malloc stage */
typedef struct		s_e2dbgsyms
{
  elfsh_Addr		mallocsym;			/* Resolved libc malloc */
  elfsh_Addr		vallocsym;			/* Resolved libc valloc */
  elfsh_Addr		callocsym;			/* Resolved libc calloc */
  elfsh_Addr		reallocsym;			/* Resolved libc realloc */
  elfsh_Addr		memalignsym;			/* Resolved libc memalign*/
  elfsh_Addr		freesym;			/* Resolved libc free */
  elfsh_Addr		mallochooksym;			/* Resolved libc malloc hook */
  elfsh_Addr		memalignhooksym;		/* Resolved libc memalign hook */
  elfsh_Addr		pthstartupsym;			/* Resolved __libc_malloc_pthread_startup */
  elfsh_Addr		pthreadcreate;			/* Resolved pthread_create */
  elfsh_Addr		pthreadexit;			/* Resolved pthread_exit */
  elfsh_Addr		signal;				/* Resolved signal function */
  elfshlinkmap_t	*map;				/* Early resolved linkmap */
}			e2dbgsyms_t;


/* This structure contains the internal data of the debugger placed in the VM */
typedef struct		s_e2dbgworld
{
  char			preloaded;			/* Say if we were preloaded */
  hash_t		bp;				/* Breakpoints hash table */
  hash_t		threads;			/* Threads hash table */
  u_char		sourcing;			/* We are executing a debugger script */
  //u_char		dbgpresent;			/* Should we proxy allocations ? */
  //e2dbgcontext_t	dbgcontext;			/* Current e2dbg scripting context */
  e2dbgsyms_t		syms;				/* Resolved symbol informations */

  /* Display commands memory */
#define			E2DBG_STEPCMD_MAX	50
  char			*displaycmd[E2DBG_STEPCMD_MAX];	/* Commands to be executed on step */
  u_short		displaynbr;			/* Number of global display cmd */

  /* Current debuggee threads information */ 
  elfshbp_t		*curbp;				/* Current breakpoint if any */
  e2dbgthread_t		*curthread;			/* Currently working thread */
  e2dbgthread_t		*stoppedthread;			/* Latest stopped thread */
  u_int			threadnbr;			/* Number of existing threads */
  u_int			threadsyncnbr;			/* Number of threads with contexts */
  u_int			threadgotnbr;			/* Number of threads with retreived contexts */

  /* Synchronization values */
#define			ELFSH_MUTEX_UNLOCKED	0
#define			ELFSH_MUTEX_LOCKED	1
  elfshmutex_t		dbgbp;				/* The breakpoint handler mutex */

  /* Exit status */
  int			exited;				/* Debugger exited */
  int			debuggee_exited;		/* Debuggee exited */
  int			(*real_main)(int argc, char **argv, char **aux);
}			e2dbgworld_t;

/* The Debugger world in the VM */
extern e2dbgworld_t	e2dbgworld;

/* Some libc extern */
#ifdef __FreeBSD__
 extern char		*__progname;
 extern char		**environ;
#else
 extern char		*__progname_full;
#endif

/* libe2dbg.hooks */
void            e2dbg_default_getregs();
void            e2dbg_default_setregs();
elfsh_Addr*     e2dbg_default_getpc();
void            e2dbg_default_setstep();
void            e2dbg_default_resetstep();
void            e2dbg_setup_hooks();
void            e2dbg_get_regvars_ia32_bsd();
void            e2dbg_get_regvars_ia32_sysv();
void            e2dbg_set_regvars_ia32_bsd();
void            e2dbg_set_regvars_ia32_sysv();
int		e2dbg_getregs();
int		e2dbg_setregs();
elfsh_Addr*     e2dbg_getpc();
int             e2dbg_setstep();
int             e2dbg_resetstep();
elfsh_Addr*     e2dbg_getpc_bsd_ia32();
elfsh_Addr*     e2dbg_getpc_sysv_ia32();
void            e2dbg_setstep_bsd_ia32();
void            e2dbg_setstep_sysv_ia32();
void            e2dbg_resetstep_sysv_ia32();
void            e2dbg_resetstep_bsd_ia32();
elfsh_Addr*	e2dbg_getfp_sysv_ia32();
elfsh_Addr*	e2dbg_getfp_bsd_ia32();

int		e2dbg_register_sregshook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_gregshook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_getpchook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_setstephook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_resetstephook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_nextfphook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_nextfphook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_getrethook(u_char at, u_char ht, u_char ost, void *f);
elfsh_Addr	e2dbg_getret(elfshobj_t *file, elfsh_Addr addr);
elfsh_Addr	e2dbg_nextfp(elfshobj_t *file, elfsh_Addr addr);
elfsh_Addr*     e2dbg_getfp();

/* More e2dbg API */
char            *e2dbg_get_string(char **params);
int		e2dbg_linkmap(elfshobj_t *file);
char		*vm_get_prompt();
char		*vm_get_mode_name();
int		vm_restore_dbgcontext(int, char, revmargv_t*, void *, char **, char*);

//u_char          e2dbg_presence_get();
//void            e2dbg_presence_set();
//void            e2dbg_presence_reset();

int             e2dbg_self();
int		e2dbg_kill(int pid, int sig);

/* breakpoint API */
void		e2dbg_generic_breakpoint(int signum, siginfo_t *info, void *context);
elfshbp_t	*e2dbg_breakpoint_from_id(uint32_t bpid);
int		e2dbg_breakpoint_add(elfsh_Addr addr, u_char flags);
int		e2dbg_display(char **cmd, u_int nbr);
int		e2dbg_is_watchpoint(elfshbp_t *b);
elfshbp_t	*e2dbg_breakpoint_lookup(char *name);
elfsh_Addr	e2dbg_breakpoint_find_addr(char *str);

/* Stack API */
int		e2dbg_bt();
int		e2dbg_stack_dump(uint32_t size, elfsh_Addr start);

/* Mutex API */
int		e2dbg_mutex_init(elfshmutex_t *m);
int		e2dbg_mutex_lock(elfshmutex_t *m);
int		e2dbg_mutex_unlock(elfshmutex_t *m);

/* e2dbg main API */
void		e2dbg_init(void) __attribute__ ((constructor));
void		e2dbg_last_init(void) __attribute__ ((constructor));

int		e2dbg_setup(char *name);
int		e2dbg_entry(e2dbgparams_t *);
void		e2dbg_start_proc();
int		e2dbg_output();

/* Early symbol / parent object resolving API */
int		e2dbg_dlsym_init();
int		e2dbg_load_linkmap(char *name);
elfshlinkmap_t*	e2dbg_linkmap_getaddr();
elfsh_Addr	e2dbg_dlsym(char *sym2resolve);
elfshobj_t	*e2dbg_get_parent_object(elfsh_Addr addr);

/* Signal handling */
void            e2dbg_sigsegv_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_internal_sigsegv_handler(int signum, siginfo_t *info, void *pcontext);
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_sigstop_handler(int signum, siginfo_t *info, void *pcontext);
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_sigusr1_handler(int signum, siginfo_t *info, void *pcontext);
void            e2dbg_thread_sigusr2(int signum, siginfo_t *info, void *pcontext);

/* Thread API */
void		e2dbg_threads_print();
int		e2dbg_thread_stopall(int signum);
void		e2dbg_thread_contall();
int		e2dbg_curthread_init();
int		pthread_attr_getstack(__const pthread_attr_t *__restrict __attr,
				      void **__restrict __stackaddr,
				      size_t *__restrict __stacksize);
/* e2dbg commands */
int             cmd_mode();
int             cmd_linkmap();
int             cmd_bt();
int             cmd_stack();
int             cmd_dbgstack();
int             cmd_run();
int             cmd_bp();
int             cmd_watch();
int             cmd_display();
int             cmd_undisplay();
int             cmd_delete();
int             cmd_step();
int		cmd_itrace();
int		cmd_start();
int		cmd_dumpregs();
int		cmd_cont();
int		cmd_threads();

/* Undefined on FreeBSD */
#if defined(__FreeBSD__)
void		on_exit(void *exitptr, int param);
#endif


#endif
