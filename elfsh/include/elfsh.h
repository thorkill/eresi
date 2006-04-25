/*
** elfsh.h for elfsh
** 
** Started on  Thu Feb 22 07:19:04 2001 mayhem
*/
#ifndef __ELFSH_H_
 #define __ELFSH_H_

/* User defined configuration */
#include "elfsh-vars.h"

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
#include <regex.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#define __USE_GNU
#include <sys/ucontext.h>

#ifdef  __BEOS__
#include <image.h>
#include <bsd_mem.h>
#else
#include <dlfcn.h>
#endif

#include <pthread.h>

#include <libelfsh.h>

#if defined(ELFSHNET)
 #include <libdump.h>
#endif

/* Thanks to sk from Devhell Labs we have a libasm */
#include <libasm.h>
 
extern asm_processor	proc;

/* Thanks to GNU we have readline */
#if defined(USE_READLN)
 #include <readline/readline.h>
 #include <readline/history.h>
#endif

/* Help strings */
#include "elfsh-help.h"

/* Now comes DEBUGGING variables for various part of the code */
#define	__DEBUG_DISASM__	0
#define	__DEBUG_SIGHANDLER__	0
#define	__DEBUG_MODEL__		0
#define	__DEBUG_SCANNER__	0
#define	__DEBUG_ASLR__		0
#define __DEBUG_NETWORK__	0
#define __DEBUG_RESOLVE__	0
#define __DEBUG_HIJACK__	0
#define __DEBUG_TEST__		0

/* Network related defines */
#define ELFSH_PORT		4444
#define ELFSH_DUMP_PORT		4445
#define MAX_CLIENTS		4
#define MAX_SEND_SIZE		2048

/* DUMP related defines */
#define ELFSH_DUMP_CMD		1
#define ELFSH_DUMP_MSG		2

/* General usage macros */
#define FATAL(a)		{ perror(a); vm_exit(-1);		      }
#define QUIT_ERROR(a)		{ vm_exit(a);				      }
#define RET(a)			ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, a)
#define RETERR(a)		ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, a, -1)
#define	PERROR_RET(a, b)	{ perror(a); RETERR (b);		      }
#define	PRINTABLE(c)		(c >= 32 && c <= 126)
#define REGX_IS_USED(a)		a
#define	IS_VADDR(s)		(s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
#define	IS_BLANK(c)		(c == ' ' || c == '\t')

#define	ELFSH_ARGVAR		"#"
#define	ELFSH_VARPREF		'$'
#define ELFSH_RESVAR		"_"
#define	ELFSH_LOADVAR		"!"
#define	ELFSH_ERRVAR		"ERR"
#define	ELFSH_SHELLVAR		"SHELL"
#define	ELFSH_EDITVAR		"EDITOR"

/* ELF Versions */
#define ELFSH_VERTYPE_NONE    0
#define ELFSH_VERTYPE_UNK     1
#define ELFSH_VERTYPE_NEED    2
#define ELFSH_VERTYPE_DEF     3
#define ELFSH_VERENTRY_MAX    4

/* Some useful macros */
#define	CHOOSE_REGX(r, idx)  r = (world.curjob->curcmd->use_regx[idx] ?       \
			     &world.curjob->curcmd->regx[idx] :               \
			     world.state.vm_use_regx ? &world.state.vm_regx : \
			     NULL)
#define	FIRSTREGX(r)	     CHOOSE_REGX(r, 0)
#define	SECONDREGX(r)	     CHOOSE_REGX(r, 1)


/* 
** Macros for declaring commands inside modules
** Necessary for beeing able to use module commands
** in script without loading the module during the
** entire script
*/
#define	ELFSH_CMDEC(name)	char modcmd_##name = name

/* Used to store ascii description for different structures types in data.c */
#define ELFSH_SEGTYPE_MAX	7
#define	ELFSH_SHTYPE_MAX	13
#define	ELFSH_OBJTYPE_MAX	5
#define	ELFSH_SYMBIND_MAX	3
#define	ELFSH_SYMTYPE_MAX	STT_NUM + 1
#define	ELFSH_ENCODING_MAX	3
#define	ELFSH_DYNAMIC_MAX	35
#define	ELFSH_EXTDYN_MAX	19
#define ELFSH_MIPSDYN_MAX	43
#define	ELFSH_ARCHTYPE_MAX	56
#define	ELFSH_STAB_MAX		256
#define	ELFSH_EXTSEG_MAX	5


