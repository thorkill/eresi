/*
** revm.h for librevm in ERESI
** 
** Started on  Thu Feb 22 07:19:04 2001 jfv
** Moved from elfsh to librevm on January 2007 -may
**
** $Id: revm.h,v 1.80 2007-08-01 14:35:57 may Exp $
*/
#ifndef __REVM_H_
 #define __REVM_H_

/* User defined configuration */
#include "revm-vars.h"

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
#include <sys/ucontext.h>
#include <pthread.h>
#include <stdarg.h> 
#include <regex.h>

#include <libelfsh.h>
#include <libedfmt.h>

#if defined(ELFSHNET)
 #include <libdump.h>
#endif

#if defined(USE_READLN)
 #include <libui.h>
#endif

#if defined(__FreeBSD__)
  extern char **environ;
#endif

#if defined(KERNSH)
#include "libkernsh.h"
#endif

/* Thanks to sk from Devhell Labs we have a libasm */
#include <libasm.h>
 
extern asm_processor	proc;

/* Help strings */
#include "revm-help.h"

/* Now comes DEBUGGING variables for various part of the code */
#define	__DEBUG_DISASM__	0
#define	__DEBUG_SIGHANDLER__	0
#define	__DEBUG_LANG__		0
#define	__DEBUG_SCANNER__	0
#define	__DEBUG_ASLR__		0
#define __DEBUG_NETWORK__	0
#define __DEBUG_RESOLVE__	0
#define __DEBUG_HIJACK__	0
#define __DEBUG_TEST__		0
#define __DEBUG_TRACE__		0
#define	__DEBUG_GRAPH__		0
#define __DEBUG_ARG_COUNT__	0
#define	__DEBUG_EXPRS__		0
#define	__DEBUG_EXPRS_MORE__	0

/* Parsing related defines */
#define	REVM_MAXNEST_LOOP	10

/* Debugger defines that must be in revm and not in the debugger */
#define	E2DBG_NAME		"Embedded ELF Debugger"
#define	E2DBG_ARGV0		"e2dbg"
#define	CMD_CONTINUE		"continue"
#define	CMD_CONTINUE2		"cont"

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
#define	PRINTABLE(c)		(c >= 32 && c <= 126)
#define REGX_IS_USED(a)		a
#define	IS_VADDR(s)		(s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
#define	IS_BLANK(c)		(c == ' ' || c == '\t')

#define	REVM_VAR_ARGC		"#"
#define	REVM_VAR_PREFIX		'$'
#define REVM_VAR_RESULT		"_"
#define	REVM_VAR_LOAD		"!"
#define	REVM_VAR_ERROR		"ERR"
#define	REVM_VAR_SHELL		"SHELL"
#define	REVM_VAR_EDITOR		"EDITOR"
#define REVM_VAR_LIBPATH	"LPATH"
#define REVM_VAR_STRIPLOG	"SLOG"

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

/* Used to store ascii description for different structures types in data.c */
#define ELFSH_SEGTYPE_MAX	7
#define	ELFSH_SHTYPE_MAX	16
#define	ELFSH_OBJTYPE_MAX	5
#define	ELFSH_SYMBIND_MAX	3
#define	ELFSH_SYMTYPE_MAX	STT_NUM + 1
#define	ELFSH_ENCODING_MAX	3
#define	ELFSH_DYNAMIC_MAX	35
#define	ELFSH_EXTDYN_MAX	19
#define ELFSH_MIPSDYN_MAX	43
#define	ELFSH_ARCHTYPE_MAX	56
#define	ELFSH_EXTSEG_MAX	5

#define	ELFSH_RELOC_i386_MAX	11
#define	ELFSH_RELOC_IA64_MAX	81
#define	ELFSH_RELOC_SPARC_MAX	55
#define ELFSH_RELOC_SPARC64_MAX	55
#define	ELFSH_RELOC_ALPHA_MAX	43
#define	ELFSH_RELOC_MIPS_MAX	35

#define ELFSH_RELOC_MAX(file)   revm_getmaxrelnbr(file)

#define	ELFSH_FEATURE_MAX	2
#define	ELFSH_POSFLAG_MAX	2
#define	ELFSH_FLAGS_MAX		4
#define	ELFSH_FLAGS1_MAX	15
#define ELFSH_MIPSFLAGS_MAX	16

