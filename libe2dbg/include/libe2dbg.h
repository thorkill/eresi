/**
 * @defgroup libe2dbg libe2dbg: The Embedded ERESI debugger library.
 * @ingroup e2dbg
 */

/**
 * @file libe2dbg/include/libe2dbg.h
 * @brief The Embedded ERESI debugger header file.
 * @ingroup libe2dbg
 *
 * Started on Sun 05 Jun 2005 17:54:01 jfv
 * $Id$
 */
#ifndef __E2DBG_H__
 #define __E2DBG_H__

#define _GNU_SOURCE

#if defined(__OpenBSD__)
#include <sys/time.h>

/* Environ is not defined in unistd.h */
extern char **environ;
#endif

#include <sys/resource.h>

#if defined(__linux__)
#include <features.h>
#endif

#include <sys/types.h>

#include "revm.h"
#include "aproxy.h"
#include "libstderesi.h"

#if defined(__FreeBSD__)
 #include <pthread_np.h>
#endif

#if defined(sun)
 #include <sys/link.h>
#endif

#define _GNU_SOURCE
#include <sys/ucontext.h>

#define		__DEBUG_E2DBG__	        0
#define		__DEBUG_BP__		0
#define		__DEBUG_EMALLOC__       0
#define		__DEBUG_LINKMAP__	0
#define		__DEBUG_THREADS__	0

#define		E2DBG_DYNAMIC_LINKMAP	((elfshlinkmap_t *) 1)
#define		E2DBG_ABSENT_LINKMAP	((elfshlinkmap_t *) 2)
#define		E2DBG_PROFILER_BUFLEN	256

#define		E2DBG_CONFIG		".e2dbgrc"
#define		E2DBG_HOOK_GETREGS	"hook_getregs"
#define		E2DBG_HOOK_SETREGS	"hook_setregs"
#define		E2DBG_HOOK_PRINTREGS	"hook_printregs"
#define		E2DBG_HOOK_GETPC	"hook_getpc"
#define		E2DBG_HOOK_SETSTEP	"hook_setstep"
#define		E2DBG_HOOK_RESETSTEP	"hook_resetstep"
#define		E2DBG_HOOK_GETFP	"hook_getfp"
#define		E2DBG_HOOK_NEXTFP	"hook_nextfp"
#define		E2DBG_HOOK_GETRET	"hook_getret"
#define		E2DBG_HOOK_BREAK	"hook_setbreak"
#define		E2DBG_HOOK_DELBREAK	"hook_delbreak"

/* Kernel related defines */
#define		E2DBG_VSYSCALL_RETADDR	(0xFFFFE420)
#define		E2DBG_SIGTRAMP_RETADDR	(0xFFFFE440)
#define		E2DBG_KERNELBASE	((eresi_Addr) 0xC0000000)

/* Generic register names */
#define	        E2DBG_SSP_VAR		"$ssp"	/* Saved stack ptr */
#define		E2DBG_SP_VAR		"$sp"
#define		E2DBG_FP_VAR		"$fp"
#define	        E2DBG_PC_VAR		"$pc"

/* IA32 registers names */
#define		E2DBG_EAX_VAR		"$eax"
#define		E2DBG_EBX_VAR		"$ebx"
#define		E2DBG_ECX_VAR		"$ecx"
#define		E2DBG_EDX_VAR		"$edx"
#define	        E2DBG_ESI_VAR		"$esi"
#define	        E2DBG_EDI_VAR		"$edi"

/* IA32 under solaris */
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