#define	ELFSH_RELOC_i386_MAX	11
#define	ELFSH_RELOC_IA64_MAX	81
#define	ELFSH_RELOC_SPARC_MAX	55
#define ELFSH_RELOC_SPARC64_MAX	55
#define	ELFSH_RELOC_ALPHA_MAX	43
#define	ELFSH_RELOC_MIPS_MAX	35


#define ELFSH_RELOC_MAX(file)   vm_getmaxrelnbr(file)

#define	ELFSH_FEATURE_MAX	2
#define	ELFSH_POSFLAG_MAX	2
#define	ELFSH_FLAGS_MAX		4
#define	ELFSH_FLAGS1_MAX	15
#define ELFSH_MIPSFLAGS_MAX	16

/* ELFsh general parameters */
#define	ELFSH_FIELD_SEP		"."
#define	ELFSH_COMMENT_START	'#'
#define ELFSH_MINUS		'-'
#define	ELFSH_SLASH		'/'
#define	ELFSH_SPACE		' '

char prompt_token[128];
#define ELFSH_SNAME		"elfsh"
#define	ELFSH_VERSION		"0.7"
#define	ELFSH_RELEASE		"a7p3rc2"
#define ELFSH_EDITION		"brz"

/* Unused, feel free to try it, its awesome */
#define ELFSH_CIRCUS_PROMPT	"\033[00;01;30m(" \
				"\033[00;01;31me" \
				"\033[00;01;32ml" \
				"\033[00;01;33mf" \
				"\033[00;01;34ms" \
				"\033[00;01;35mh" \
				"\033[00;01;36m-" \
				"\033[00;01;30m"  \
				ELFSH_VERSION     \
				"\033[00;01;35m-" \
				"\033[05;01;35mcircus"  \
				"\033[00;01;35m-edition"\
				"\033[00;01;30m)"       \
				"\033[00m " 

/* The ELF shell crew prompt */
#define ELFSH_CREW_PROMPT	"\033[00;01;30m("         \
				"\033[00;01;31melfsh"     \
				"\033[00;01;30m-"         \
				"\033[00;01;33m"          \
				ELFSH_VERSION              \
				"\033[00;01;30m-"         \
				"\033[00;01;33m"          \
				ELFSH_RELEASE              \
				"\033[00;01;30m-"         \
				"\033[00;01;32mdevhell"   \
				"\033[00;01;30m)"         \
				"\033[00m " 

/* Be original, do your own */
//#define	  ELFSH_PROMPT ELFSH_CREW_PROMPT
#define ELFSH_PROMPT ELFSH_CIRCUS_PROMPT


#define ELFSH_NAME		"ELF shell"

#define	ELFSH_INIT		"elfsh_init"
#define	ELFSH_FINI		"elfsh_fini"
#define ELFSH_HELP		"elfsh_help"

/* For elfsh/elfsh/disasm.c:display_object() */
#define	ELFSH_HEXA_VIEW		0
#define	ELFSH_DISASM_VIEW	1

/* For elfsh/elfsh/modules.c:vm_change_handler() */
#define	ELFSH_ORIG		((void *) -1)

/* Return of an input function in case of ignorable input */
#define ELFSH_VOID_INPUT -1
#define ELFSH_EXIT_INPUT -2

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
#define CMD_STAB		"stab"
#define CMD_STAB2		"st"
#define CMD_DWARF		"dwarf"
#define CMD_DWARF2		"dw"
#define CMD_ELF			"elf"
#define CMD_ELF2		"e"
#define	CMD_INTERP		"interp"
#define	CMD_INTERP2		"i"
#define	CMD_NOTE		"notes"
#define	CMD_NOTE2		"n"
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
#define	CMD_GET			"get"
#define	CMD_PRINT		"print"
#define	CMD_EXEC		"exec"
#define	CMD_ADD			"add"
#define	CMD_SUB			"sub"
#define	CMD_MUL			"mul"
#define	CMD_DIV			"div"
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
#define	CMD_STRIP		"strip"
#define	CMD_SSTRIP		"sstrip"
#define	CMD_RELINJCT		"reladd"
#define	CMD_STOP		"stop"
#define	CMD_HIJACK		"redir"
#define CMD_COLOR               "setcolor"
#define CMD_NOCOLOR             "nocolor"

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

#ifdef __DEBUG_TEST__
#define CMD_TEST		 "test"
#endif

/* Regx option, a module of struct s_args */
typedef struct		s_list
{
  regex_t		name;
  char			*rname;
  u_int			off;
  u_int			size;
  char			otype;
}			elfshlist_t;