/* REVM general parameters */
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

/* Traces directory */
#define	REVM_TRACE_REP		".etrace"

/* For revm_object_display() */
#define	REVM_VIEW_HEX		0
#define	REVM_VIEW_DISASM	1

/* For elfsh/elfsh/modules.c:revm_change_handler() */
#define	ELFSH_ORIG		((void *) -1)

/* For lang/access.c */
#define	REVM_INVALID_FIELD	((u_int) -1)

/* ELFsh actions, for parametrizing some function behaviors */
#define	REVM_HASH_MERGE		(1 << 0)
#define	REVM_HASH_UNMERGE	(1 << 1)
#define	REVM_CREATE_NEWID	(1 << 2)

/* Commands */
#define CMD_DISASM		"disasm"
#define	CMD_DISASM2		"D"
#define	CMD_HEXA		"hexa"
#define	CMD_HEXA2		"X"
#define CMD_REL			"rel"
#define CMD_REL2		"r"
#define CMD_DYNAMIC		"dyn"
#define	CMD_DYNAMIC2		"d"
#define CMD_SYM			"sym"
#define	CMD_SYM2		"syms"
#define CMD_DYNSYM		"dynsym"
#define CMD_DYNSYM2		"ds"
#define CMD_SHT			"sht"
#define CMD_SHT2		"s"
#define CMD_PHT			"pht"
#define CMD_PHT2		"p"
#define CMD_DWARF		"dwarf"
#define CMD_DWARF2		"dw"
#define CMD_ELF			"elf"
#define CMD_ELF2		"e"
#define	CMD_INTERP		"interp"
#define	CMD_INTERP2		"i"
#define	CMD_NOTE		"notes"
#define	CMD_NOTE2		"n"
#define CMD_COREINFO		"coreinfo"
#define	CMD_GOT			"got"
#define	CMD_GOT2		"g"
#define	CMD_CTORS		"ctors"
#define	CMD_CTORS2		"ct"
#define	CMD_DTORS		"dtors"
#define	CMD_DTORS2		"dt"
#define	CMD_SHTRM		"shtrm"
#define	CMD_COMMENT		"comments"
#define	CMD_COMMENT2		"c"
#define CMD_BINFILE_R		"f"
#define	CMD_BINFILE_W		"w"
#define	CMD_SET			"set"
#define	CMD_PRINT		"print"
#define	CMD_EXEC		"exec"
#define	CMD_ADD			"add"
#define	CMD_SUB			"sub"
#define	CMD_MUL			"mul"
#define	CMD_DIV			"div"
#define	CMD_TEST		"test"
#define	CMD_MOD			"mod"
#define	CMD_INFO		"info"
#define	CMD_METACMD		"!"
#define	CMD_WRITE		"write"
#define	CMD_APPEND		"append"
#define	CMD_EXTEND		"extend"
#define	CMD_FIXUP		"fixup"
#define	CMD_FINDREL		"findrel"
#define	CMD_MODLOAD		"modload"
#define	CMD_MODULOAD		"modunload"
#define CMD_MODHELP		"modhelp"
#define	CMD_HELP		"help"
#define	CMD_RELINJCT		"reladd"
#define	CMD_STOP		"stop"
#define	CMD_HIJACK		"redir"
#define CMD_COLOR               "setcolor"
#define CMD_NOCOLOR             "nocolor"
#define CMD_TRACES		"traces"
#define CMD_TRACEADD		"traceadd"
#define CMD_TRACEEXCLUDE	"traceexclude"
#define CMD_TRACERUN		"tracerun"
#define CMD_TRACEADD_CMDLINE	"t"
#define CMD_TRACEEXCL_CMDLINE	"u"
#define	CMD_TYPE		"type"
#define	CMD_TYPEDEF		"typedef"

