/**
 * @defgroup librevm librevm: The Reverse Engineering Vector Machine.
 * @ingroup ERESI
 */
/**
* @file librevm/include/revm-objects.h
** @ingroup librevm
**
** Started on  Thu Feb 22 07:19:04 2001 jfv
** $Id$
*/
#ifndef __REVM_OBJECTS_H_
 #define __REVM_OBJECTS_H_

/** 
 * Meta-object : Abstract object description when reflected 
 */
typedef	struct		s_revm_object
{

  /* Access handlers */
  eresi_Addr          (*get_obj)(void *parent);
  int	              (*set_obj)(void *parent, eresi_Addr value);
  char                *(*get_name)(void*, void *obj);
  int                 (*set_name)(void*, void *, char *);
  char                *(*get_data)(void*, u_int off, u_int);
  int                 (*set_data)(void*, u_int, char *, u_int, u_int);

  /* Attributes */
  elfshobj_t          *root;          /* Root parent */
  void                *parent;        /* Direct parent */
  u_int               off;            /* Optional byte offset */
  u_int               size;           /* Size of the immediate string */
  u_int               sizelem;        /* Size of element for OBJRAW */
  u_char              immed;          /* Immediate binary flag */
  u_char              perm;	      /* TRUE if obj is a script variable */
  aspectype_t	      *otype;	     /* The object type identifier */

  /* Only when describing elements of hash tables or list */
#define		      CONT_UNKNOW	0
#define		      CONT_HASH		1
#define		      CONT_LIST		2
  u_char	      contype;		/* Container type */
  char		      *hname;		/* Name of parent container */
  char		      *kname;		/* Name of element in container */

  /* Immediate value if immed flag is set */
  union               immval
  {
    u_char            byte;
    u_short           half;
    u_int             word;
    eresi_Addr        ent;
    char              *str;
  }                   immed_val;
}                     revmobj_t;


typedef struct s_revm_annotation revmannot_t;

/**
 * @brief Structure for an expression in ERESI : A tree of revmobj_t 
 */
typedef struct		s_revm_expr
{
  char			*label;		/*! Relative current field name */
  char			*strval;	/*! ASCII form of current expression object */
  aspectype_t		*type;		/*! Expression type */
  revmobj_t		*value;		/*! Expression field value */
  revmannot_t		*annot;		/*! Annotation on this expression */
  struct s_revm_expr	*parent;	/*! Parent expression */
  struct s_revm_expr	*childs;	/*! Child expressions list, if non-terminal */
  struct s_revm_expr	*next;		/*! Next expression if curexpr is in record */
}			revmexpr_t;


/**
 * @brief Generic structure for objects program annotations in ERESI 
 */
struct			s_revm_annotation
{
  u_char		inhash;		/*! Is this expression somewhere in hash or list ? */
#define EDFMT_SCOPE_UNK    0
#define EDFMT_SCOPE_GLOBAL 1
#define EDFMT_SCOPE_FUNC   2
  u_char 		scope;		/*! XXX: Unused for now ? */
  eresi_Addr 		addr; 		/*! Global address */
  u_int			typenum;	/*! Type id associated to this annotation */
  u_int 		reg;   		/*! Function reg id base (stack) */
  int 			relvalue;     	/*! Relative value based on reg */
  int			nameoff;	/*! Name offset in string table */
					/* XXX: should be a symbol instead */
  revmexpr_t		*expr;		/* Annotated expression, if already constructed */
};


/**********************************************************************************/
/****** Those 2 objects are reminicescence of the old object system of elfsh ******/
/**** Need to disapear to use only revmannot/revmobj/aspectype instead of those *****/
/**************** This only affects ERESI types of ELF structures ******************/
/**********************************************************************************/


/**
 * @brief REVM Level 2 (= child of L1) object structure 
 */
typedef struct	s_L2handler
{

  /* For fields */
  eresi_Addr	(*get_obj)(void *obj);                          /*! Read object */
  int		(*set_obj)(void *par, eresi_Addr arg);          /*! Write object */
  
  /* For names */
  char          *(*get_name)(elfshobj_t *, void *obj);          /*! Get name */
  int	        (*set_name)(elfshobj_t *, void *, char *);      /*! Set name */
  
  /* For sections data */
  char		*(*get_data)(elfshsect_t *, u_int, u_int);	/*! Read data */
  int		(*set_data)(elfshsect_t*, u_int, char*, u_int, u_int); /* Write data */
  
  u_char        type;                                           /*! Object type */
}               revmL2_t;