/* SPARC registers name */
#define		E2DBG_TSTATE_VAR	"$tstate"
#define		E2DBG_NPC_VAR		"$npc"
#define		E2DBG_Y_VAR		"$y"
#define		E2DBG_G1_VAR		"$g1"
#define		E2DBG_G2_VAR		"$g2"
#define		E2DBG_G3_VAR		"$g3"
#define		E2DBG_G4_VAR		"$g4"
#define		E2DBG_G5_VAR		"$g5"
#define		E2DBG_G6_VAR		"$g6"
#define		E2DBG_G7_VAR		"$g7"
#define		E2DBG_O0_VAR		"$o0"
#define		E2DBG_O1_VAR		"$o1"
#define		E2DBG_O2_VAR		"$o2"
#define		E2DBG_O3_VAR		"$o3"
#define		E2DBG_O4_VAR		"$o4"
#define		E2DBG_O5_VAR		"$o5"
#define		E2DBG_O6_VAR		"$o6"
#define		E2DBG_O7_VAR		"$o7"

/* AMD64 registers name */
#define		E2DBG_R8_VAR		"$r8"
#define		E2DBG_R9_VAR		"$r9"
#define		E2DBG_R10_VAR		"$r10"
#define		E2DBG_R11_VAR		"$r11"
#define		E2DBG_R12_VAR		"$r12"
#define		E2DBG_R13_VAR		"$r13"
#define		E2DBG_R14_VAR		"$r14"
#define		E2DBG_R15_VAR		"$r15"
#define		E2DBG_RDI_VAR		"$rdi"
#define		E2DBG_RSI_VAR		"$rsi"
#define		E2DBG_RBP_VAR		"$rbp"
#define		E2DBG_RBX_VAR		"$rbx"
#define		E2DBG_RAX_VAR		"$rax"
#define		E2DBG_RDX_VAR		"$rdx"
#define		E2DBG_RCX_VAR		"$rcx"
#define		E2DBG_RSP_VAR		"$rsp"
#define		E2DBG_RIP_VAR		"$rip"
#define		E2DBG_EFLAGS_VAR	"$eflags"
#define		E2DBG_CS_VAR		"$cs"
#define		E2DBG_GS_VAR		"$gs"
#define		E2DBG_FS_VAR		"$fs"
#define		E2DBG_CR2_VAR		"$cr2"


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
 signal(SIGSEGV, SIG_IGN);				\
 signal(SIGBUS, SIG_IGN);				\
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
 ac.sa_sigaction   = e2dbg_sigsegv_handler;		\
 sigaction(SIGSEGV, &ac, NULL);			        \
 ac.sa_sigaction   = e2dbg_sigsegv_handler;		\
 sigaction(SIGBUS, &ac, NULL);		                \
} while (0)


// was in SETSIG -> commenting it make the core available
// ac.sa_sigaction   = e2dbg_sigsegv_handler;      	
//sigaction(SIGSEGV, &ac, NULL);			        


/* Create variable from register value */
#define			E2DBG_GETREG(name, val)		\
do							\
{							\
  revmobj_t		*r;				\
  revmexpr_t		*e;				\
  e = revm_expr_get(name);				\
  if (!e || !e->value)					\
    {							\
      r = revm_create_LONG(1, val);			\
      e = revm_expr_create_from_object(r, name, 1);	\
    }							\
  else							\
    e->value->immed_val.ent = val;			\
}							\
while (0)

/* Set the register value */
#define			E2DBG_SETREG(name, val)		\
do							\
{							\
  revmobj_t		*r;				\
  revmexpr_t		*e;				\
  e = revm_expr_get(name);				\
  if (!e || !e->value)					\
    {							\
      r = revm_create_LONG(1, val);			\
      e = revm_expr_create_from_object(r, name, 1);	\
    }							\
  else							\
   val = e->value->immed_val.ent;	                \
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
  eresi_Off	dynoff;
  eresi_Off	symoff;
  eresi_Off	stroff;
  eresi_Off	strsz;
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

  eresi_Addr		past;			/* Previous opcode instead of break */
  u_char		step;			/* Is this thread beeing stepped ? */
  u_char		was_step;		/* Is this thread was just beeing stepped ? */
  u_char		trace;			/* Is the thread beeing itraced ? */
  void			*(*entry)(void *);	/* Entry point */
  pthread_attr_t	*attr;			/* Thread attributes */
  ucontext_t		*context;		/* Thread context on signal */

  time_t		stime;			/* Creation time */
  time_t		etime;			/* Ending time */

  eresi_Addr		tlsaddr;		/* Address of TLS data */
  unsigned int		tlsize;			/* Size of TLS data */
  eresi_Addr		stackaddr;		/* Address of stack */
  unsigned int		stacksize;		/* Size of stack */

}			e2dbgthread_t;


