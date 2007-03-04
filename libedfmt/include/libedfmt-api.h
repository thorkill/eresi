/*
** libedfmt-api.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Fev 02 2006 09:53:25 mxatone
**
*/

#ifndef __LIBEDFMT_API_H__
#define __LIBEDFMT_API_H__

#define EDFMT_NAME_SIZE 256

/* Declare structures needed */
struct			s_efile;
typedef struct		s_efile edfmtfile_t;

typedef struct 	       	s_etype
{
  char			name[EDFMT_NAME_SIZE];

#define EDFMT_TYPE_UNK	  0	/* Unknown type */
#define EDFMT_TYPE_BASIC  1	/* Basic type like: int, char, long ...*/
#define EDFMT_TYPE_ARRAY  2	/* Array with size and parent type */
#define EDFMT_TYPE_PTR	  3	/* Pointer type, that point to a parent type */
#define EDFMT_TYPE_STRUCT 4	/* Structure type with childs */
#define EDFMT_TYPE_UNION  5     /* Union type with childs */
#define EDFMT_TYPE_ATTR	  6	/* Structure attribute */
#define EDFMT_TYPE_VOID	  7	/* Void pointer */
#define EDFMT_TYPE_LINK	  8	/* Link type using parent */
  u_char		type;

  int			start;
  int			size;

  u_char       		parsed;

  struct s_etype       	*parent;   	/* Used in structure attribute, array and pointer */
  struct s_etype       	*child;   	/* Used in structure and attribute */
  struct s_etype       	*next;		/* Next element on a list */

  edfmtfile_t		*file;		/* Point to my file */
}			edfmttype_t;

typedef struct		s_evar
{
  char			name[EDFMT_NAME_SIZE];

#define EDFMT_SCOPE_UNK	    0
#define EDFMT_SCOPE_GLOBAL  1
#define EDFMT_SCOPE_FUNC    2
  u_char		scope;

  elfsh_Addr 		addr;		/* Global address */
  u_int 		reg;		/* Func reg base */
  int 			stackpos;	/* Func stack position */
  
  edfmttype_t		*type;

  struct s_evar		*next;
}			edfmtvar_t;

typedef struct		s_efunc_arg
{
  char			name[EDFMT_NAME_SIZE];

  u_int			reg;
  int			pos;

  edfmttype_t		*type;

  struct s_efunc_arg	*next;
}			edfmtfuncarg_t;

typedef struct		s_efunc
{
  char			name[EDFMT_NAME_SIZE];

  int			argc;
  edfmtfuncarg_t	*arguments;

  edfmttype_t		*rettype;

  elfsh_Addr		start;
  elfsh_Addr		end;

  struct s_efunc	*next;
}			edfmtfunc_t;


struct			s_efile
{
  char			name[EDFMT_NAME_SIZE];

  elfsh_Addr		start;
  elfsh_Addr		end;

  /* Types */
  edfmttype_t		*types;
  edfmttype_t		*ltype;
  hash_t		htype;

  /* Variables */
  edfmtvar_t		*vars;
  edfmtvar_t		*lvar;
  hash_t		hvar;

  /* Functions */
  edfmtfunc_t		*funcs;
  edfmtfunc_t		*lfunc;
  hash_t		hfunc;

  struct s_efile	*next;

  /* Intra file dependences */
  struct s_efile	*parent;
  struct s_efile	*child;
  struct s_efile	*lchild;
};

typedef struct		s_einfo
{
  edfmtfile_t		*files;
  edfmtfile_t		*lfile;

  /* Types */
  edfmttype_t		*types;
  edfmttype_t		*ltype;
  hash_t		htype;

  /* Variables */
  edfmtvar_t		*vars;
  edfmtvar_t		*lvar;
  hash_t		hvar;

  /* Function */
  edfmtfunc_t		*funcs;
  edfmtfunc_t		*lfunc;
  hash_t		hfunc;

  char 			*alloc_pool;
  int 			alloc_pos;
  int 			alloc_size;
}			edfmtinfo_t;

#define HASH_ADDX(_table, _name, _value) 			\
do {								\
  if (hash_get(_table, _name) != NULL)				\
    hash_set(_table, _name, (void *) _value);			\
  else								\
    hash_add(_table, strdup(_name), (void *) _value);		\
} while (0)

/* api.c */
int		edfmt_add_init(elfshobj_t *file);
int		edfmt_add_end();
int		edfmt_update_type(edfmttype_t *type);
int		edfmt_update_var(edfmtvar_t *var);
int		edfmt_update_func(edfmtfunc_t *func);
int		edfmt_change_type_nfile(edfmttype_t *type);
edfmtfile_t   	*edfmt_restore_parent_file();
edfmtfile_t	*edfmt_get_current_file();
edfmtfile_t	*edfmt_get_inc_file(edfmtfile_t *file, char *name);
edfmtfile_t    	*edfmt_add_file(edfmtfile_t *parent_file, char *name, elfsh_Addr start, elfsh_Addr end);
int		edfmt_reset_file();
int		edfmt_reactive_file();
edfmtinfo_t    	*edfmt_get_uniinfo(elfshobj_t *file);
edfmttype_t	*edfmt_add_type_unk(char *name);
edfmttype_t	*edfmt_add_type_basic(char *name, int size);
edfmttype_t	*edfmt_add_type_array(char *name, int size, edfmttype_t *type);
edfmttype_t	*edfmt_add_type_ptr(char *name, edfmttype_t *type);
edfmttype_t	*edfmt_add_type_struct(char *name, int size);
edfmttype_t	*edfmt_add_type_union(char *name, int size);
edfmttype_t	*edfmt_add_type_attr(edfmttype_t *tstruct, char *name, 
				     int start, int size, edfmttype_t *type);
edfmttype_t	*edfmt_add_type_void(char *name);
edfmttype_t	*edfmt_add_type_link(char *name, edfmttype_t *type);

edfmtvar_t	*edfmt_add_var_global(edfmttype_t *type, char *name, 
				      elfsh_Addr addr);
edfmtfunc_t	*edfmt_add_func(char *name, edfmttype_t *ret, 
				elfsh_Addr start, elfsh_Addr end);
edfmtfuncarg_t	*edfmt_add_arg(edfmtfunc_t *func, char *name,
			       u_int reg, int pos, edfmttype_t *type);
edfmttype_t	*edfmt_check_type(char *name);
edfmtvar_t	*edfmt_check_var(char *name);
edfmtfunc_t	*edfmt_check_func(char *name);

#endif
