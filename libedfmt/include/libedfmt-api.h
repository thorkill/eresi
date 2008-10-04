/*
** libedfmt-api.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Fev 02 2006 09:53:25 mxatone
**
**
** $Id: libedfmt-api.h,v 1.11 2007-06-02 08:28:51 mxatone Exp $
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
  unsigned char		type;

  int			start;
  int			tmp_ssize;	/* Temporary structure size */
  int			size;

  /* Flags */
  unsigned char		valid;
  unsigned char       		parsed;

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
  unsigned char		scope;

  eresi_Addr 		addr;		/* Global address */
  unsigned int 		reg;		/* Func reg base */
  int 			stackpos;	/* Func stack position */
  
  edfmttype_t		*type;

  struct s_evar		*next;
}			edfmtvar_t;

typedef struct		s_efunc_arg
{
  char			name[EDFMT_NAME_SIZE];

  unsigned int			reg;		/* Reg base */
  int			pos;		/* Reg or stack position */

  edfmttype_t		*type;		/* Argument type */

  struct s_efunc_arg	*next;
}			edfmtfuncarg_t;

typedef struct		s_efunc
{
  char			name[EDFMT_NAME_SIZE];

  int			argc;		/* Argument number */
  edfmtfuncarg_t	*arguments;	/* Argument list */

  edfmttype_t		*rettype;	/* Return type */

  eresi_Addr		start;		/* Start address */
  eresi_Addr		end;		/* End address */

  struct s_efunc	*next;
}			edfmtfunc_t;


struct			s_efile
{
  char			name[EDFMT_NAME_SIZE];

  eresi_Addr		start;		/* Start address */
  eresi_Addr		end;		/* End address */

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
  /* File list */
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

  /* Allocated pool */
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

#endif