#define CMD_INSERT		"insert"
#define	CMD_INSERT2		"ins"
#define	CMD_REMOVE		"remove"
#define	CMD_REMOVE2		"rm"
#define	CMD_FLUSH		"flush"
#define	CMD_VLIST		"vlist"
#define	CMD_SOURCE		"source"
#define CMD_SDIR		"sdir"
#define	CMD_CLEANUP		"cleanup"
#define CMD_LSCRIPTS		"lscripts"
#define CMD_CAT			"cat"
#define CMD_PROFILE		"profile"
#define	CMD_LOG			"log"
#define	CMD_EXPORT		"export"
#define	CMD_EDIT		"edit"

/* Type related commands */
#define	CMD_INFORM		"inform"
#define	CMD_UNINFORM		"uninform"
#define	CMD_INFORM2		"annotate"
#define	CMD_UNINFORM2		"unannotate"

/* Debugging format commands */
#define CMD_DEBUG		"debug"

/* ELF Version commands */
#define CMD_VERSION		"version"
#define CMD_VERNEED		"verneed"
#define CMD_VERDEF		"verdef"
#define	CMD_HASH		"hash"

/* Network commands */
#define	CMD_NETWORK		"net"
#define	CMD_NETWORK2		"network"
#define	CMD_NETLIST		"netlist"
#define	CMD_NETKILL		"netkill"
#define	CMD_SHARED		"shared"
#define	CMD_CONNECT		"connect"
#define	CMD_DISCON		"disconnect"
#define	CMD_PEERSLIST		"peerslist"
#define CMD_RCMD		"rcmd"

/* Scripting only commands */
#define CMD_CMP			"cmp"
#define	CMD_CMP2		"compare"
#define	CMD_JMP			"jmp"
#define CMD_JE			"je"
#define	CMD_JNE			"jne"
#define	CMD_JL			"jl"
#define	CMD_JG			"jg"
#define	CMD_JLE			"jle"
#define	CMD_JGE			"jge"
#define	CMD_FOREACH		"foreach"
#define	CMD_FOREND		"forend"
#define	CMD_MATCH		"rewrite"
#define	CMD_PARAM_INTO		"into"
#define	CMD_MATCHEND		"endrewrite"
#define	CMD_CASE		"case"
#define	CMD_REFLECT		"reflect"
#define	CMD_DEFAULT		"default"

/* Prefixes */
#define	CMD_SORT		 "sort"
#define	CMD_SORT2		 "sr"
#define	CMD_QUIET		 "q"
#define	CMD_QUIET2		 "quiet"
#define	CMD_VERB		 "verb"
#define	CMD_VERB2		 "v"
#define CMD_ALL			 "all"
#define CMD_ALL2		 "a"
#define	CMD_ALERT		 "alert"
#define	CMD_FORCE		 "force"

/* Interactive only command */
#define	CMD_LOAD		 "load"
#define	CMD_UNLOAD		 "unload"
#define	CMD_SAVE		 "save"
#define	CMD_QUIT		 "quit"
#define	CMD_QUIT2		 "exit"
#define	CMD_SWITCH		 "switch"
#define	CMD_LIST		 "list"
#define	CMD_LIST2		 "l"
#define	CMD_WORKSPACE		 "workspace"
#define	CMD_WORKSPACE2		 "w"
#define	CMD_VECTORS		 "vectors"
#define	CMD_TABLES		 "tables"
#define	CMD_LISTS		 "lists"
#define	CMD_EMPTY		 "empty"

/* Code analysis commands */
#define CMD_FLOWJACK		"flowjack"
#define	CMD_GRAPH		"graph"
#define CMD_INSPECT		"inspect"
#define CMD_ADDGOTO		"addgoto"
#define	CMD_SETGVL		"setgvl"
#define CMD_UNSTRIP		"unstrip"
#define CMD_RENAME		"rename"
#define	CMD_CONTROL		"control"
#define CMD_ANALYSE		"analyse"

/* config commands */
#define CMD_CONFIGURE		"configure"