/* Structure for constants */
typedef struct		s_const
{
  const char	        *desc;
  const char	        *name;
  elfsh_Addr	       	val;
}			elfshconst_t;


/* ELFsh command handlers */
typedef struct	s_cmdhandler
{
  int		(*reg)(u_int i, u_int s, char **a);	/* Registration handler */
  int		(*exec)();				/* Execution handler */
  char		*arg1;					/* Option activation variable ptr */
  void		*arg2;					/* Option regex ptr */
  char		*arg3;					/* Regex switch ptr */
  char		wflags;					/* 1 if the cmd need a valid curfile */
  char		*help;					/* Help string */
}		elfshcmd_t;


/* Thats the command line options registering structure */
typedef struct		s_args
{
  char			*param[10];	/* option parameters */
  char			use_regx[2];	/* 1 if the option use a regx */
  regex_t		regx[2];	/* regx */
  elfshlist_t		disasm[2];	/* D/X parameters */
  char			argc;		/* Number of args in param[] */
  elfshcmd_t		*cmd;		/* Command descriptor */
  char			*name;		/* Command name */
  struct s_args		*next;
  struct s_args		*prev;
}			elfshargv_t;

/* Take a elfshargv_t and fill its argc field */
#define		ELFSH_CMDARGS_COUNT(cmd)	\
do						\
{						\
  int		len;				\
  for (len = 0; cmd->param[len] != 0; len++)	\
    cmd->argc++;				\
}						\
while (0)



/* This structure contains the control flow context for e2dbg scripts */
/* Must be here and not in e2dbg.h cause of elfshargv_t */
typedef struct		s_e2dbgcontext
{
  int			savedfd;
  char			savedmode;
  elfshargv_t		*savedcmd;
  char			*(*savedinput)();
  char			**savedargv;
  char			*savedname;
  elfshargv_t		*curcmd;
}			e2dbgcontext_t;



/* The Embedded ELF debugger include file comes here */
#include <e2dbg.h>


/* ELFsh module structure */
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
}                     elfshmod_t;


  /* ELFsh color structure */
typedef struct          s_color
{

#define COLOR_NONE         0
#define COLOR_BOLD         1
#define COLOR_UNDERLINE    4
#define COLOR_RESET        160                                                                                                                                       
#define COLOR_SEPARE       ";"                                                                                                                                       
                                                                                                                                                                     
#define COLOR_TOKENS       50
#define COLOR_TOKEN_LEN    140
                                                                                                                                                                     
#define COLOR_FG_BLACK     30                                                                                                                                        
#define COLOR_FG_RED       31                                                                                                                                        
#define COLOR_FG_GREEN     32                                                                                                                                        
#define COLOR_FG_YELLOW    33                                                                                                                                        
#define COLOR_FG_BLUE      34                                                                                                                                        
#define COLOR_FG_MAGENTA   35                                                                                                                                        
#define COLOR_FG_CYAN      36                                                                                                                                        
#define COLOR_FG_WHITE     37                                                                                                                                        
                                                                                                                                                                     
#define COLOR_BG_BLACK     40                                                                                                                                        
#define COLOR_BG_RED       41                                                                                                                                        
#define COLOR_BG_GREEN     42                                                                                                                                        
#define COLOR_BG_YELLOW    43                                                                                                                                        
#define COLOR_BG_BLUE      44                                                                                                                                        
#define COLOR_BG_MAGENTA   45                                                                                                                                        
#define COLOR_BG_CYAN      46                                                                                                                                        
#define COLOR_BG_WHITE     47                                                                                                                                        
  u_int                 fground;                                                                                                                                     
  u_int                 bground;                                                                                                                                     
  u_int                 bold;                                                                                                                                        
  u_int                 underline;                                            
  
}                       color_t;

extern u_int		nocolor;

/* Elfsh Output Caching structure */
typedef struct          s_elfsh_outbuf
{
  int			nblines;
  unsigned int          cols;
  unsigned int          lines;
  char                  ignore;
}			elfshoutbuf_t;



/* Input / Output template for ELFsh */
typedef struct           s_elfsh_io
{
  
#define       ELFSH_IOSTD     1
#define       ELFSH_IONET     2
#define	      ELFSH_IODUMP    3
#define       ELFSH_IONUM     4
  char        type;                   /* IO type           */
  int         input_fd;               /* Input file        */
  int         output_fd;              /* Output file       */
  char        *(*input)();            /* Read Input data   */
  int         (*output)(char *buf);   /* Write output data */
  
  /* dump specific */
#if defined(ELFSHNET)
  pkt_t       *pkt;                   /* dump received pkt */
#endif
  int          new;                   /* 0 if already used */
#if defined(USE_READLN)
  char         *buf;                  /* readline line */
#endif
  
  elfshoutbuf_t       outcache;
}                     elfshio_t;


