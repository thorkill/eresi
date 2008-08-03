/**
** @file revm.h
** @ingroup librevm_hdr
** Started on  Thu Feb 22 07:19:04 2001 jfv
** Moved from elfsh to librevm on January 2007 -may
**
** $Id: revm.h,v 1.97 2008-02-16 12:32:27 thor Exp $
*/
#ifndef __REVM_H_
 #define __REVM_H_

/* User defined configuration */
#include "revm-vars.h"

/* Do not put all those headers when compiling for kernel */
#ifndef __KERNEL__

#define __USE_ISOC99

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
#define __USE_GNU
#if !defined(__OpenBSD__)
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

#if defined(ERESI_NET)
 #include <libdump.h>
#endif

#if defined(USE_READLN)
 #include <libui.h>
#else
 #define RL_PROMPT_START_IGNORE '\001'
 #define RL_PROMPT_END_IGNORE   '\002'
#endif

#if defined(__FreeBSD__)
  extern char **environ;
#endif

#if defined(KERNSH)
#include "libkernsh.h"
#endif

/* Disassembling engine */
#include <libasm.h>
 
extern asm_processor	proc;

/* Now come INTERNAL DEBUGGING VARIABLES for various part of the ERESI code */
#define	__DEBUG_DISASM__	FALSE
#define	__DEBUG_SIGHANDLER__	FALSE
#define	__DEBUG_LANG__		FALSE
#define	__DEBUG_SCANNER__	FALSE
#define	__DEBUG_ASLR__		FALSE
#define __DEBUG_NETWORK__	TRUE
#define __DEBUG_RESOLVE__	FALSE
#define __DEBUG_HIJACK__	FALSE
#define __DEBUG_TEST__		FALSE
#define	__DEBUG_GRAPH__		FALSE
#define __DEBUG_ARG_COUNT__	FALSE
#define	__DEBUG_EXPRS__		FALSE
#define	__DEBUG_EXPRS_MORE__	FALSE

/* Parsing related defines */
#define	REVM_MAXNEST_LOOP	10

/* XXX: REVM still needs those definition somewhere ... modularity weaknesses w/ libstderesi */
#define	E2DBG_NAME		"Embedded ELF Debugger"
#define	E2DBG_ARGV0		"e2dbg"
#define	CMD_CONTINUE		"continue"
#define	CMD_CONTINUE2		"cont"
#define	CMD_QUIT		 "quit"
#define	CMD_QUIT2		 "exit"
#define	CMD_RETURN		"return"
#define	CMD_RETURN2		"ret"
#define	CMD_PARAM_INTO		"into"
#define	CMD_FOREACH		"foreach"
#define	CMD_FOREND		"forend"
#define	CMD_MATCH		"rewrite"
#define	CMD_MATCHEND		"rwtend"
#define	CMD_DEFAULT		"default"
#define	CMD_CASE		"case"
#define	CMD_PRE			"pre"
#define	CMD_POST		"post"

/* Special cmd_ return values that indicate special events */
#define	REVM_SCRIPT_ERROR	(-1)
#define	REVM_SCRIPT_OK		(0)
#define	REVM_SCRIPT_CONTINUE	(1)
#define	REVM_SCRIPT_STOP	(2)
#define	REVM_SCRIPT_QUIT	(3)

/* General usage macros */
#define FATAL(a)		{ perror(a); revm_exit(-1);		      }
#define QUIT_ERROR(a)		{ revm_exit(a);				      }
#define RET(a)			PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, a)
#define RETERR(a)		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, a, -1)
#define	PERROR_RET(a, b)	{ perror(a); RETERR (b);		      }
#define REGX_IS_USED(a)		a
#define	IS_BLANK(c)		(c == ' ' || c == '\t')

#define	REVM_VAR_ARGC		"$#"
#define REVM_VAR_RESULT		"$_"
#define	REVM_VAR_LOAD		"$!"
#define	REVM_VAR_ERROR		"$ERR"
#define	REVM_VAR_SHELL		"$SHELL"
#define	REVM_VAR_EDITOR		"$EDITOR"
#define REVM_VAR_LIBPATH	"$LPATH"
#define REVM_VAR_STRIPLOG	"$SLOG"
#define	REVM_TMPVAR_PREFIX	"$TMPVAR"
#define	REVM_VAR_ESHLEVEL	"$ESHLVL"
#define	REVM_VAR_PREFIX		'$'