/* config strings */
#define ELFSH_VMCONFIG_ONLOAD_RCONTROL	"onload.restore_control"
#define ELFSH_VMCONFIG_GRAPH_STORAGEPATH "graph.storagepath"
#define ELFSH_VMCONFIG_GRAPH_VIEWCMD	"graph.viewcmd"
#define ELFSH_VMCONFIG_GRAPH_AUTOVIEW	"graph.autoview"
#define ELFSH_VMCONFIG_GRAPH_AUTOBUILD	"graph.autobuild"
#define ELFSH_VMCONFIG_USE_MORE         "vm.use_more"
#define	ELFSH_VMCONFIG_USE_ASMDEBUG	"asm.debug"

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
  elfsh_Addr	       	val;
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
  char			*param[128];	/* option parameters */
  char			use_regx[2];	/* 1 if the option use a regx */
  regex_t		regx[2];	/* regx */
  revmlist_t		disasm[2];	/* D/X parameters */
  char			argc;		/* Number of args in param[] */
  revmcmd_t		*cmd;		/* Command descriptor */
  char			*name;		/* Command name */
  char		        *endlabel;	/* Co-Label for foreach/forend */
#define	REVM_IDX_UNINIT ((unsigned int) (-1))
  u_int		        listidx;       /* Iteration index for this foreach */
  struct s_args		*next;
  struct s_args		*prev;
}			revmargv_t;


/* Trace structures, used by the tracer */
typedef struct		s_revmtraces
{
  int			(*exec)(elfshobj_t*, char *, char **);	/* Function used */

  /* Unexistant (0), optional (1), needed (2) */
  char			flagName;	/* Need a first argument */
  char			flagArg;       	/* Need a second argument */
}			revmtraces_t;



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


/* REVM job structure, one per client */
typedef struct        s_job
{
  revmworkspace_t     ws;		/* The job workspace */

  /* Scripting machine job context */
#define		      REVM_MAXSRCNEST  10
  revmargv_t	      *script[REVM_MAXSRCNEST]; /* List of script commands */
  revmargv_t         *lstcmd[REVM_MAXSRCNEST]; /* Last command for each depth */
  u_int               sourced;          /* script depth (if beeing sourced) */

  /* File job context */
  revmargv_t	      *curcmd;          /* Next command to be executed */
  hash_t              loaded;           /* List of loaded ELF objects */
  elfshobj_t          *current;         /* Current working ELF object */
  asm_processor*      proc;		/* Processor structure */

  /* Debugger job context */
  hash_t              dbgloaded;        /* List of objects loaded into e2dbg */
  elfshobj_t          *dbgcurrent;      /* Current working e2dbg file */

  /* Job context in a rewrite construct */
#define	REVM_IDX_UNINIT ((unsigned int) (-1))
  u_int		      *curlistidx;      /* Current iteration index (most-nested foreach) */
  list_t	      *itlist;		/* Current list being iterated */
  revmexpr_t	      *matchexpr;	/* Expression to match if inside a rewrite construct */
  char		      *matchexprname;	/* Name of matchme expression */
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
  mjrsession_t        mjr_session;    /* Session holding contexts for mjollnir */
  int		      fifo_s2c;	      /* Fd for the debugger IO FIFO */
  int		      fifo_c2s;	      /* Fd for the debugger IO FIFO */
}		      revmworld_t;


/* The world */
extern revmworld_t	world;

/* All the StandAlone hashtables */
extern hash_t		cmd_hash;	 /* commands handlers */
extern hash_t		parser_hash;	 /* parsers handlers */
extern hash_t		file_hash;	 /* elfshobj_t pointers */
extern hash_t		const_hash;	 /* elf.h picked up constants values */
extern hash_t		redir_hash;	 /* Function redirections hash table */
extern hash_t		mod_hash;	 /* Modules name hash table */
extern hash_t		vars_hash;	 /* Scripting variables hash table */
extern hash_t		exprs_hash;	 /* ERESI expressions types hash */ 
extern hash_t		exprtypes_hash;	 /* Types of expressions */
extern hash_t		labels_hash[10]; /* Scripting labels hash table */

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

extern hash_t		traces_cmd_hash;/* trace cmd table */
extern hash_t		goto_hash;	/* goto hash */

/* Lattice for I/O */
extern char		*(*hooks_input[REVM_IO_NUM])();
extern int		(*hooks_output[REVM_IO_NUM])(char *buf);