/* ELFsh's socket structure */
typedef struct       s_socket
{
  struct sockaddr_in addr;        /* sockaddr_in struct */
  int                socket;      /* The socket */
  char               **recvd;     /* List of received buffer */
  
  /* XXX: NEW/OLD is not explicit enough, rename ... */
#define NEW   1
#define OLD   0
  int                recvd_f;     /* NEW if the buffer was not passed to the parser yet */
#define YES 1
#define NO  0
  int                ready_f;     /* Have we received the trailing \n ? */
  
}                    elfshsock_t;



/* Printed screen */
typedef struct        s_screen
{
  u_char              x;              /* Screen height */
  u_char              y;              /* Screen width */
  char                *buf;           /* Screen buffer */
  char                *head;          /* Buffer's beginning */
  char                *tail;          /* Buffer's end */
}                     elfshscreen_t;


/* ELFsh job structure, one per client */
typedef struct        s_job
{
  
#define       ELFSH_INPUT     0
#define       ELFSH_OUTPUT    1
  elfshio_t           io;		 /* Current IO for this job */
  elfshsock_t         sock;		 /* Current socket, unused in initial job */
  
#define		      ELFSH_MAX_SOURCE_DEPTH  10
  elfshargv_t	      *script[ELFSH_MAX_SOURCE_DEPTH]; /* List of script commands */
  elfshargv_t         *lstcmd[ELFSH_MAX_SOURCE_DEPTH]; /* Last command for each depth */
  u_int               sourced;                         /* script depth (if beeing sourced) */ 
  
  elfshargv_t	      *curcmd;          /* Next command to be executed */
  elfshobj_t          *list;            /* List of loaded ELF objects */
  elfshobj_t          *current;         /* Current working ELF object */

  elfshobj_t          *dbglist;         /* List of objects loaded into e2dbg */
  elfshobj_t          *dbgcurrent;      /* Current working e2dbg file */
  
  u_char              active;                 
  time_t              createtime;
  int                 logfd;            /* Log file descriptor */
  elfshscreen_t       screen;           /* Last printed screen */
 
  char		      *oldline;		/* Previous command line */

#define       ELFSH_JOB_LOGGED (1 << 0)
  u_char              state;            /* Job state flags */
  
  asm_processor*      proc;		/* Processor structure */
  
}                     elfshjob_t;


/* Hold all the VM flags, sort of global context */
typedef struct        s_state
{
  char                vm_quiet;       /* Quiet mode : 0 or 1 */
  char                vm_use_regx;    /* Is a global regx available ? */
  regex_t	      vm_regx;        /* Global regx */
  char                *vm_sregx;      /* Global regx in string format */
  char                vm_use_alert;   /* Is an alert regx available ? */
  regex_t	      vm_alert;       /* Alert regx */
  char                *vm_salert;     /* ALert regx in string format */
  char                *sort;          /* Actual sorting choice */
  char                *input;         /* Implicit File input (-f option) */
  char                *output;        /* Implicit File output (-w option) */
  
#define       ELFSH_VMSTATE_CMDLINE   0
#define       ELFSH_VMSTATE_SCRIPT    1
#define       ELFSH_VMSTATE_IMODE     2
#define       ELFSH_VMSTATE_DEBUGGER  3
  char                vm_mode;        /* Command line, scripting, interactive, debugger ? */
  char                vm_stopped;     /* We are in a signal handler */
  char                vm_shared;      /* Next opened object must be shared */
  char                vm_net;         /* We are a node connected to the elf network */
  u_int               lastid;         /* Last Object ID */
}		      elfshstate_t;


/* Completion structure */
typedef struct        s_elfsh_comp
{
#define		      ELFSH_COMPMAX   16
  char		      **cmds[ELFSH_COMPMAX];         /* Matchs list for readline */
}		      elfshcomp_t;


