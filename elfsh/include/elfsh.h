/*
** elfsh.h for elfsh
**
** Started on  Thu Feb 22 07:19:04 2001 mayhem
** Last update Fri Aug 15 22:43:41 2003 jv
*/

#ifndef __ELFSH_H_
 #define __ELFSH_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>
#include <setjmp.h>
#include <time.h>
#include <ctype.h>
#include <regex.h>
#include <dlfcn.h>
#include <libelfsh.h>
#include <libhash.h>


/* Thanks to DH for libasm, choose to disable it in elfsh/elfsh/Makefile */
#if defined(USE_LIBASM)
 #include <libasm.h>
 extern asm_processor	proc;
#endif

/* Thanks to GNU we have readline */
#if defined(USE_READLN)
 #include <readline/readline.h>
 #include <readline/history.h>
#endif

/* Now comes DEBUGGING variables for various part of the code */
#define	__DEBUG_DISASM__	0
#define	__DEBUG_SIGHANDLER__	0
#define	__DEBUG_MODEL__		0
#define	__DEBUG_SCANNER__	0
#define	__DEBUG_ASLR__		0

/* General usage macros */
#define FATAL(a)		{ perror(a); exit(-1);			      }
#define QUIT_ERROR(a)		{ elfsh_error(); exit(a);		      }
#define RET(a)			{ /* elfsh_error(); */ return (a);	      }
#define RETERR(a)		{ fprintf(stderr, "%s \n", a); return (-1);   }
#define	PERROR_RET(a, b)	{ perror(a); return (b);		      }
#define	PRINTABLE(c)		(c >= 32 && c <= 126)
#define REGX_IS_USED(a)		a
#define	IS_VADDR(s)		(s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
#define	IS_BLANK(c)		(c == ' ' || c == '\t')

/* Some useful macros */
#define	CHOOSE_REGX(r)	r = (world.args.use_regx     ? &world.args.regx     : \
			     world.state.vm_use_regx ? &world.state.vm_regx : \
			     NULL)


/* Used to store ascii description for different structures types in data.c */
#define ELFSH_SEGTYPE_MAX	7
#define	ELFSH_SHTYPE_MAX	12
#define	ELFSH_OBJTYPE_MAX	5
#define	ELFSH_SYMBIND_MAX	3
#define	ELFSH_SYMTYPE_MAX	7
#define	ELFSH_ENCODING_MAX	3
#define	ELFSH_DYNAMIC_MAX	35
#define	ELFSH_EXTDYN_MAX	19
#define	ELFSH_ARCHTYPE_MAX	55
#define	ELFSH_ARCHTYPE_MAX	55
#define	ELFSH_STAB_MAX		256

#define	ELFSH_RELOC_i386_MAX	11
#define	ELFSH_RELOC_SPARC_MAX	24
#define ELFSH_RELOC_MAX(file)   vm_getmaxrelnbr(file)


#define	ELFSH_FEATURE_MAX	2
#define	ELFSH_POSFLAG_MAX	2
#define	ELFSH_FLAGS_MAX		4
#define	ELFSH_FLAGS1_MAX	15

/* ELFsh general parameters */
#define	ELFSH_FIELD_SEP		"."
#define	ELFSH_COMMENT_START	'#'
#define ELFSH_MINUS		'-'
#define	ELFSH_SLASH		'/'
#define	ELFSH_SPACE		' '
#define	ELFSH_VERSION		"0.51b3"
#define ELFSH_PROMPT		"[ELFsh-"ELFSH_VERSION"]$ "
#define	ELFSH_SHELL		"/bin/bash"
#define	ELFSH_INIT		"elfsh_init"
#define	ELFSH_FINI		"elfsh_fini"
#define	ELFSH_MODPATH		"/usr/share/elfsh/"

/* For elfsh/elfsh/disasm.c:display_object() */
#define	ELFSH_HEXA_VIEW		0
#define	ELFSH_DISASM_VIEW	1

/* For elfsh/elfsh/modules.c:vm_change_handler() */
#define	ELFSH_ORIG		((void *) -1)

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
#define	CMD_HELP		"help"
#define	CMD_STRIP		"strip"
#define	CMD_SSTRIP		"sstrip"
#define	CMD_RELINJCT		"reladd"
#define	CMD_STOP		"stop"
#define	CMD_HIJACK		"redir"

/* Prefixes */
#define	CMD_SORT		 "sort"
#define	CMD_SORT2		 "sr"
#define	CMD_QUIET		 "q"
#define	CMD_QUIET2		 "quiet"
#define	CMD_VERB		 "verb"
#define	CMD_VERB2		 "v"
#define CMD_ALL			 "all"
#define CMD_ALL2		 "a"

/* Interactive only command */
#define	CMD_LOAD		 "load"
#define	CMD_UNLOAD		 "unload"
#define	CMD_SAVE		 "save"
#define	CMD_QUIT		 "quit"
#define	CMD_QUIT2		 "exit"
#define	CMD_SWITCH		 "switch"
#define	CMD_LIST		 "list"
#define	CMD_LIST2		 "l"



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
  long			val;
}			elfshconst_t;


