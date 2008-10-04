/*
** libedfmt-stabs.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Sat 13 2006 09:52:12 mxatone
**
**
** $Id: libedfmt-stabs.h,v 1.9 2007-08-17 01:45:46 heroine Exp $
**
*/

#ifndef __STABS_H__
#define __STABS_H__

#define STABS_MAX_ARGUMENTS 20
#define STABS_NAME_SIZE     90
#define STABS_CTYPENUM_SIZE 20
#define STABS_FILE_IT       20

/* Some define shortcut */
#define STABS_IVD_STR(_str) (!*_str || !**_str)

#define stabs_c_ent \
stabs_ent[stabs_index]

#define stabs_c_str \
(stabs_datastr + stabs_c_ent.strindex)

/**
 * @brief Represent an entrie on the stab section 
 */
typedef struct	s_stabsent
{
  eresi_Addr   	strindex; 	/*!< The index to find the string on the string stab section */
  unsigned char       	type;		/*!< Type id */
  unsigned char	other;		/*!< Never use, already @ 0 */
  u_short	desc;		/*!< Used to store short information like line */
  eresi_Addr  	value;		/*!< Used to store addr aligned informations */
}	     	edfmtstabsent_t;

/* Forward declaration */
struct s_edfmtbstabsdata;
typedef struct s_stabsdata edfmtstabsdata_t;
struct s_stabstype;
typedef struct s_stabstype edfmtstabstype_t;

/**
 * @brief A type is defined as two number, file and type number 
 */
typedef struct 	s_stabstypenum
{
  long		file;			/*!< TO COMPLETE */
  long		number;			/*!< TO COMPLETE */
}		edfmtstabstypenum_t;

/**
 * @brief A structure attribute 
 */
typedef struct	s_stabsattr
{
  char		name[STABS_NAME_SIZE];	/*!< TO COMPLETE */

  edfmtstabstype_t *type;


  long		start;			/*!< Start offset */
  long		size;			/*!< Start size */

  struct s_stabsattr *next;		/*!< TO COMPLETE */
}		edfmtstabsattr_t;

/**
 * @brief Describe a structure 
 */
typedef struct 	s_stabsstruct
{
  
  long		size;		/*!< Total size */

  
  edfmtstabsattr_t *attr;	/*!< Attribute list */
}		edfmtstabsstruct_t;

/**
 * @brief Describe an enum 
 */
typedef struct 	s_stabsenum
{
  char		name[STABS_NAME_SIZE];
  long		value;

  struct s_stabsenum *next;
}		edfmtstabsenum_t;

/** 
 * In stabs, a basic type is describe by its range 
 * We get those informations (except too long numbers)
 * only for parsing purpore, resolve basic type size
 * should use name matching engine 
 */
typedef struct	s_stabsrange
{
  long 		low;
  long 		high;

  /* We don't always have size information */
  long		size;
}		edfmtstabsrange_t;

/**
 * @brief Describe an array 
 */
typedef struct	s_stabsarray
{
  long		low;
  long		high;

#define 	STABS_ARRAY_STRING (1 << 1)
#define 	STABS_ARRAY_VECTOR (1 << 2)
  long		type;

  edfmtstabstype_t *link;
}		edfmtstabsarray_t;

/**
 * @brief Represent a function 
 */
typedef struct 	s_stabsfunc
{
  unsigned int		s_line; /* Start line */
  eresi_Addr	s_addr; /* Start address */
  eresi_Addr	e_addr; /* End address */

  edfmtstabsdata_t *args[STABS_MAX_ARGUMENTS];

  /* Return type */
  edfmtstabstype_t *rettype;

  /* Data parent */
  edfmtstabsdata_t *data;

  /* Next entrie */
  struct s_stabsfunc *next;
}		edfmtstabsfunc_t;

/** 
 * @brief Describe a type, a type isn't always basic 
 */
struct		s_stabstype
{
#define STABS_TYPE_UNK 	     0
#define STABS_TYPE_STRUCT    1
#define STABS_TYPE_UNION     2
#define STABS_TYPE_LINK	     3
#define STABS_TYPE_RANGE     4
#define STABS_TYPE_ARRAY     5
#define STABS_TYPE_VOID	     6
#define STABS_TYPE_ENUM      7
#define STABS_TYPE_CMEMBER   8
#define STABS_TYPE_PTR	     9
#define STABS_TYPE_REF	    10
#define STABS_TYPE_FUNC	    11
#define STABS_TYPE_VOLATILE 12
#define STABS_TYPE_CONST    13
#define STABS_TYPE_CLINK    14
  unsigned char       	type;

