/**
** @file libstderesi.h
** 
** Started on Thu Feb 22 07:19:04 2001 jfv
** $Id: libstderesi.h,v 1.5 2008-02-16 12:32:27 thor Exp $
*/
#ifndef __LIBSTDERESI_H_
 #define __LIBSTDERESI_H_

/* Debug flags for libstderesi */
#define	__DEBUG_REWRITE__ 0

/* User defined configuration */
#include "revm.h"
#include "libetrace.h"
#include "libmjollnir.h"

/* Help strings */
#include "libstderesi-help.h"

/* Commands */
#define	CMD_DEFINE		"define"
#define	CMD_UNDEF		"undef"
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
#define	CMD_UNION		"union"
#define	CMD_TYPEDEF		"typedef"
#define	CMD_INTO		"into"

#define CMD_INSERT		"insert"
#define	CMD_INSERT2		"ins"
#define	CMD_REMOVE		"remove"
#define	CMD_REMOVE2		"rm"
#define	CMD_FLUSH		"flush"
#define	CMD_VLIST		"vlist"
#define	CMD_VARLIST		"varlist"
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
#define CMD_NETWORK_GDBSUPPORT  "netgdb"
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
#define	CMD_REFLECT		"reflect"

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
#define	CMD_SWITCH		 "switch"
#define	CMD_LIST		 "flist"
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

#define	CMD_RSHT		"rsht"
#define	CMD_RPHT		"rpht"

/* Structures for subcommands provided by libetrace */

/* Sub-commands, for now only used by the tracer */
typedef struct		s_revmtraces
{
  int			(*exec)(elfshobj_t*, char *, char **);	/* Function used */

  /* Unexistant (0), optional (1), needed (2) */
  char			flagName;	/* Need a first argument */
  char			flagArg;       	/* Need a second argument */
}			tracecmd_t;

/* Extern variables */
extern char	quit_msg[512];
extern void	(*prompt_token_setup)(char *name, unsigned int size);
extern char	prompt_token[512];
extern unsigned char   quit_msg_setup;

/* Used to store ascii description for different structures types in data.c */
#define	ELFSH_INSTRTYPE_MAX	24
#define	ELFSH_OPTYPE_MAX	4
#define ELFSH_SEGTYPE_MAX	7
#define	ELFSH_SHTYPE_MAX	16
#define	ELFSH_OBJTYPE_MAX	5
#define	ELFSH_SYMBIND_MAX	3
#define	ELFSH_SYMTYPE_MAX	STT_NUM + 1
#define	ELFSH_ENCODING_MAX	3
#define	ELFSH_DYNAMIC_MAX	35
#define	ELFSH_EXTDYN_MAX	19
#define ELFSH_MIPSDYN_MAX	43
#define	ELFSH_ARCHTYPE_MAX	95
#define	ELFSH_EXTSEG_MAX	5
#define	ELFSH_RELOC_i386_MAX	11
#define	ELFSH_RELOC_IA64_MAX	81
#define	ELFSH_RELOC_SPARC_MAX	55
#define ELFSH_RELOC_SPARC64_MAX	55
#define	ELFSH_RELOC_ALPHA_MAX	43
#define	ELFSH_RELOC_MIPS_MAX	35

#define	ELFSH_FEATURE_MAX	2
#define	ELFSH_POSFLAG_MAX	2
#define	ELFSH_FLAGS_MAX		4
#define	ELFSH_FLAGS1_MAX	15
#define ELFSH_MIPSFLAGS_MAX	16

#define	ELFSH_LINKTYPE_MAX	7
#define	ELFSH_LINKSCOPE_MAX	3

/* Data value/string/description arrays */
extern revmconst_t     asm_instr_type[ELFSH_INSTRTYPE_MAX];
extern revmconst_t     asm_operand_type[ELFSH_OPTYPE_MAX];
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
extern revmconst_t     elfsh_arch_type[ELFSH_ARCHTYPE_MAX];
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
extern revmconst_t     elfsh_link_type[ELFSH_LINKTYPE_MAX];
extern revmconst_t     elfsh_link_scope[ELFSH_LINKSCOPE_MAX];

/* Constructors */
void            eresi_commands_init();
void            eresi_stdconstructor() __attribute__((constructor));

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
int		cmd_return();
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
int             cmd_network_gdbsupport();
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
int		cmd_into();
int		cmd_pre();
int		cmd_post();
int		cmd_reflect();
int		cmd_default();
int		cmd_define();
int		cmd_undef();

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

/* Trace functions */
int             traces_addcmd(char *cmd, void *exec, char flagName, char flagArg);   
int             traces_add(elfshobj_t *file, char *name, char **optarg);  
int             traces_rm(elfshobj_t *file, char *name, char **optarg);  
int             traces_exclude(elfshobj_t *file, char *freg, char **oreg);  
int             traces_rmexclude(elfshobj_t *file, char *freg, char **oreg);  
int             traces_enable(elfshobj_t *file, char *name, char **optarg);  
int             traces_disable(elfshobj_t *file, char *name, char **optarg);  
int             traces_create(elfshobj_t *file, char *name, char **optarg);  
int             traces_delete(elfshobj_t *file, char *name, char **optarg);  
int             traces_flush(elfshobj_t *file, char *name, char **optarg);  
int             traces_list(elfshobj_t *file, char *name, char **optarg);  
int             traces_run(elfshobj_t *file, char **argv, int argc);

#endif /* __LIBSTDERESI_H_ */