/* Hold all resolved symbols that we need in pre-malloc stage */
typedef struct		s_e2dbgsyms
{
  eresi_Addr		piebase;		/* Computed base address for PIE */
  eresi_Addr		mallocsym;		/* Resolved libc malloc */
  eresi_Addr		vallocsym;		/* Resolved libc valloc */
  eresi_Addr		callocsym;		/* Resolved libc calloc */
  eresi_Addr		reallocsym;		/* Resolved libc realloc */
  eresi_Addr		memalignsym;		/* Resolved libc memalign*/
  eresi_Addr		freesym;		/* Resolved libc free */
  eresi_Addr		mallochooksym;		/* Resolved libc malloc hook */
  eresi_Addr		memalignhooksym;        /* Resolved libc memalign hook */
  eresi_Addr		pthstartupsym;		/* Resolved __libc_malloc_pthread_startup */
  eresi_Addr		pthreadcreate;		/* Resolved pthread_create */
  eresi_Addr		pthreadexit;		/* Resolved pthread_exit */
  eresi_Addr		signal;			/* Resolved signal function */
#if defined(sun)
  Link_map		*map;			/* Linkmap has a different format on solaris */
#else
  elfshlinkmap_t	*map;			/* Early resolved linkmap */
#endif
}			e2dbgsyms_t;


/* This structure contains the internal data of the debugger placed in the VM */
typedef struct		s_e2dbgworld
{
  char			preloaded;			/* Say if we were preloaded */
  hash_t		bp;				/* Breakpoints hash table */
  hash_t		threads;			/* Threads hash table */
  u_char		sourcing;			/* We are executing a debugger script */
  e2dbgsyms_t		syms;				/* Resolved symbol informations */

  /* Display commands memory */
#define			E2DBG_STEPCMD_MAX	50
  char			*displaycmd[E2DBG_STEPCMD_MAX];	/* Commands to be executed on step */
  u_short		displaynbr;			/* Number of global display cmd */

  /* Data currently being traced */
  eresi_Addr		tracedata[E2DBG_STEPCMD_MAX];	/* Address of traced variables */
  char			*tracedstr[E2DBG_STEPCMD_MAX];	/* Strings for traced variables */
  u_short		tdatanbr;			/* Number of traced variables */		

  /* Current debuggee threads information */ 
  elfshbp_t		*curbp;				/* Current breakpoint if any */
  e2dbgthread_t		*curthread;			/* Currently working thread */
  e2dbgthread_t		*stoppedthread;			/* Latest stopped thread */
  u_int			threadnbr;			/* Number of existing threads */
  u_int			threadsyncnbr;			/* Number of threads with contexts */
  u_int			threadgotnbr;			/* Number of threads with retreived contexts */

  /* Synchronization values */
#define			E2DBG_MUTEX_UNLOCKED	0
#define			E2DBG_MUTEX_LOCKED	1
  elfshmutex_t		dbgbp;				/* The breakpoint handler mutex */

  /* Exit status */
  int			exited;				/* Debugger exited */
  int			debuggee_exited;		/* Debuggee exited */
  int			(*real_main)(int argc, char **argv, char **aux);
}			e2dbgworld_t;

/* The Debugger world in the VM */
extern e2dbgworld_t	e2dbgworld;

