/**
 * @defgroup ERESI The ERESI runtime environment macros
 */

/**
 * @file librevm/include/revm-macros.h
 * @ingroup librevm
 *
 * $Id: revm.h 1456 2013-03-12 05:22:33Z may $
 */

#ifndef __REVM_MACROS_H__
  #define __REVM_MACROS_H__

/* Parsing related defines */
#define	REVM_MAXNEST_LOOP	10

/* XXX: REVM still needs those definitions somewhere, they should go into a new libstderesi/debugging/ */
#define	E2DBG_NAME		"Embedded ELF Debugger"
#define	E2DBG_ARGV0		"e2dbg"
#define	CMD_CONTINUE		"c"
#define	CMD_CONTINUE2		"continue"
#define	CMD_CONTINUE3		"cont"
#define	CMD_NEXT		"n"
#define	CMD_NEXT2		"next"
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
#define	REVM_OP_SHL		10
#define	REVM_OP_SHR		11
#define REVM_OP_OR		12

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
#define ERESI_CONFIG_ONLOAD_RCONTROL	"onload.restore_control"
#define ERESI_CONFIG_GRAPH_STORAGEPATH  "graph.storagepath"
#define ERESI_CONFIG_GRAPH_VIEWCMD	"graph.viewcmd"
#define ERESI_CONFIG_GRAPH_AUTOVIEW	"graph.autoview"
#define ERESI_CONFIG_GRAPH_AUTOBUILD	"graph.autobuild"
#define REVM_CONFIG_USEMORE		"vm.use_more"
#define	ERESI_CONFIG_ASMVERB		"asm.debug"
#define ERESI_CONFIG_HISTORY		"history"
#define ERESI_DEFAULT_HISTORY		".eresi_history"

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


#endif