/* The ELF shell world */
typedef struct        s_elfsh_world
{
  elfshmod_t	      *modlist;       /* ELFsh loaded modules list */
  elfshstate_t        state;          /* Flags structure */
  elfshcomp_t	      comp;           /* Completion strings */
  hash_t	      jobs;           /* Hash table of jobs */
  elfshjob_t	      *initial;       /* Main initial job */
  elfshjob_t	      *curjob;        /* Current job */
  elfshobj_t	      *shared;        /* List of shared descriptors */
  char                *scriptsdir;    /* Directory which contains script commands */
  asm_processor       proc;           /* Libasm world */
  asm_processor	      proc_sparc;     /* Libasm Sparc */	
  e2dbgworld_t        e2dbg;          /* Debugger world */
}		      elfshworld_t;


/* Meta object : describe an object in a standard way, whatever its hierarchy level */
typedef struct        s_elfshpath
{

  /* Handlers */
  elfsh_Addr          (*get_obj)(void *parent);
  int	              (*set_obj)(void *parent, elfsh_Addr value);
  char                *(*get_name)(elfshobj_t *, void *obj);
  int                 (*set_name)(elfshobj_t *, void *, char *);
  char                *(*get_data)(elfshsect_t *, u_int off, u_int);
  int                 (*set_data)(elfshsect_t *, u_int, char *, u_int, u_int);

  elfshobj_t          *root;          /* Root parent */
  void                *parent;        /* Direct parent */
  u_int               off;            /* Optional byte offset */
  u_int               size;           /* Size of the immediate string */
  u_int               sizelem;        /* Size of element for OBJRAW */
  char                immed;          /* Immediate binary flag */

  /* Immediate value of immed flag is set */
  union               immval
  {
    u_char            byte;
    u_short           half;
    u_int             word;
    elfsh_Addr        ent;
    char              *str;
  }                   immed_val;

  /* Here is the object type list */
#define               ELFSH_OBJINT    0       /* word : always 4 bytes */
#define               ELFSH_OBJSTR    1       /* String */
#define               ELFSH_OBJRAW    2       /* Raw */
#define               ELFSH_OBJUNK    3       /* Unknown */
#define               ELFSH_OBJLONG   4       /* Long object 4 or 8 bytes */
#define               ELFSH_OBJSHORT  5       /* Short : 2 bytes */
#define               ELFSH_OBJBYTE   6       /* One byte */
  char                        type;

  /* TRUE if this object is a scripting variable */
  char                        perm;
}                     elfshpath_t;



/* ELFsh Level 2 object (= L1 child) structure */
typedef struct		s_L2handler
{

  /* For fields */
  elfsh_Addr		(*get_obj)(void *obj);                          /* Read object */
  int			(*set_obj)(void *par, elfsh_Addr arg);          /* Write object */
  
  /* For names */
  char                  *(*get_name)(elfshobj_t *, void *obj);          /* Get name */
  int			(*set_name)(elfshobj_t *, void *, char *);      /* Set name */
  
  /* For sections data */
  char			*(*get_data)(elfshsect_t *, u_int off, u_int sizelem);   /* Read data */
  int			(*set_data)(elfshsect_t *, u_int, char *, u_int, u_int); /* Write data */
  
  char                type;                                           /* Object type */

}                     elfshL2_t;



/* ELFsh Level 1 object (= parent object) structure */
typedef struct        s_L1handler
{
  hash_t	*l2list;					/* A ptr on the child L2 hashtable */
  u_int		elem_size;					/* Size of one element of this object */
  
  /* Handlers */
  void		*(*get_obj)(void *file, void *arg);		/* Read object */
  void		*(*get_obj_idx)(void *file, elfsh_Addr i, void *a); /* Read handler for mutiple instanced L1 obj */
  void		*(*get_obj_nam)(void *file, char *name);	/* Read handler by name */
  void		*(*get_entptr)(void *file, elfsh_Addr idx);	/* Get address */
  elfsh_Addr   	(*get_entval)(void *ptr);			/* Get value */
  elfsh_Addr   	(*set_entval)(void *ptr, elfsh_Addr vaddr);     /* Set value */

}		elfshL1_t;

#if defined(USE_READLN)
extern rl_command_func_t *rl_ctrll;
#endif


/* The world */
extern elfshworld_t	world;

/* All the StandAlone hashtables */
extern hash_t		cmd_hash;	 /* commands handlers */
extern hash_t		file_hash;	 /* elfshobj_t pointers */
extern hash_t		const_hash;	 /* elf.h picked up constants values */
extern hash_t		redir_hash;	 /* Function redirections hash table */
extern hash_t		mod_hash;	 /* Modules name hash table */
extern hash_t		vars_hash;	 /* Scripting variables hash table */
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

extern hash_t           bg_color_hash;   /* colors def */
extern hash_t           fg_color_hash;   /* colors def */
extern hash_t           t_color_hash;    /* colors type */

