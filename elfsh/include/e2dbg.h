/*
* e2dbg.h for The Embedded ELF debugger 
*
* Started on Sun 05 Jun 2005 17:54:01 mm
*
*/


#define			__E2DBG_DEBUG__		0


#define			E2DBG_PROMPT		"(e2dbg-"ELFSH_VERSION") "
#define			E2DBG_NAME		"Embedded ELF Debugger"
#define			E2DBG_DYNAMIC_LINKMAP	((elfshlinkmap_t *) 1)
#define			E2DBG_ARGV0		"e2dbg"
#define			E2DBG_PROFILER_BUFLEN	256

/* Here the debugger specific commands */
#define		E2DBG_PROFILE_IN(file, func, line)	\
do							\
{							\
  if (elfsh_is_prof_enable())				\
    ELFSH_PROFILE_IN(file, func, line);			\
}							\
while (0)



/* This structure contains the internal data of the debugger placed in the VM */
typedef struct		s_e2dbgworld
{
  void			*e2dbg_funcret;
  int			e2dbg_init_done;
  
  ucontext_t		*context;			/* Context information : registers etc */
  
  hash_t		bp;
}			e2dbgworld_t;


extern e2dbgworld_t	e2dbgworld;


/* e2dbg/elfsh switching name fonctions */
char		*vm_get_prompt();
char		*vm_get_mode_name();
void		e2dbg_init(void) __attribute__((constructor));
void		e2dbg_setup(char *name);
int		e2dbg_entry(int ac, char **av);
int		e2dbg_run(int ac, char **av);
void            e2dbg_sigsegv_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *context);
void		e2dbg_genericbp_ia32(int signum, siginfo_t *info, void *context);




/* Signal handling for debugger */
#define		CLRSIG	do {		    \
		signal(SIGSEGV, SIG_IGN);   \
		signal(SIGINT, SIG_IGN);    \
		signal(SIGTRAP, SIG_IGN);   \
}		while (0);

#define		SETSIG	do {				\
 struct sigaction ac;					\
							\
 memset(&ac, 0x00, sizeof(ac));				\
 ac.sa_flags       = SA_SIGINFO;			\
 ac.sa_sigaction   = e2dbg_genericbp_ia32;		\
 sigaction(SIGTRAP, &ac, NULL);				\
 ac.sa_sigaction   = e2dbg_sigsegv_handler;		\
 sigaction(SIGSEGV, &ac, NULL);				\
 ac.sa_sigaction   = e2dbg_sigint_handler;		\
 sigaction(SIGINT, &ac, NULL);				\
}		while (0);

#define		INIT do {				\
	        if (!e2dbgworld.e2dbg_init_done)	\
	        e2dbg_init();				\
}		while (0);
 