  edfmtstabstypenum_t num;
  char cnum[STABS_CTYPENUM_SIZE];

  /**
   * Depend of the field type, we will use on of this union
   * entry 
   */
  union {
    edfmtstabsstruct_t struct_union;
    edfmtstabsenum_t *enum_attr;
    struct s_stabstype *link;
    edfmtstabsrange_t r;
    edfmtstabsarray_t arr;
  } u;

  /* Data parent */
  edfmtstabsdata_t *data;
  struct s_stabstype *parent_link;

  /* Transformated type link */
  edfmttype_t *transtype;

  struct s_stabstype *next;
};

/**
 * Data structure represent a scoped type
 * there is many type apply on different scope:
 * function, variables, etc etc .. 
 */
struct		s_stabsdata
{
  char 		name[STABS_NAME_SIZE];

#define STABS_SCOPE_UNK	         0
#define STABS_SCOPE_TYPE         1
#define STABS_SCOPE_LVAR         2
#define STABS_SCOPE_ARG	         3
#define STABS_SCOPE_GFUNC        4
#define STABS_SCOPE_LFUNC        5
#define STABS_SCOPE_GVAR         6
#define STABS_SCOPE_ARGRREG      7
#define STABS_SCOPE_ARGREF       8
#define STABS_SCOPE_CONST_F      9
#define STABS_SCOPE_CONST_I      10
#define STABS_SCOPE_CONST_E      11
#define STABS_SCOPE_ARGREG       12
#define STABS_SCOPE_VARREG       13
#define STABS_SCOPE_STATICVAR    14
#define STABS_SCOPE_PSTATICVAR   15
  unsigned char	scope;

  edfmtstabstype_t *type;

  /* This union depend of the scope field */
  union {
    eresi_Addr addr;
    eresi_Addr stackpos;
    eresi_Addr reg;
    edfmtstabsfunc_t func;
    double fl;
    int i;
  } u;

  struct s_stabsdata *next;
};

/** 
 * Describe a relation between a line and an address 
 */
typedef struct	s_stabsline
{
  unsigned int		line;
  eresi_Addr	addr;

  struct s_stabsline *next;
}		edfmtstabsline_t;

/**
 * @brief Describe a file
 *
 * There is two different types of files so (object file) and include (header file) 
 * - an object file wouldn't have any parent
 * - an include file can have others include file but always have a parent 
 */
typedef struct 	s_stabsfile
{
  /**
   * Path and file, and include file wouldn't have a path, 
   * an absolute path is on file field 
   */
  char	      	*path;
  char		*file;

  eresi_Addr	s_addr;
  eresi_Addr	e_addr;

  /* Function pointers */
  edfmtstabsfunc_t *func;
  edfmtstabsfunc_t *last_func;

  /* Lines pointers */
  edfmtstabsline_t *line;
  edfmtstabsline_t *last_line;

  /* Variable */
  edfmtstabsdata_t *vars;
  edfmtstabsdata_t *lvar;

  /* Types */
  edfmtstabstype_t *types;
  edfmtstabstype_t *ltype;

  /* Include & parent pointers */
  struct s_stabsfile *inc;
  struct s_stabsfile *last_inc;

  struct s_stabsfile *parent;

  /* This field is use both by so and include files */
  struct s_stabsfile *next;
}		edfmtstabsfile_t;

/**
 * This structure is the information center 
 */
typedef struct	s_stabsinfo
{
  /* Files informations */
  char 		**dir_list;
  char 		**file_list;
  unsigned int 	index_list;
  unsigned int 	num_list;

  char 		*alloc_pool;
  int 		alloc_pos;
  int 		alloc_size;

  /* File list */
  edfmtstabsfile_t *file;
}		edfmtstabsinfo_t;

