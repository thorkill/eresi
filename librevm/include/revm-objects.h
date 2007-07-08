/*
** revm.h for librevm in elfsh
** 
** Started on  Thu Feb 22 07:19:04 2001 mayhem
** 
** Moved from elfsh to librevm on January 2007 -may
**
**
** $Id: revm-objects.h,v 1.2 2007-07-08 00:28:31 may Exp $
**
*/
#ifndef __REVM_OBJECTS_H_
 #define __REVM_OBJECTS_H_


/* Meta-object : Abstract object description when reflected */
typedef	struct		s_revm_object
{

  /* Access handlers */
  elfsh_Addr          (*get_obj)(void *parent);
  int	              (*set_obj)(void *parent, elfsh_Addr value);
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
  char                immed;          /* Immediate binary flag */
  char                perm;	      /* TRUE if obj is a script variable */

  /* Type information */
  u_int               type;	      /* The object type identifier */

  /* Only when describing elements of hash tables */
  char		      *hname;		/* Name of parent hash table */
  char		      *kname;		/* Name of key in hash table */

  /* Immediate value if immed flag is set */
  union               immval
  {
    u_char            byte;
    u_short           half;
    u_int             word;
    elfsh_Addr        ent;
    char              *str;
  }                   immed_val;
}                     revmobj_t;


/* Structure for an expression in ERESI : A tree of revmobj_t */
typedef struct		s_revm_expr
{
  char			*label;		/* Field label */
  u_int			typeid;		/* Type ID of current expression object */
  char			*strval;	/* ASCII form of current expression object */
  revmobj_t		*value;		/* The meta-object, if terminal */
  struct s_revm_expr	*childs;	/* Child objects list, if non-terminal */
  struct s_revm_expr	*next;		/* Next object if in record */
}			revmexpr_t;


/* Generic structure for objects program annotations in ERESI */
typedef struct 		s_revm_annotation
{
#define EDFMT_SCOPE_UNK    0
#define EDFMT_SCOPE_GLOBAL 1
#define EDFMT_SCOPE_FUNC   2
  u_char 		scope;		/* XXX: Unused for now ? */
  elfsh_Addr 		addr; 		/* Global address */
  revmexpr_t		*expr;		/* Available expression if any */
  u_int			typenum;	/* Type id */
  u_int 		reg;   		/* Function reg id base (stack) */
  int 			relvalue;     	/* Relative value based on reg */
  int			nameoff;	/* Name offset in string table */
					/* XXX: should be a symbol instead */
}			revmannot_t;



/**********************************************************************************/
/****** Those 2 objects are reminicescence of the old object system of elfsh ******/
/**** Need to disapear to use only revmannot/revmobj/aspectype instead of those *****/
/**************** This only affects ERESI types of ELF structures ******************/
/**********************************************************************************/


/* REVM Level 2 (= child of L1) object structure */
typedef struct	s_L2handler
{

  /* For fields */
  elfsh_Addr	(*get_obj)(void *obj);                          /* Read object */
  int		(*set_obj)(void *par, elfsh_Addr arg);          /* Write object */
  
  /* For names */
  char          *(*get_name)(elfshobj_t *, void *obj);          /* Get name */
  int	        (*set_name)(elfshobj_t *, void *, char *);      /* Set name */
  
  /* For sections data */
  char		*(*get_data)(elfshsect_t *, u_int, u_int);	/* Read data */
  int		(*set_data)(elfshsect_t*, u_int, char*, u_int, u_int); /* Write data */
  
  u_char        type;                                           /* Object type */
}               revmL2_t;



/* ELFsh Level 1 object (= parent object) structure */
typedef struct	s_L1handler
{
  hash_t	*l2list;					/* A ptr on the child L2 hashtable */
  u_int		elem_size;					/* Size of one element of this object */
  
  /* Handlers */
  void		*(*get_obj)(void *container, void *retarg);	/* Read object and return size */
  void		*(*get_obj_idx)(void *f, elfsh_Addr i, void *a); /* Read handler for arrays objects */
  void		*(*get_obj_nam)(void *f, char *name);		/* Read handler by name */
  void		*(*get_entptr)(void *f, elfsh_Addr idx);	/* Get address */
  elfsh_Addr   	(*get_entval)(void *ptr);			/* Get value */
  elfsh_Addr   	(*set_entval)(void *ptr, elfsh_Addr vaddr);     /* Set value */
}		revmL1_t;



#endif