/* Some constant tokens for case command */
#define	REVM_CASE_ARROW		"->"
#define	REVM_CASE_QMARK		"?"

/* REVM atomic operations */
#define	REVM_OP_UNKNOW		0
#define	REVM_OP_ADD		1
#define	REVM_OP_SUB		2
#define	REVM_OP_MUL		3
#define	REVM_OP_DIV		4
#define	REVM_OP_MOD		5
#define	REVM_OP_SET		6
#define	REVM_OP_CMP		7
#define	REVM_OP_MATCH		8
#define	REVM_OP_COPY		9

/* Some useful macros */
#define	CHOOSE_REGX(r, idx)  r = (world.curjob->curcmd->use_regx[idx] ?       \
			     &world.curjob->curcmd->regx[idx] :               \
			     world.state.revm_use_regx ? &world.state.revm_regx : \
			     NULL)
#define	FIRSTREGX(r)	     CHOOSE_REGX(r, 0)
#define	SECONDREGX(r)	     CHOOSE_REGX(r, 1)

/** Macros for declaring commands inside modules
 ** Necessary for beeing able to use module commands
 ** in script without loading the module during the
 ** entire script */
#define	REVM_COMMAND_DECLARE(name)	char modcmd_##name = name

/* ELF Versions */
#define ELFSH_VERTYPE_NONE    0
#define ELFSH_VERTYPE_UNK     1
#define ELFSH_VERTYPE_NEED    2
#define ELFSH_VERTYPE_DEF     3
#define ELFSH_VERENTRY_MAX    4

#define ELFSH_RELOC_MAX(file)   revm_getmaxrelnbr(file)

/* REVM general parameters */
#define	REVM_MAXARGC		128
#define	REVM_SEP		"."
#define	REVM_COMMENT_START	'#'
#define REVM_DASH		'-'
#define	REVM_SLASH		'/'
#define	REVM_SPACE		' '

#define ELFSH_NAME		"ELF shell"
#define	ELFSH_INIT		"elfsh_init"
#define	ELFSH_FINI		"elfsh_fini"
#define ELFSH_HELP		"elfsh_help"

/* REVM files */
#define	REVM_CONFIG		".eresirc"
#define	REVM_FIFO_C2S		"/tmp/.revm.io.c2s"
#define	REVM_FIFO_S2C		"/tmp/.revm.io.s2c"

/* For revm_object_display() */
#define	REVM_VIEW_HEX		0
#define	REVM_VIEW_DISASM	1

/* For elfsh/elfsh/modules.c:revm_change_handler() */
#define	ERESI_ORIG		((void *) -1)

/* For lang/access.c */
#define	REVM_INVALID_FIELD	((u_int) -1)

/* ELFsh actions, for parametrizing some function behaviors */
#define	REVM_HASH_MERGE		(1 << 0)
#define	REVM_HASH_UNMERGE	(1 << 1)
#define	REVM_CREATE_NEWID	(1 << 2)

/* config strings */
#define ERESI_VMCONFIG_ONLOAD_RCONTROL	"onload.restore_control"
#define ERESI_VMCONFIG_GRAPH_STORAGEPATH "graph.storagepath"
#define ERESI_VMCONFIG_GRAPH_VIEWCMD	"graph.viewcmd"
#define ERESI_VMCONFIG_GRAPH_AUTOVIEW	"graph.autoview"
#define ERESI_VMCONFIG_GRAPH_AUTOBUILD	"graph.autobuild"
#define ERESI_VMCONFIG_USE_MORE         "vm.use_more"
#define	ERESI_VMCONFIG_USE_ASMDEBUG	"asm.debug"
#define ERESI_VMCONFIG_HISTORY		"history"

#define ERESI_DEFAULT_HISTORY		".eresi_history"

/* Debug fonctions */
#if __DEBUG_NETWORK__
#define DEBUG_NET(_x)				\
  do						\
    {						\
      _x;					\
    } while(0)
#else
#define DEBUG_NET(_x)
#endif

/* Manage string table */
#define REVM_STRTABLE_GET(_out, _in) 	\
do {					\
  if (_in > strtable_current)		\
    _out = NULL;			\
  else					\
    _out = strtable + _in;		\
} while(0)


/* Take a revmargv_t and fill its argc field */
/* Only used when the command is unknown (module) and tries to be determined */
#define		REVM_CMDARGS_COUNT(cmd)		\
do						\
{						\
  int		len;				\
  for (len = 0; cmd->param[len] != 0; len++)	\
    cmd->argc++;				\
}						\
while (0)