enum stabs_type
  {
    STABS_TYPE_GSYM = 0x20,
    STABS_TYPE_FNAME = 0x22,
    STABS_TYPE_FUN = 0x24,
    STABS_TYPE_STSYM = 0x26,
    STABS_TYPE_LCSYM = 0x28,
    STABS_TYPE_MAIN = 0x2a,
    STABS_TYPE_ROSYM = 0x2c,
    STABS_TYPE_PC = 0x30,
    STABS_TYPE_NSYMS = 0x32,
    STABS_TYPE_NOMAP = 0x34,
    STABS_TYPE_OBJ = 0x38,
    STABS_TYPE_OPT = 0x3c,
    STABS_TYPE_RSYM = 0x40,
    STABS_TYPE_M2C = 0x42,
    STABS_TYPE_SLINE = 0x44,
    STABS_TYPE_DSLINE = 0x46,
    STABS_TYPE_BSLINE = 0x48,
    //STABS_TYPE_BROWS = 0x48, ??
    STABS_TYPE_DEFD = 0x4a,
    STABS_TYPE_FLINE = 0x4c,
    STABS_TYPE_EHDECL = 0x50,
    //STABS_TYPE_MOD2 = 0x50, ??
    STABS_TYPE_CATCH = 0x54,
    STABS_TYPE_SSYM = 0x60,
    STABS_TYPE_ENDM = 0x62,
    STABS_TYPE_SO = 0x64,
    STABS_TYPE_LSYM = 0x80,
    STABS_TYPE_BINCL = 0x82,
    STABS_TYPE_SOL = 0x84,
    STABS_TYPE_PSYM = 0xa0,
    STABS_TYPE_EINCL = 0xa2,
    STABS_TYPE_ENTRY = 0xa4,
    STABS_TYPE_LBRAC = 0xc0,
    STABS_TYPE_EXCL = 0xc2,
    STABS_TYPE_SCOPE = 0xc4,
    STABS_TYPE_RBRAC = 0xe0,
    STABS_TYPE_BCOMM = 0xe2,
    STABS_TYPE_ECOMM = 0xe4,
    STABS_TYPE_ECOML = 0xe8,
    STABS_TYPE_WITH = 0xea,
    STABS_TYPE_NBTEXT = 0xf0,
    STABS_TYPE_NBDATA = 0xf2,
    STABS_TYPE_NBBSS = 0xf4,
    STABS_TYPE_NBSTS = 0xf6,
    STABS_TYPE_NBLCS = 0xf8
  };

#define STABS_IS_SPECIAL(_x) \
(_x == '-' || _x == '(' || (_x >= '0' && _x <= '9'))

#define STABS_STR_DELIM 	':'

/* Symbol type */
#define STABS_STR_T_PARAM_RREG 	'a'
#define STABS_STR_T_CONST	'c'
#define STABS_STR_T_CONST_FLOAT	'r'
#define STABS_STR_T_CONST_INT	'i'
#define STABS_STR_T_CONST_ENUM	'e'
#define STABS_STR_T_FFUNC	'f'
#define STABS_STR_T_GFUNC	'F'
#define STABS_STR_T_GVAR	'G'
#define STABS_STR_T_ICONST     	'i'
#define STABS_STR_T_ARGLIST	'p'
#define STABS_STR_T_PARAM_REG	'R'
#define STABS_STR_T_VAR_REG	'r'
#define STABS_STR_T_VAR_FSCOPE	'S'
#define STABS_STR_T_VAR_LOCAL	's'
#define STABS_STR_T_TYPE_NAME	't'
#define STABS_STR_T_TYPE_STRUCT	'T'
#define STABS_STR_T_PARAM_REF	'v'
#define STABS_STR_T_PROC_SVAR	'V'

/* Type descriptor */
#define STABS_STR_D_METHOD	'#'
#define STABS_STR_D_PTR		'*'
#define STABS_STR_D_REF		'&'
#define STABS_STR_D_ATTR	'@'
#define STABS_STR_D_ARRAY	'a'
#define STABS_STR_D_VOLATILE	'B'
#define STABS_STR_D_ENUM	'e'
#define STABS_STR_D_TFUNC	'f'
#define STABS_STR_D_CONST	'k'
#define STABS_STR_D_RANGE	'r'
#define STABS_STR_D_STRUCT	's'
#define STABS_STR_D_BITSTRING	'S'
#define STABS_STR_D_UNION	'u'
#define STABS_STR_D_CREF	'x'

/* AIX attributes */
#define STABS_STR_AIX_SIZE	's'
#define STABS_STR_AIX_STRING	'S'
#define STABS_STR_AIX_VECTOR	'V'

/* Usefull local functions */
edfmtstabstype_t *edfmt_stabs_type(char **str, char *link);
edfmtstabsdata_t *edfmt_stabs_data(char **str);

#endif