/* Thats the command line options registering structure */
typedef struct		s_args
{
  char			*param[256];	/* option parameters */
  char			use_regx;	/* 1 if the option use a regx */
  regex_t		regx;		/* regx */
  elfshlist_t		disasm;		/* D/X parameters */
}			elfshargv_t;

/* ELFsh module structure */
typedef struct		s_module
{
  char			*path;		/* Name */
  void			*handler;	/* Object handler */
  void			(*init)();	/* Constructor pointer */
  void			(*fini)();	/* Destructor pointer */
  u_int			id;		/* Object ID */
  time_t		loadtime;	/* Load time stamp */
  struct s_module	*next;		/* Next module of the list */
}			elfshmod_t;


/* Hold all the VM flags, modifying the behavior or the shell */
typedef struct	s_state
{
  char		vm_quiet;	/* Quiet mode : 0 or 1 */
  char		vm_use_regx;	/* Is a global regx available ? */
  regex_t	vm_regx;	/* Global regx */
  char		*vm_sregx;	/* Global regx in string format */

  char		*sort;		/* Actual sorting choice */
  char		*input;		/* Implicit File input (-f option) */
  char		*output;	/* Implicit File output (-w option) */

#define	ELFSH_VMSTATE_CMDLINE	0
#define	ELFSH_VMSTATE_SCRIPT	1
#define	ELFSH_VMSTATE_IMODE	2
  char		vm_mode;	/* Command line, scripting, or interactive ? */
  char		vm_stopped;	/* We are in a signal handler */
  u_int		lastid;		/* Last Object ID */
}		elfshstate_t;




/* The ELF shell world */
typedef struct		s_elfsh_world
{
  elfshargv_t		args;		/* Command line parameters */
  elfshobj_t		*list;		/* List of loaded ELF objects */
  elfshobj_t		*current;	/* current working ELF object */
  elfshstate_t		state;		/* Flags structure */
  elfshmod_t		*modlist;	/* ELFsh module list */
  char			**cmds;		/* commands list for readline */

#if defined(USE_LIBASM)
  asm_processor		proc;		/* Libasm world */
#endif

}			elfshworld_t;


/* Meta object : describe an object in a standard way, whatever its hierarchy level */
typedef struct		s_elfshpath
{

  /* Handlers */
  u_long		(*get_obj)(void *parent);
  u_long		(*set_obj)(void *parent, long value);
  char			*(*get_name)(elfshobj_t *, void *obj);
  int			(*set_name)(elfshobj_t *, void *, char *);
  char			*(*get_data)(elfshsect_t *, u_int off, u_int);
  int			(*set_data)(elfshsect_t *, u_int, char *, u_int, u_int);

  elfshobj_t		*root;		/* Root parent */
  void			*parent;	/* Direct parent */

  u_int		        off;		/* Optional byte offset */
  u_int			size;		/* Size of the immediate string */
  u_int			sizelem;	/* Size of element for OBJRAW */
  char			immed;		/* Immediate binary flag */

  /* Immediate value of immed flag is set */
  union			immval
  {
    char		*str;
    long		ent;
  }			immed_val;

  /* Here is the object type list */
#define		ELFSH_OBJINT	0	/* Dword */
#define		ELFSH_OBJSTR	1	/* String */
#define		ELFSH_OBJRAW	2	/* Raw */
#define		ELFSH_OBJUNK	3	/* Unknown */
  char			type;



}			elfshpath_t;


/* ELFsh Level 2 object (= L1 child) structure */
typedef struct		s_L2handler
{

  /* For fields */
  int		(*get_obj)(void *obj);				/* Read object */
  int		(*set_obj)(void *par, u_int arg);		/* Write object */

  /* For names */
  char		*(*get_name)(elfshobj_t *, void *obj);		/* Get name */
  int		(*set_name)(elfshobj_t *, void *, char *);	/* Set name */

  /* For sections data */
  char		*(*get_data)(elfshsect_t *, u_int off, u_int sizelem);	 /* Read data */
  int		(*set_data)(elfshsect_t *, u_int, char *, u_int, u_int); /* Write data */

  char		type;						/* Object type */

}			elfshL2_t;


/* ELFsh Level 1 object (= parent object) structure */
typedef struct	s_L1handler
{
  hash_t	*l2list;					/* A ptr on the child L2 hashtable */
  u_int		elem_size;					/* Size of one element of this object */

  /* Handlers */
  void		*(*get_obj)(void *file, void *arg);		/* Read object */
  void		*(*get_obj_idx)(void *file, u_int i, void *a);	/* Read handler for mutiple instanced L1 obj */
  void		*(*get_obj_nam)(void *file, char *name);	/* Read handler by name */
  void		*(*get_entptr)(void *file, u_int idx);		/* Get address */
  u_int		(*get_entval)(void *ptr);			/* Get value */
  u_int		(*set_entval)(void *ptr, u_int vaddr);		/* Set value */

}		elfshL1_t;