/* Regx option, a module of struct s_args */
typedef struct		s_list
{
  regex_t		name;
  char			*rname;
  u_int			off;
  u_int			size;
  char			otype;
}			revmlist_t;


/* Structure for constants */
typedef struct		s_const
{
  const char	        *desc;
  const char	        *name;
  eresi_Addr	       	val;
}			revmconst_t;


/* ELFsh command handlers */
typedef struct		s_cmdhandler
{
  int			(*reg)(u_int i, u_int s, char **a);	/* Registration handler */
  int			(*exec)();				/* Execution handler */
  char			*arg1;					/* Option activation variable ptr */
  void			*arg2;					/* Option regex ptr */
  char			*arg3;					/* Regex switch ptr */
  char			wflags;					/* 1 if the cmd need a valid curfile */
  char			*help;					/* Help string */
}			revmcmd_t;


/* Thats the command line options registering structure */
typedef struct		s_args
{
  char			*param[REVM_MAXARGC];	/* Parameters */
  char			use_regx[2];		/* 1 if the option use a regx */
  regex_t		regx[2];		/* Regx */
  revmlist_t		disasm[2];		/* D/X parameters */
  char			argc;			/* Number of args in param[] */
  revmcmd_t		*cmd;			/* Command descriptor */
  char			*name;			/* Command name */
  char		        *endlabel;		/* Co-Label for foreach/forend */
#define	REVM_IDX_UNINIT ((unsigned int) (-1))
  u_int		        listidx;		/* Iteration index for this foreach */
  struct s_args		*next;
  struct s_args		*prev;
}			revmargv_t;

#include <libmjollnir.h>

/* REVM module structure */
typedef struct	      s_module
{
  char		      *path;	      /* Name */
#ifdef __BEOS__
  image_id            handler;        /* Object handler */
#else
  void                *handler;       /* Object handler */
#endif
  void                (*help)();      /* Help wrapper */
  void                (*init)();      /* Constructor pointer */
  void                (*fini)();      /* Destructor pointer */
  u_int               id;             /* Object ID */
  time_t              loadtime;       /* Load time stamp */
  struct s_module     *next;          /* Next module of the list */
}                     revmmod_t;


/* This structure contains the control flow context for e2dbg scripts */
typedef struct		s_revmcontext
{
  int			savedfd;
  char			savedmode;
  revmargv_t		*savedcmd;
  char			*(*savedinput)();
  char			**savedargv;
  char			*savedname;
  revmargv_t		*curcmd;
}			revmcontext_t;

/* We use a separate header for the generic IO sublib */
#include "revm-io.h"

/* We use a separate header for defnition of object structures */
#include "revm-objects.h"



/* This structure stores the current FOREACH iteration state for a job */
typedef struct		s_revmiteration
{
#define	REVM_IDX_UNINIT ((unsigned int) (-1))
  u_int			*curindex;	/* Current iteration index (most-nested foreach) */
  char			*curkey;	/* Name (key) of matchme expression in list */
  char			*curname;	/* Name (bound) of induction variable if any */
  list_t		*list;		/* Current list being iterated */
}			revmiter_t;  

/* This structure stores the current REWRITE transformation state for a job */
typedef struct	      s_revmrewrite
{
  revmexpr_t	      *matchexpr;	/* Expression to rewrite */
  u_char	      matched;		/* Matched flag : just 0 or 1 depending on last try */
  u_char	      replaced;		/* Indicate if we have already transformed */
}		      revmrewrite_t;

/* REVM job structure, one per client */
typedef struct        s_job
{
  revmworkspace_t     ws;		/* The job workspace */

  /* Scripting machine job context */
#define		      REVM_MAXSRCNEST  50
  revmargv_t	      *script[REVM_MAXSRCNEST]; /* List of script commands */
  revmargv_t         *lstcmd[REVM_MAXSRCNEST]; /* Last command for each depth */
  u_int               sourced;          /* script depth (if beeing sourced) */

  /* File job context */
  revmargv_t	      *curcmd;          /* Next command to be executed */
  hash_t              loaded;           /* List of loaded ELF objects */
  elfshobj_t          *curfile;         /* Current working ELF object */
  asm_processor*      proc;		/* Processor structure */

  /* Debugger job context */
  hash_t              dbgloaded;        /* List of objects loaded into e2dbg */
  elfshobj_t          *dbgcurrent;      /* Current working e2dbg file */

  /* Job iteration and rewritten expression name if any */
  revmiter_t	      iter;		/* Iteration context */
  revmrewrite_t	      rwrt;		/* Rewrite  context */
}                     revmjob_t;