/* Lattice for I/O */
extern char		*(*hooks_input[ELFSH_IONUM])();
extern int		(*hooks_output[ELFSH_IONUM])(char *buf);

/* Data value/string/description arrays */
extern elfshconst_t	elfsh_extseg_type[ELFSH_EXTSEG_MAX];
extern elfshconst_t	elfsh_seg_type[ELFSH_SEGTYPE_MAX];
extern elfshconst_t	elfsh_sh_type[ELFSH_SHTYPE_MAX];
extern elfshconst_t	elfsh_obj_type[ELFSH_OBJTYPE_MAX];
extern elfshconst_t	elfsh_sym_bind[ELFSH_SYMBIND_MAX];
extern elfshconst_t	elfsh_sym_type[ELFSH_SYMTYPE_MAX];
extern elfshconst_t	elfsh_dynentry_type[ELFSH_DYNAMIC_MAX];
extern elfshconst_t	elfsh_encoding[ELFSH_ENCODING_MAX];
extern elfshconst_t	elfsh_extdyn_type[ELFSH_EXTDYN_MAX];
extern elfshconst_t	elfsh_mipsdyn_type[ELFSH_MIPSDYN_MAX];
extern char		*elfsh_arch_type[ELFSH_ARCHTYPE_MAX];
extern char		*elfsh_stab_type[ELFSH_STAB_MAX];
extern elfshconst_t     elfsh_feature1[ELFSH_FEATURE_MAX];
extern elfshconst_t     elfsh_posflag1[ELFSH_POSFLAG_MAX];
extern elfshconst_t     elfsh_flags[ELFSH_FLAGS_MAX];
extern elfshconst_t     elfsh_flags1[ELFSH_FLAGS1_MAX];
extern elfshconst_t     elfsh_mipsflags[ELFSH_MIPSFLAGS_MAX];

extern elfshconst_t     elfsh_rel_type_i386[ELFSH_RELOC_i386_MAX];
extern elfshconst_t     elfsh_rel_type_ia64[ELFSH_RELOC_IA64_MAX];
extern elfshconst_t     elfsh_rel_type_sparc[ELFSH_RELOC_SPARC64_MAX];
extern elfshconst_t     elfsh_rel_type_alpha[ELFSH_RELOC_ALPHA_MAX];
extern elfshconst_t     elfsh_rel_type_mips[ELFSH_RELOC_MIPS_MAX];

extern elfshconst_t   elfsh_verentry_type[ELFSH_VERENTRY_MAX];

/* Network related variables */
//extern hash_t      elfsh_net_client_list;  /* The client socket's list */
extern int         elfsh_net_client_count; /* Number of clients connected */
//extern elfshsock_t    elfsh_net_serv_sock;    /* The main socket structur */

/* Commands execution handlers, each in their respective file */
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
int		cmd_sym();
int		cmd_stab();
int             cmd_hexa();
int             cmd_disasm();
int             cmd_shtrm();
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
int		cmd_meta();
int		cmd_write();
int		cmd_append();
int		cmd_extend();
int		cmd_fixup();
int		cmd_quiet();
int		cmd_verb();
int		cmd_exec();
int		cmd_findrel();
int		cmd_modload();
int		cmd_modunload();
int		cmd_strip();
int		cmd_sstrip();
int		cmd_relinject();
int		cmd_stop();
int		cmd_hijack();
int		cmd_cmp();
int		cmd_jmp();
int		cmd_je();
int		cmd_jne();
int		cmd_jg();
int		cmd_jl();
int		cmd_jge();
int		cmd_jle();
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

int		cmd_shared();
int		cmd_cleanup();

int             cmd_verneed();
int             cmd_verdef();
int             cmd_version();
int             cmd_hashx();

int		vm_add_script_cmd(char *dirstr);
int		vm_clearscreen(int i, char c);
int		vm_install_clearscreen();


#ifdef __DEBUG_TEST__
int		cmd_test();
#endif

int		vm_screen_switch();


/* Hash functions */
int           vm_hashunk(int i);
int           vm_hashbucketprint(int t, int i, int s, char *n, int r, int h, int c);
int           vm_hashchainprint(int i, int s, char *n, int r, int h);


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
int		cmd_run();
int		cmd_display();
int		cmd_undisplay();