/* Data value/string/description arrays */
extern revmconst_t     elfsh_extseg_type[ELFSH_EXTSEG_MAX];
extern revmconst_t     elfsh_seg_type[ELFSH_SEGTYPE_MAX];
extern revmconst_t     elfsh_sh_type[ELFSH_SHTYPE_MAX];
extern revmconst_t     elfsh_obj_type[ELFSH_OBJTYPE_MAX];
extern revmconst_t     elfsh_sym_bind[ELFSH_SYMBIND_MAX];
extern revmconst_t     elfsh_sym_type[ELFSH_SYMTYPE_MAX];
extern revmconst_t     elfsh_dynentry_type[ELFSH_DYNAMIC_MAX];
extern revmconst_t     elfsh_encoding[ELFSH_ENCODING_MAX];
extern revmconst_t     elfsh_extdyn_type[ELFSH_EXTDYN_MAX];
extern revmconst_t     elfsh_mipsdyn_type[ELFSH_MIPSDYN_MAX];
extern char	       *elfsh_arch_type[ELFSH_ARCHTYPE_MAX];
extern revmconst_t     elfsh_feature1[ELFSH_FEATURE_MAX];
extern revmconst_t     elfsh_posflag1[ELFSH_POSFLAG_MAX];
extern revmconst_t     elfsh_flags[ELFSH_FLAGS_MAX];
extern revmconst_t     elfsh_flags1[ELFSH_FLAGS1_MAX];
extern revmconst_t     elfsh_mipsflags[ELFSH_MIPSFLAGS_MAX];
extern revmconst_t     elfsh_rel_type_i386[ELFSH_RELOC_i386_MAX];
extern revmconst_t     elfsh_rel_type_ia64[ELFSH_RELOC_IA64_MAX];
extern revmconst_t     elfsh_rel_type_sparc[ELFSH_RELOC_SPARC64_MAX];
extern revmconst_t     elfsh_rel_type_alpha[ELFSH_RELOC_ALPHA_MAX];
extern revmconst_t     elfsh_rel_type_mips[ELFSH_RELOC_MIPS_MAX];
extern revmconst_t     elfsh_verentry_type[ELFSH_VERENTRY_MAX];

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

/* Commands execution handlers, each in their respective file */
int		cmd_configure();
int		cmd_type();
int		cmd_declare();
int		cmd_typedef();
int		cmd_dyn();
int		cmd_sht();
int             cmd_rsht();
int             cmd_rpht();
int		cmd_rel();
int		cmd_dynsym();
int		cmd_symtab();
int		cmd_pht();
int		cmd_got();
int		cmd_dtors();
int		cmd_ctors();
int		cmd_elf();
int		cmd_interp();
int		cmd_list();
int		cmd_notes();
int		cmd_coreinfo();
int		cmd_sym();
int		cmd_hexa();
int		cmd_disasm();
int		cmd_shtrm();
int		cmd_comments();
int		cmd_modhelp();
int		cmd_help();
int		cmd_quit();
int		cmd_load();
int		cmd_unload();
int		cmd_save();
int		cmd_dolist();
int		cmd_doswitch();
int		cmd_set();
int		cmd_get();
int		cmd_print();
int		cmd_info();
int		cmd_add();
int		cmd_sub();
int		cmd_mul();
int		cmd_div();
int		cmd_mod();
int		cmd_cmp();
int		cmd_test();
int		cmd_meta();
int		cmd_write();
int		cmd_append();
int		cmd_extend();
int		cmd_fixup();
int		cmd_quiet();
int             cmd_force();
int		cmd_verb();
int		cmd_exec();
int		cmd_findrel();
int		cmd_modload();
int		cmd_modunload();
int		cmd_relinject();
int		cmd_stop();
int		cmd_hijack();
int		cmd_insert();
int		cmd_remove();
int		cmd_sort();
int		cmd_glregx();
int		cmd_alert();
int		cmd_flush();
int		cmd_vlist();
int		cmd_source();
int		cmd_scriptsdir();
int		cmd_script();
int		cmd_lscripts();
int		cmd_cat();
int             cmd_color();
int             cmd_nocolor();
int		cmd_phtend();
int		cmd_network();
int		cmd_netlist();
int		cmd_netkill();
int		cmd_discon();
int		cmd_connect();
int		cmd_peerslist();
int		cmd_rcmd();
int		cmd_profile();
int		cmd_log();
int		cmd_export();  
int		cmd_edit();
int		cmd_traces();
int		cmd_traceadd();
int		cmd_traceexclude();
int		cmd_tracerun();
int		cmd_shared();
int             cmd_verneed();
int             cmd_verdef();
int             cmd_version();
int             cmd_hashx();
int		cmd_vectors();
int		cmd_tables();
int		cmd_lists();
int		cmd_empty();
int		cmd_inform();
int		cmd_uninform();