/* Some libc extern */
#if defined(__FreeBSD__) || defined(__NetBSD__)
 extern char		*__progname;
 extern char		**environ;
#elif defined(sun)
 extern char		**environ;
#else
 extern char		*__progname_full;
#endif

/* Default hook handler */
void		e2dbg_default_printregs();
void            e2dbg_default_getregs();
void            e2dbg_default_setregs();
eresi_Addr*     e2dbg_default_getpc();
void            e2dbg_default_setstep();
void            e2dbg_default_resetstep();
int		e2dbg_default_deletebreak_handler(elfshbp_t *bp);
void            e2dbg_setup_hooks();

/* User hook handlers */
int             e2dbg_user_hooks_install(void);

/* libe2dbg.hooks */
int		e2dbg_getregs();
int		e2dbg_setregs();
int		e2dbg_printregs();
eresi_Addr*     e2dbg_getpc();
int             e2dbg_setstep();
int             e2dbg_resetstep();
eresi_Addr	e2dbg_getret(elfshobj_t *file, eresi_Addr addr);
eresi_Addr	e2dbg_nextfp(elfshobj_t *file, eresi_Addr addr);
eresi_Addr*     e2dbg_getfp();
int		e2dbg_setbreak(elfshobj_t *file, elfshbp_t *bp);
int		e2dbg_deletebreak(elfshbp_t *bp);

/* Backend for ia32 */
void            e2dbg_get_regvars_ia32_bsd();
void            e2dbg_get_regvars_ia32_sysv();
void            e2dbg_set_regvars_ia32_bsd();
void            e2dbg_set_regvars_ia32_sysv();
eresi_Addr*     e2dbg_getpc_bsd_ia32();
eresi_Addr*     e2dbg_getpc_sysv_ia32();
void            e2dbg_setstep_bsd_ia32();
void            e2dbg_setstep_sysv_ia32();
void            e2dbg_resetstep_sysv_ia32();
void            e2dbg_resetstep_bsd_ia32();
eresi_Addr*	e2dbg_getfp_sysv_ia32();
eresi_Addr*	e2dbg_getfp_bsd_ia32();
void            *e2dbg_bt_ia32(void *frame);
void            *e2dbg_getret_ia32(void *frame);
int             e2dbg_break_ia32(elfshobj_t *f, elfshbp_t *bp);
int		e2dbg_delbreak_user(elfshbp_t *bp);
void		e2dbg_print_regvars_ia32_freebsd();
void		e2dbg_print_regvars_ia32_netbsd();
void		e2dbg_print_regvars_ia32_sysv();
void		e2dbg_print_regvars_mips_irix();

/* Backend for AMD64 */
void            e2dbg_get_regvars_amd64_bsd();
void            e2dbg_get_regvars_amd64_sysv();
void            e2dbg_set_regvars_amd64_bsd();
void            e2dbg_set_regvars_amd64_sysv();
eresi_Addr*     e2dbg_getpc_bsd_amd64();
eresi_Addr*     e2dbg_getpc_sysv_amd64();
void            e2dbg_setstep_bsd_amd64();
void            e2dbg_setstep_sysv_amd64();
void            e2dbg_resetstep_sysv_amd64();
void            e2dbg_resetstep_bsd_amd64();
eresi_Addr*	e2dbg_getfp_sysv_amd64();
eresi_Addr*	e2dbg_getfp_bsd_amd64();
void            *e2dbg_bt_amd64(void *frame);
void            *e2dbg_getret_amd64(void *frame);
int             e2dbg_break_amd64(elfshobj_t *f, elfshbp_t *bp);