/* Registration handlers for options from opt.c */
int		vm_getoption(u_int index, u_int argc, char **argv);
int		vm_getoption2(u_int index, u_int argc, char **argv);
int		vm_getoption3(u_int index, u_int argc, char **argv);
int		vm_getregxoption(u_int index, u_int argc, char **argv);
int		vm_getinput(u_int index, u_int argc, char **argv);
int		vm_getoutput(u_int index, u_int argc, char **argv);
int		vm_getdisasm(u_int index, u_int argc, char **argv);
int		vm_gethexa(u_int index, u_int argc, char **argv);
int		vm_getvarparams(u_int index, u_int argc, char **argv);

/* Libasm resolve handlers */
void		do_resolve(void *data, u_int vaddr, char *, u_int);
u_int		display_instr(int, u_int, u_int, u_int, u_int,
			      char *, u_int, char *, char *, u_int *);
char		*vm_resolve(elfshobj_t *file, elfsh_Addr addr, elfsh_SAddr *roffset);

/* General VM functions */
elfshpath_t	*vm_lookup_param(char *param, u_int mode);
elfshpath_t	*vm_check_object(elfshpath_t *pobj);
elfshobj_t	*vm_getfile(u_int index);
elfshmod_t	*vm_getmod(u_int index);
char		*vm_reverse(elfshobj_t *file, u_int vaddr);

/* Lookup functions */
elfshobj_t	*vm_lookup_file(char *param);
elfshpath_t	*vm_lookup_immed(char *param);
elfsh_Addr	vm_lookup_index(char *param);
char		*vm_lookup_var(char *param);
char		*vm_lookup_string(char *param);

/* Lazy Abstract Type system functions */
int		vm_convert2str(elfshpath_t *obj);
int		vm_convert2int(elfshpath_t *obj);
int		vm_convert2long(elfshpath_t *obj);
int		vm_convert2raw(elfshpath_t *obj);
int		vm_convert2byte(elfshpath_t *obj);
int		vm_convert2short(elfshpath_t *obj);

/* Command API */
int		vm_setcmd(char *cmd, void *exec, void *reg, u_int needcur);
int		vm_addcmd(char *cmd, void *exec, void *reg, 
			  u_int needfile, char *help);
int		vm_delcmd(char *cmd);

/* Versions functions */
int             vm_version_pdef(hashdef_t *p, u_int ai, u_int i, char *id, 
				char *n, char *t, regex_t *r);
int             vm_version_pneed(hashneed_t *p, u_int ai, u_int i, char *id, 
				 char *n, char *t, regex_t *r);
int             vm_version_unk(u_int ai, u_int i, char *id, char *n, char *t);

/* Parsing, Scanning, I/O functions */
char		*vm_filter_param(char *buf, char *ptr);
char		*vm_build_unknown(char *buf, const char *str, u_long type);
void		vm_filter_zero(char *str);
int		vm_parseopt(int argc, char **argv);
void            vm_findhex(u_int argc, char **argv);
char            *vm_getln(char *ptr);
u_int           vm_findblanks(char *buf);
char            **vm_doargv(u_int nbr, u_int *argc, char *buf);
int		vm_initio();
char            **vm_input(int *argc);
char		*vm_stdinput();
int		vm_flush();
int		vm_output(char *str);
int		vm_output_nolog(char *str);
int		vm_outerr(char *str);
int		vm_output_bcast(char *str);
int		vm_stdoutput(char *str);
int		vm_display_prompt();
void		vm_ln_handler (char *c);
void		vm_setinput(elfshjob_t *j, int fd);
void		vm_setoutput(elfshjob_t *j, int fd);
char		*vm_basename(char *str);
char		*vm_get_string(char **params);
void		vm_log(char *str);
int             vm_closelog();

/* Internal functions */
elfshmod_t	*vm_modprobe();
void		vm_setup_hashtables();

int		vm_doerror(void (*fct)(char *str), char *str);
void		vm_error(char *label, char *param);
void		vm_badparam(char *str);
void		vm_unknown(char *str);
void		vm_exit(int err);

void		vm_print_banner();
void		vm_dynentinfo(elfshobj_t *f, elfsh_Dyn *ent, char *info);
int		vm_usage(char *str);
int		vm_modlist();
int		vm_isnbr(char *string);
void		vm_load_cwfiles();
int		vm_implicit(elfshcmd_t *actual);
int	        vm_unload_cwfiles();
int		dprintf(int fd, char *format, ...);
void	        vm_print_pht(elfsh_Phdr *phdr, uint16_t num, elfsh_Addr base);
int             vm_print_sht(elfsh_Shdr *shdr, u_int num, char rtflag);
int		vm_load_file(char *name, elfsh_Addr base, elfshlinkmap_t *lm);
int		vm_is_loaded(char *name);
int		vm_doswitch(int nbr);