/* Scripting only commands */
int		cmd_jmp();
int		cmd_je();
int		cmd_jne();
int		cmd_jg();
int		cmd_jl();
int		cmd_jge();
int		cmd_jle();
int		cmd_foreach();
int		cmd_forend();
int		cmd_match();
int		cmd_matchend();
int		cmd_case();
int		cmd_reflect();
int		cmd_default();

/* Flow analysis commands */
int		cmd_flowload(void);
int		cmd_flowsave(void);
int		cmd_graph(void);
int		cmd_inspect(void);
int		cmd_flow(void);
int		cmd_testflow(void);
int		cmd_flowjack(void);
int		cmd_addgoto(void);
int		cmd_setgvl(void);
int		cmd_analyse();
int		cmd_unstrip();
int 		cmd_rename();
int		cmd_control();

/* Workspaces commands */
int		cmd_workspace();
int		cmd_next_workspace();

/* Debugging functions */
int		cmd_mode();
int		cmd_linkmap();
int		cmd_bt();
int		cmd_bp();
int		cmd_watch();
int		cmd_stack();
int		cmd_dbgstack();
int		cmd_delete();
int		cmd_step();
int		cmd_display();
int		cmd_undisplay();

/* Debug format functions */
int		cmd_debug();

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

/* Libasm resolve handlers */
void		asm_do_resolve(void *data, elfsh_Addr vaddr, char *, u_int);
char		*revm_resolve(elfshobj_t *file, elfsh_Addr addr, elfsh_SAddr *roff);

/* General VM functions */
revmobj_t	*revm_lookup_param(char *param);
revmobj_t	*revm_check_object(revmobj_t *pobj);
void		revm_destroy_object(revmobj_t *pobj);
revmobj_t	 *revm_copy_object(revmobj_t *pobj);
elfshobj_t	*revm_getfile(u_int index);
revmmod_t	*revm_getmod(u_int index);
char		*revm_reverse(elfshobj_t *file, u_int vaddr);

/* Lookup functions */
revmobj_t	*revm_lookup_immed(char *param);
revmobj_t	*revm_lookup_var(char *param);
elfshobj_t	*revm_lookup_file(char *param);
u_int		revm_lookup_index(char *param);
char		*revm_lookup_string(char *param);
elfsh_Addr	revm_lookup_addr(char *param);

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
revmobj_t	*parse_list(char *param, char *fmt);

/* Versions functions */
int             revm_version_pdef(hashdef_t *p, u_int ai, u_int i, char *id, 
				char *n, char *t, regex_t *r);
int             revm_version_pneed(hashneed_t *p, u_int ai, u_int i, char *id, 
				 char *n, char *t, regex_t *r);
int             revm_version_unk(u_int ai, u_int i, char *id, char *n, char *t);

/* Disassembling and hexadecimal view functions */
u_int		revm_instr_display(int, u_int, elfsh_Addr, u_int, u_int,
				 char *, u_int, char *);
int		revm_section_display(elfshsect_t *s, char *name, revmlist_t *re);
int		revm_match_sht(elfshobj_t *file, elfshsect_t *l, revmlist_t *actual);
int		revm_match_symtab(elfshobj_t *file, elfshsect_t *symtab, 
				revmlist_t *actual, int flag);
int		revm_match_special(elfshobj_t *file, elfsh_Addr vaddr, revmlist_t*);
int             revm_object_display(elfshsect_t *parent, elfsh_Sym *sym, int size, 
				  u_int off, u_int foffset, elfsh_Addr vaddr, 
				  char *name, char otype);

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

/* Trace functions */
int		revm_traces_add_arguments(int argc, char **argv);
edfmtfunc_t 	*revm_traces_tracable_with_type(elfshobj_t *file, char *func_name, u_char external);
elfshtraces_t  	*revm_traces_createargs(elfshobj_t *file, char *name,
					 edfmtfunc_t *func, elfsh_Addr vaddr,
					 u_char external);