/* ELFsh command handlers */
typedef struct	s_cmdhandler
{
  int		(*reg)(u_int i, u_int s, char **a);	/* Registration handler */
  int		(*exec)();				/* Execution handler */
  char		*arg1;					/* Option activation variable ptr */
  void		*arg2;					/* Option regex ptr */
  char		*arg3;					/* Regex switch ptr */
  char		wflags;					/* 1 if the cmd need a valid curfile */
}		elfshcmd_t;



/* The world */
extern elfshworld_t	world;

/* All the StandAlone hashtables */
extern hash_t		cmd_hash;	/* commands handlers */
extern hash_t		file_hash;	/* elfshobj_t pointers */
extern hash_t		const_hash;	/* elf.h picked up constants values */
extern hash_t		mod_hash;	/* Modules name hash table */

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

/* Data value/string/description arrays */
extern elfshconst_t	elfsh_seg_type[ELFSH_SEGTYPE_MAX];
extern elfshconst_t	elfsh_sh_type[ELFSH_SHTYPE_MAX];
extern elfshconst_t	elfsh_obj_type[ELFSH_OBJTYPE_MAX];
extern elfshconst_t	elfsh_sym_bind[ELFSH_SYMBIND_MAX];
extern elfshconst_t	elfsh_sym_type[ELFSH_SYMTYPE_MAX];
extern elfshconst_t	elfsh_dynentry_type[ELFSH_DYNAMIC_MAX];
extern elfshconst_t	elfsh_encoding[ELFSH_ENCODING_MAX];
extern elfshconst_t	elfsh_extdyn_type[ELFSH_EXTDYN_MAX];
extern char		*elfsh_arch_type[ELFSH_ARCHTYPE_MAX];
extern char		*elfsh_stab_type[ELFSH_STAB_MAX];
extern elfshconst_t    elfsh_feature1[ELFSH_FEATURE_MAX];
extern elfshconst_t    elfsh_posflag1[ELFSH_POSFLAG_MAX];
extern elfshconst_t    elfsh_flags[ELFSH_FLAGS_MAX];
extern elfshconst_t    elfsh_flags1[ELFSH_FLAGS1_MAX];

extern elfshconst_t    elfsh_rel_type_i386[ELFSH_RELOC_i386_MAX];
extern elfshconst_t    elfsh_rel_type_sparc[ELFSH_RELOC_SPARC_MAX];

/* Commands execution handlers, each in their respective file */
int		cmd_dyn();
int		cmd_sht();
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

/* Registration handlers for options from opt.c */
int		vm_getoption(u_int index, u_int argc, char **argv);
int		vm_getoption2(u_int index, u_int argc, char **argv);
int		vm_getoption3(u_int index, u_int argc, char **argv);
int		vm_getregxoption(u_int index, u_int argc, char **argv);
int		vm_getglregx(u_int index, u_int argc, char **argv);
int		vm_getinput(u_int index, u_int argc, char **argv);
int		vm_getoutput(u_int index, u_int argc, char **argv);
int		vm_getsort(u_int index, u_int argc, char **argv);
int		vm_getdisasm(u_int index, u_int argc, char **argv);
int		vm_gethexa(u_int index, u_int argc, char **argv);
int		vm_getvarparams(u_int index, u_int argc, char **argv);

int		dprintf(int fd, char *format, ...);

/* Libasm resolve handler */
void		do_resolve(void *data, u_int vaddr, char *, u_int);
u_int		display_instr(int, u_int, u_int, u_int, u_int,
			      char *, u_int, char *);

/* Other VM functions */
elfshobj_t	*vm_getfile(u_int index);
elfshmod_t	*vm_getmod(u_int index);
elfshmod_t	*vm_modprobe();
char		*vm_filter_param(char *buf, char *ptr);
char		**vm_getln(int *argc);
char		*vm_build_unknown(char *buf, const char *str, u_long type);
char		*vm_reverse(elfshobj_t *file, u_int vaddr);
void		vm_load_cwfiles(char **argv);
void		vm_setup_hashtables();
void		vm_badparam(char *str);
void		vm_unknown(char *str);
void		vm_print_banner();
void		vm_dynentinfo(elfshobj_t *f, Elf32_Dyn *ent, char *info);
void		vm_filter_zero(elfshpath_t *obj);
int		vm_implicit(elfshcmd_t *actual, char **argv);
int	        vm_unload_cwfiles();
int		vm_parseopt(int argc, char **argv);
int		vm_lookup_param(char *param, elfshpath_t *pobj, u_int mode);
int		vm_usage(char *str);
int		vm_openscript(char *name, char *av0);
int		vm_doerror(void (*fct)(char *str), char *str);
int		vm_modlist();
int		vm_convert_object(elfshpath_t *obj, u_int objtype);
int		vm_check_object(elfshpath_t *pobj);
int		vm_isnbr(char *string);

/* cmdapi.c */
int		vm_setcmd(char *cmd, void *exec, void *reg, u_int needcur);
int		vm_addcmd(char *cmd, void *exec, void *reg, u_int needfile);
int		vm_delcmd(char *cmd);

/* readline stuff (XXX: need to be prefixed) */
char		**coustom_completion(const char* text, int start, int end);

/* Object constructor functions */
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
				  int		flags);

#endif /* __ELFSH_H_ */