/**
 * @brief ELFsh Level 1 object (= parent object) structure 
 */
typedef struct	s_L1handler
{
  hash_t	*l2list;					/*! A ptr on the child L2 hashtable */
  u_int		elem_size;					/*! Size of one element of this object */
  
  /* Handlers */
  void		*(*get_obj)(void *container, void *retarg);	/*! Read object and return size */
  void		*(*get_obj_idx)(void *f, eresi_Addr i, void *a); /*! Read handler for arrays objects */
  void		*(*get_obj_nam)(void *f, char *name);		/*! Read handler by name */
  void		*(*get_entptr)(void *f, eresi_Addr idx);	/*! Get address */
  eresi_Addr   	(*get_entval)(void *ptr);			/*! Get value */
  eresi_Addr   	(*set_entval)(void *ptr, eresi_Addr vaddr);     /*! Set value */
}		revmL1_t;




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

/* This structure stores the current state of FOREACH iteration (job specific) */
typedef struct		s_revmiter
{
  char			*curkey;	/* Key of currently processed variable */
  revmexpr_t		*curind;	/* Induction variable if any */
#define REVM_CONTAINER_HASH 1
#define REVM_CONTAINER_LIST 2
  char			tcontainer;	/* type of container */
  void			*container;	/* Current list (or hash) being iterated over */
#define REVM_IDX_UNINIT ((unsigned int) (-1))
  u_int		        elmidx;		/* Index of currently processed variable */
  u_int			reclevel;	/* Current recursion level on loop start */
  char			*end;		/* END loop label */
#define REVM_LOOP_UNKNOWN 0
#define REVM_LOOP_SIMPLE  1
#define REVM_LOOP_REGEX   2
#define REVM_LOOP_RANGE   3
  u_char		looptype;	/* Loop construct type */
  regex_t		*usedregx;	/* Regx */
}			revmiter_t;  

/* This structure stores the current state of a REWRITE transformation (job specific) */
typedef struct	      s_revmrewrite
{
  revmexpr_t	      *matchexpr;	/* Expression to rewrite */
  list_t	      *transformed;     /* List of produced expressions in latest transformation */
  u_int		      idloop;           /* Loop scope at which the rewrite happens */
  u_char	      matched;		/* Matched flag : just 0 or 1 depending on last try */
  u_char	      replaced;		/* Indicate if we have already transformed */
}		      revmrewrite_t;

/* This structure stores the current function recursion state for a job */
typedef struct		s_revmrecur
{
  char			*funcname;	/* Names of call-stack functions */
  revmargv_t		*script;	/* Script commands at this depth */
  revmargv_t		*lstcmd;	/* Last command at this depth */
  hash_t		labels;		/* Defined labels */
  hash_t		exprs;		/* Expressions */
  revmrewrite_t		rwrt;		/* "rewrite" matching context */  
}			revmrecur_t;

/* REVM job structure, one per client */
typedef struct        s_job
{
  u_int		      id;				/* Job identifier */
  revmworkspace_t     ws;				/* The job workspace */

  /* Job recursion and iteration contexts */
#define		      REVM_MAXSRCNEST  50
  revmrecur_t	      recur[REVM_MAXSRCNEST];		/* Recursion contexts */
  u_int               curscope;				/* Current recursion depth */
  revmiter_t	      iter[REVM_MAXSRCNEST];		/* "foreach" iteration context */
  u_int		      curloop;				/* Current iteration depth */

  /* Job files context */
  revmargv_t	      *curcmd;				/* Next command to be executed */
  hash_t              loaded;				/* List of loaded ELF objects */
  elfshobj_t          *curfile;				/* Current working ELF object */
  asm_processor       *proc;				/* Processor structure */

  /* Job debugger context */
  hash_t              dbgloaded;			/* List of objects loaded into e2dbg */
  elfshobj_t          *dbgcurrent;			/* Current working e2dbg file */
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
  asm_processor       proc_ia32;      /* Libasm Intel */
  asm_processor	      proc_sparc;     /* Libasm Sparc */
  asm_processor	      proc_mips;      /* Libasm Mips */
  asm_processor	      proc_arm;	      /* Libasm ARM */
  mjrsession_t        mjr_session;    /* Session holding contexts for mjollnir */
  int		      fifo_s2c;	      /* Fd for the debugger IO FIFO */
  int		      fifo_c2s;	      /* Fd for the debugger IO FIFO */
  void		      (*cmd_init)();  /* Command constructor from libstderesi */
}		      revmworld_t;

#endif