/* The REVM world */
typedef struct        s_world
{
  revmstate_t         state;          /* Flags structure */
  revmcontext_t       context;        /* Save the VM context before sourcing */
  revmmod_t	      *modlist;       /* ELFsh loaded modules list */
  hash_t	      jobs;           /* Hash table of jobs */
  revmjob_t	      *initial;       /* Main initial job */
  revmjob_t	      *curjob;        /* Current job */
  hash_t	      shared_hash;    /* Hash of shared descriptors */
  char                *scriptsdir;    /* Directory which contains script commands */
  asm_processor       proc;           /* Libasm world */
  asm_processor	      proc_sparc;     /* Libasm Sparc */
  asm_processor	      proc_mips;      /* Libasm Mips */
  mjrsession_t        mjr_session;    /* Session holding contexts for mjollnir */
  int		      fifo_s2c;	      /* Fd for the debugger IO FIFO */
  int		      fifo_c2s;	      /* Fd for the debugger IO FIFO */
  void		      (*cmd_init)();  /* Command constructor from libstderesi */
}		      revmworld_t;


/* The world */
extern revmworld_t	world;

/* All the StandAlone hashtables */
extern list_t		frames_list;     /* List of frames for the ERESI interpreter */
extern hash_t		instrlists_hash; /* Tables of expression lists */
extern hash_t		cmd_hash;	 /* commands handlers */
extern hash_t		parser_hash;	 /* parsers handlers */
extern hash_t		file_hash;	 /* elfshobj_t pointers */
extern hash_t		const_hash;	 /* elf.h picked up constants values */
extern hash_t		redir_hash;	 /* Function redirections hash table */
extern hash_t		mod_hash;	 /* Modules name hash table */
extern hash_t		exprs_hash;	 /* ERESI expressions types hash */ 
extern hash_t		labels_hash[REVM_MAXSRCNEST]; /* Scripting labels hash table */

/* The Level 1 object hash table : hash the object name and returns a L1handler_t* */
extern hash_t		L1_hash;	/* For HDR, SHT, PHT, GOT, DTORS, CTORS, DYNAMIC, SECTIONS */

/* The Level 2 object hash table list : hash the object name and returns a L2handler_t* */
extern hash_t		elf_L2_hash;	/* For the ELF header fields */
extern hash_t		sht_L2_hash;	/* For the Section header table fields */
extern hash_t		pht_L2_hash;	/* For the Program header table fields */
extern hash_t		got_L2_hash;	/* For the Global offset table fields */
extern hash_t		crs_L2_hash;	/* For the .ctors fields */
extern hash_t		drs_L2_hash;	/* For the .dtors fields */
extern hash_t		sym_L2_hash;	/* For symbol fields */
extern hash_t		rel_L2_hash;	/* For Relocation table fields */
extern hash_t		sct_L2_hash;	/* For Section data access */
extern hash_t		dynsym_L2_hash;	/* For .dynsym */
extern hash_t		dyn_L2_hash;	/* For .dynamic */

extern hash_t		vers_L2_hash;   /* For .gnu.version */
extern hash_t		verd_L2_hash;   /* For .gnu.version_d */
extern hash_t		vern_L2_hash;   /* For .gnu.version_r */
extern hash_t		hashb_L2_hash;  /* For .hash (bucket) */
extern hash_t		hashc_L2_hash;  /* For .hash (chain) */

extern hash_t           bg_color_hash; 	/* colors def */
extern hash_t           fg_color_hash; 	/* colors def */
extern hash_t           t_color_hash;  	/* colors type */

//extern hash_t		traces_cmd_hash;/* trace cmd table */
extern hash_t		goto_hash;	/* goto hash */

/* Lattice for I/O */
extern char		*(*hooks_input[REVM_IO_NUM])();
extern int		(*hooks_output[REVM_IO_NUM])(char *buf);

/* This has to be prototyped in libelfsh and not put in extern ! */
extern int 		elfsh_load_core_info(elfshobj_t *);

/* Network related variables */
//extern hash_t      elfsh_net_client_list;  /* The client socket's list */
extern int         elfsh_net_client_count; /* Number of clients connected */
//extern revmsock_t    elfsh_net_serv_sock;    /* The main socket structur */