FILE	      	*revm_traces_init(char *tfname, char *rsofname, char *rtfname);
int	       	revm_traces_add(FILE *fp, int *argcount, char *func_name);
int 	       	traces_addcmd(char *cmd, void *exec, char flagName, char flagArg);
int	       	traces_add(elfshobj_t *file, char *name, char **optarg);
int	       	traces_rm(elfshobj_t *file, char *name, char **optarg);
int	       	traces_exclude(elfshobj_t *file, char *freg, char **oreg);
int		traces_rmexclude(elfshobj_t *file, char *freg, char **oreg);
int	      	traces_enable(elfshobj_t *file, char *name, char **optarg);
int	       	traces_disable(elfshobj_t *file, char *name, char **optarg);
int	       	traces_create(elfshobj_t *file, char *name, char **optarg);
int	       	traces_delete(elfshobj_t *file, char *name, char **optarg);
int	       	traces_flush(elfshobj_t *file, char *name, char **optarg);
int	       	traces_list(elfshobj_t *file, char *name, char **optarg);
int		traces_run(elfshobj_t *file, char **argv, int argc);

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
void	        revm_pht_print(elfsh_Phdr *phdr, uint16_t num, elfsh_Addr base);
char		*revm_fetch_sht_typedesc(elfsh_Word typenum);
int             revm_sht_print(elfsh_Shdr *shdr, u_int num, char rtflag);
int		revm_load_init_dephash(elfshobj_t *file, char *name);
int		revm_file_load(char *name, elfsh_Addr base, elfshlinkmap_t *lm);
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

/* Vector related functions */
int		revm_vectors_getdims(char *str, unsigned int *dims);
char		*revm_ascii_vtype(vector_t *cur);
int		revm_vectors_getdimnbr(char *str);
int		revm_vector_bad_dims(vector_t *v, unsigned int *dims, u_int dimnbr);

/* Dependences related information : deps.c */
int		revm_load_enumdep(elfshobj_t *obj);
int		revm_load_dep(elfshobj_t *p, char *n, elfsh_Addr b, elfshlinkmap_t *);
char	     	*revm_load_searchlib(char *name);
elfshobj_t	*revm_is_dep(elfshobj_t *obj, char *path);
elfshobj_t	*revm_is_depid(elfshobj_t *obj, int id);
int		revm_unload_dep(elfshobj_t *obj, elfshobj_t *root);

/* Top skeleton functions */
int		revm_init() __attribute__((constructor));
int		revm_loop(int argc, char **argv);
int		revm_setup(int ac, char **av, char mode, char side);
int		revm_run(int ac, char **av);
int		revm_config();

/* Scripting flow functions */
int		revm_execscript();
int		revm_execmd();
int		revm_move_pc(char *idx);
int		revm_openscript(char **av);
int		revm_testscript(int ac, char **av);
int		revm_exec_str(char *str);
int	        revm_restore_dbgcontext(int, char, revmargv_t*, void*, char**, char*);

/* Variable functions */
int             revm_setvar_str(char *varname, char *value);
int             revm_setvar_raw(char *varname, char *value, u_int len);
int             revm_setvar_byte(char *varname, u_char byte);
int             revm_setvar_short(char *varname, u_short val);
int             revm_setvar_int(char *varname, u_int val);
int             revm_setvar_long(char *varname, u_long val);

/* Type related functions */
int		revm_types_print();
int		revm_type_print(char *type, char mode);
int		revm_type_copy(char *from, char *to);
int		revm_type_hashcreate(char *name);

/* Data access related functions */
aspectype_t	*revm_fieldoff_get(aspectype_t *par, char *fld, u_int *off);
revmobj_t	*revm_object_lookup_real(aspectype_t *type, char *objname, char *objpath);
revmobj_t	*revm_object_lookup(char *str);
revmobj_t	*revm_object_create(aspectype_t *type, void *data);
char		*revm_generic_getname(void *type, void *data);
int		revm_generic_setname(void *type, void *data, void *newdata);
elfsh_Addr	revm_generic_getobj(void *data);
elfsh_Addr	revm_hash_getobj(void *data);
int		revm_byte_setobj(void *data, elfsh_Addr value);
int		revm_short_setobj(void *data, elfsh_Addr value);
int		revm_int_setobj(void *data, elfsh_Addr value);
int		revm_long_setobj(void *data, elfsh_Addr value);
char		*revm_generic_getdata(void *data, int off, int sizelm);
int		revm_generic_setdata(void *d, int off, void *ndat, int sz, int szlm);