/* Backend for sparc32 */
void            e2dbg_get_regvars_sparc32_bsd();
void            e2dbg_get_regvars_sparc32_sysv();
void            e2dbg_set_regvars_sparc32_bsd();
void            e2dbg_set_regvars_sparc32_sysv();
eresi_Addr*     e2dbg_getpc_bsd_sparc32();
eresi_Addr*     e2dbg_getpc_sysv_sparc32();
eresi_Addr*     e2dbg_getfp_bsd_sparc32();
eresi_Addr*     e2dbg_getfp_sysv_sparc32();
void            e2dbg_setstep_bsd_sparc32();
void            e2dbg_setstep_sysv_sparc32();
void            e2dbg_resetstep_sysv_sparc32();
void            e2dbg_resetstep_bsd_sparc32();
void            *e2dbg_bt_sparc32(void *frame);
void            *e2dbg_getret_sparc32(void *frame);
int             e2dbg_break_sparc32(elfshobj_t *f, elfshbp_t *bp);

/* e2dbg vector API for registration */
int		e2dbg_register_sregshook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_gregshook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_pregshook(u_char archtype, u_char hosttype, u_char ostype, void *fct);
int		e2dbg_register_getpchook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_setstephook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_resetstephook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_nextfphook(u_char at, u_char ht, u_char ost, void *(*fct)(void *frame));
int		e2dbg_register_getrethook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_breakhook(u_char a, u_char o, u_char os, void *fct);
int		e2dbg_register_delbreakhook(u_char hosttype, void *fct);
int             e2dbg_register_getfphook(u_char archtype, u_char hosttype,
					 u_char ostype, eresi_Addr *(*fct)(void));

/* More e2dbg API */
char            *e2dbg_get_string(char **params);
int		e2dbg_linkmap(elfshobj_t *file);
char		*revm_get_prompt();
char		*revm_modename_get();
int             e2dbg_self();
int		e2dbg_kill(int pid, int sig);
int		e2dbg_step();

/* Linkmap related API */
int		e2dbg_linkmap_load(char *name);
elfshlinkmap_t*	e2dbg_linkmap_getaddr();

/* breakpoint API */
void		e2dbg_generic_breakpoint(int signum, siginfo_t *info, void *context);
elfshbp_t	*e2dbg_breakpoint_from_id(uint32_t bpid);
int		e2dbg_breakpoint_add(eresi_Addr addr);
int		e2dbg_display(char **cmd, u_int nbr);
int		e2dbg_is_watchpoint(elfshbp_t *b);
elfshbp_t	*e2dbg_breakpoint_lookup(char *name);
eresi_Addr	e2dbg_breakpoint_find_addr(char *str);
int		elfsh_bp_add(hash_t *bps, elfshobj_t *file, 
			     char *resolv, eresi_Addr addr);

/* Stack API */
int		e2dbg_bt();
int		e2dbg_stack_dump(uint32_t size, eresi_Addr start);

/* Mutex API */
int		e2dbg_mutex_init(elfshmutex_t *m);
int		e2dbg_mutex_lock(elfshmutex_t *m);
int		e2dbg_mutex_unlock(elfshmutex_t *m);

/* e2dbg main API */
void		e2dbg_init(void) __attribute__ ((constructor));
void		e2dbg_last_init(void) __attribute__ ((constructor));

int		e2dbg_setup(char *name);
int		e2dbg_entry(e2dbgparams_t *);
void		e2dbg_register_command(void);
void		e2dbg_register_dump(char *name, ureg32 reg);
void		e2dbg_start_proc();
int		e2dbg_output();

/* Early symbol / parent object resolving API */
int		e2dbg_dlsym_init();
eresi_Addr	e2dbg_dlsym(char *sym2resolve);
elfshobj_t	*e2dbg_get_parent_object(eresi_Addr addr);

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
int		e2dbg_curthread_init(void);

#if !defined(sun)
int		pthread_attr_getstack(__const pthread_attr_t *__restrict __attr,
				      void **__restrict __stackaddr,
				      size_t *__restrict __stacksize);
#endif

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
int		cmd_next();

/* Undefined on FreeBSD */
#if defined(__FreeBSD__)
void		on_exit(void *exitptr, int param);
#endif

#endif