/* Lib path */
extern char	   elfsh_libpath[BUFSIZ];

/* String table for .elfsh.strings */
extern char	   *strtable;
extern u_int	   strtable_current;
extern u_int	   strtable_max;

/* Prompt storage */
extern void	(*prompt_token_setup)(char *name, u_int size);
extern char	prompt_token[512];

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
revmexpr_t	*revm_lookup_param(char *param);
revmobj_t	*revm_check_object(revmobj_t *pobj);
void		revm_destroy_object(revmobj_t *pobj);
revmobj_t	 *revm_copy_object(revmobj_t *pobj);
elfshobj_t	*revm_getfile(u_int index);
revmmod_t	*revm_getmod(u_int index);
char		*revm_reverse(elfshobj_t *file, u_int vaddr);

/* Lookup functions */
revmobj_t	*revm_lookup_immed(char *param);
revmexpr_t	*revm_lookup_var(char *param);

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
int		revm_command_add(char *cmd, void *exec, void *reg, 
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
u_int		revm_instr_display(int, u_int, eresi_Addr, u_int, u_int,
				 char *, u_int, char *);
int		revm_section_display(elfshsect_t *s, char *name, revmlist_t *re);
int		revm_match_sht(elfshobj_t *file, elfshsect_t *l, revmlist_t *actual);
int		revm_match_symtab(elfshobj_t *file, elfshsect_t *symtab, 
				revmlist_t *actual, int flag);
int		revm_match_special(elfshobj_t *file, eresi_Addr vaddr, revmlist_t*);
int             revm_object_display(elfshsect_t *parent, elfsh_Sym *sym, int size, 
				  u_int off, u_int foffset, eresi_Addr vaddr, 
				  char *name, char otype);
int		revm_hexa_display(elfshsect_t *parent, char *name, eresi_Addr vaddr, u_int index, 
				  u_int size, u_int off, char *buff, u_int foffset);

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
int		dprintf(int fd, char *format, ...);
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
int		revm_config(char *config);
void		revm_postexec(int retval);
void		revm_cleanup();

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
aspectype_t	*revm_fieldoff_get(aspectype_t *par, char *fld, u_int *off);
revmobj_t	*revm_object_lookup_real(aspectype_t *type, char *objname, char *objpath, char trans);
revmobj_t	*revm_object_lookup(char *str);
revmobj_t	*revm_object_create(aspectype_t *type, void *data, char transaddr);

/* Generic handlers for data accesses */
char		*revm_generic_getname(void *type, void *data);
int		revm_generic_setname(void *type, void *data, void *newdata);
eresi_Addr	revm_generic_getobj(void *data);
eresi_Addr	revm_hash_getobj(void *data);
int		revm_byte_setobj(void *data, eresi_Addr value);
int		revm_short_setobj(void *data, eresi_Addr value);
int		revm_int_setobj(void *data, eresi_Addr value);
int		revm_long_setobj(void *data, eresi_Addr value);
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

/* Interface related functions */
int		revm_system(char *cmd);
void		revm_dbgid_set(u_int pid);
u_int		revm_dbgid_get();

/* Atomic operations */
int             revm_preconds_atomics(revmexpr_t **o1, revmexpr_t **o2);
int		revm_nextconds_atomics(revmexpr_t *o1, revmexpr_t *o2);
int		revm_arithmetics(revmexpr_t *e1, revmexpr_t *e2, u_char op);
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
int		revm_workspace_next();

/* libedfmt related functions */
int		revm_edfmt_parse(elfshobj_t *file);
int		revm_edfmt_uni_print(elfshobj_t *file);

/* Type/Inform related functions */
revmexpr_t	*revm_inform_type(char *type, char *name, char *addr, revmexpr_t *e, u_char p, u_char r);
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
int		revm_expr_print(char *pathname);
int		revm_expr_match_by_name(char *original, char *candidate);
int		revm_expr_compare_by_name(char *original, char *candidate, eresi_Addr *val);
int		revm_expr_set_by_name(char *dest, char *source);
aspectype_t	*revm_exprtype_get(char *exprvalue);
revmexpr_t	*revm_expr_create_from_object(revmobj_t *copyme, char *name);
revmexpr_t	*revm_expr_copy(revmexpr_t *source, char *dstname, u_char isfield);
int		revm_expr_destroy(char *e);
revmexpr_t	*revm_expr_lookup(u_int oid);

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

#endif /* __ELFSH_H_ */