/* Object creation/verification functions */
int		revm_convert_object(revmobj_t *obj, u_int objtype);
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
				 elfsh_Addr saddr, elfsh_Addr daddr);
revmobj_t	*revm_create_IMMED(char type, char perm, u_int val);
revmobj_t	*revm_create_IMMEDSTR(char perm, char *str);
revmobj_t	*revm_create_LONG(char perm, elfsh_Addr val);
revmobj_t	*revm_create_CADDR(char perm, elfsh_Addr val);
revmobj_t	*revm_create_DADDR(char perm, elfsh_Addr val);

/* Interface related functions */
int		revm_system(char *cmd);
void		revm_dbgid_set(u_int pid);
u_int		revm_dbgid_get();

/* Atomic operations */
int             revm_preconds_atomics(revmobj_t **o1, revmobj_t **o2);
int		revm_arithmetics(revmobj_t *o1, revmobj_t *o2, u_char op);
int		revm_hash_add(hash_t *h, revmobj_t *o);
int		revm_hash_del(hash_t *h, revmobj_t *o);
int		revm_list_add(list_t *h, revmobj_t *o);
int		revm_list_del(list_t *h, revmobj_t *o);
int		revm_hash_set(char *tab, char *elm, void *obj, u_int type);
int		revm_list_set(char *tab, char *elm, void *obj, u_int type);
int		revm_testbit(revmobj_t *o1, revmobj_t *o2, u_int *result);
int		revm_object_compare(revmobj_t *o1, revmobj_t *o2, elfsh_Addr *val);
int		revm_object_set(revmobj_t *o1, revmobj_t *o2);

/* Job related functions */
int		revm_own_job(revmjob_t *job);
int		revm_valid_workspace(char *name);
void		revm_switch_job(revmjob_t *job);
revmjob_t	*revm_clone_job(char *name, revmjob_t *job);
int		revm_add_script_cmd(char *dirstr);
revmjob_t	*revm_localjob_get();
revmjob_t	*revm_socket_add(int socket, struct sockaddr_in *addr);
int              revm_screen_switch();
int              revm_screen_clear(int i, char c);
int              revm_screen_update(u_short isnew, u_short prompt_display);

/* libedfmt related functions */
int		revm_edfmt_parse(elfshobj_t *file);
int		revm_edfmt_uni_print(elfshobj_t *file);

/* Inform related functions */
int		revm_inform_type(char *type, char *name, char *addr, revmexpr_t *e, u_char p, u_char r);
int		revm_inform_type_addr(char *t, char *n, elfsh_Addr a, revmexpr_t *, u_char p, u_char r);
int		revm_check_addr(elfshobj_t *obj, elfsh_Addr add);

/* Expression related functions */
revmexpr_t	*revm_expr_create(aspectype_t *type, char *name, char *val);
revmexpr_t	*revm_expr_get(char *pathname);
int		revm_expr_compare(revmexpr_t *orig, revmexpr_t *candid);
int		revm_expr_match(revmexpr_t *candid, revmexpr_t *orig);
int		revm_expr_set(revmexpr_t *adst, revmexpr_t *asrc);
int		revm_expr_print(char *pathname);
int		revm_expr_match_by_name(char *original, char *candidate);
int		revm_expr_compare_by_name(char *original, char *candidate);
int		revm_expr_set_by_name(char *dest, char *source);
aspectype_t	*revm_exprtype_get(char *exprvalue);


/* May not be defined */
#if __BSD__
extern int vsscanf(const char * restrict str, const char * restrict format,
		     va_list ap);
#elif defined(__linux__)
extern int vsscanf (__const char *__restrict __s,
                    __const char *__restrict __format, _G_va_list __arg);
#endif


void		wait4exit(void *);

#endif /* __ELFSH_H_ */