int		vm_init() __attribute__((constructor)) ;
int		vm_loop(int argc, char **argv);
int		vm_setup(int ac, char **av);
int		vm_run(int ac, char **av);
int		vm_config();
int		vm_main(int ac, char **av);

/* Scripting flow functions */
int		vm_execscript();
int		vm_execmd();
int		vm_move_pc(char *idx);
int		vm_openscript(char **av);
int		vm_testscript(int ac, char **av);
int		vm_exec_str(char *str);

/* Variable functions */
int             vm_setvar_str(char *varname, char *value);
int             vm_setvar_raw(char *varname, char *value, u_int len);
int             vm_setvar_byte(char *varname, u_char byte);
int             vm_setvar_short(char *varname, u_short val);
int             vm_setvar_int(char *varname, u_int val);
int             vm_setvar_long(char *varname, u_long val);


/* Readline stuff (XXX: need to be prefixed) */
char		**custom_completion(const char* text, int start, int end);

/* Color functions */
color_t         *vm_colortable(char *t, char *te);
int             vm_colorpattern(color_t *t, char *te, char *p);
color_t         *vm_colorblank();
char            *vm_colorget(char *p, char *ty, void *o);
void            vm_endline();
char 		*vm_coloradv(char *ty, char *p, char *te);
char            *vm_colorstr(char *t);
char            *vm_colorfieldstr(char *t);
char            *vm_colortypestr(char *t);
char            *vm_coloraddress(char *p, elfsh_Addr a);
char            *vm_colornumber(char *p, u_int n);
char            *vm_colorsends(char *t); 
char            *vm_colorwarn(char *t);
char		*vm_colorstring(char *text);
char		*vm_colorsends(char *text); 
char		*vm_colorwarn(char *text);
char            *vm_colorstr_fmt(char *p, char *t);
char            *vm_colorfieldstr_fmt(char *p, char *t);
char            *vm_colortypestr_fmt(char *p, char *t);
char		*vm_colorwarn_fmt(char *pattern, char *text);
char		*vm_colorinstr(char *text);

/* Object creation/verification functions */
int		vm_convert_object(elfshpath_t *obj, u_int objtype);
elfshL1_t	*vm_create_L1ENT(void		*get_obj,
				 void		*get_obj_idx,
				 void		*get_obj_nam,
				 hash_t		*l2_hash,
				 void		*get_entptr,
				 void		*get_entval,
				 void		*set_entval,
				 u_int		elem_size);
elfshL2_t	*vm_create_L2ENT(void	*get_obj,
				 void	*set_obj,
				 char	type,
				 void	*get_name,
				 void	*set_name,
				 void	*get_data,
				 void	*set_data);
elfshcmd_t	*vm_create_CMDENT(int		(*exec)(void *file, void *av),
				  int		(*reg)(u_int i, u_int ac, char **av),
				  int		flags, char *help);
elfshredir_t	*vm_create_REDIR(u_char type, char *sname, char *dname, 
				 elfsh_Addr saddr, elfsh_Addr daddr);
elfshpath_t	*vm_create_IMMED(char type, char perm, u_int val);
elfshpath_t	*vm_create_IMMEDSTR(char perm, char *str);
elfshpath_t	*vm_create_LONG(char perm, elfsh_Addr val);

/* Network related functions */
int		vm_net_init();
elfshjob_t	*vm_get_curlocaljob();
int		vm_select();
char		*vm_net_input();
int		vm_net_output(char *buf);
int		vm_net_recvd();
int		vm_net_accept();
int		vm_dump_accept();
elfshjob_t	*vm_socket_add(int socket, struct sockaddr_in *addr);
int		vm_socket_del(char *inet_addr);
int		vm_socket_get_nb_recvd(char *inet);
int		vm_update_recvd(elfshsock_t *socket);
char		*vm_socket_merge_recvd(elfshsock_t *socket);

/* Interface related functions */
int		vm_system(char *cmd);
void		vm_dbgid_set(u_int pid);
u_int		vm_dbgid_get();

/* Workspace related functions*/
int		cmd_workspace();
int		cmd_next_workspace();
int		vm_own_job(elfshjob_t *job);
int		vm_valid_workspace(char *name);
void		vm_switch_job(elfshjob_t *job);
elfshjob_t	*vm_clone_job(elfshjob_t *job);

void		wait4exit(void *);

#endif /* __ELFSH_H_ */



int		vm_config();
