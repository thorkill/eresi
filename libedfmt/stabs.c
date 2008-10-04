/**
 * @defgroup libedfmt Libedfmt : The Elf debug format library of ELFsh
 */
/*
** @file stabs.c
** @ingroup libedfmt
**
** Started Jan 01 2007 21:30:13 mxatone
**
**
** $Id: stabs.c,v 1.18 2007-06-02 08:28:51 mxatone Exp $
**
*/

#include "libedfmt.h"

/**
 * This file implements stab support for libedfmt.
 * specificitation @ http://wwwcdf.pd.infn.it/localdoc/stabs.pdf 
 * -
 * This version of stabs format parsing has been approved for x86
 * and gentoo linux, some bugs can append on Sparc, more test will
 * be done to make this work correctly 
 * @file stabs.c
 */

#define STABS_HNAME_TYPE_REF 		"stabs_type_reference"
#define STABS_HNAME_TYPE_CROSS_REF 	"stabs_type_reference"
#define STABS_HNAME_RES_SIZE 		"stabs_resolve_size"

/**
 * This function is called a lot of time, we win some time 
 */
#define STABS_DATA(_str) (!STABS_IVD_STR(_str) ? edfmt_stabs_data(_str) : NULL)

/**
 * Temporary structure for the next table 
 */
typedef struct s_res_size
{
  char *name;
  long size;
}	       edfmt_res_size_t;

/**
 * This table store size for every basic elements
 * Stabs doens't get size information but range information that are
 * hard to parse and can create some bug, then we won't use it
 */
edfmt_res_size_t resolved_table[] =
  {
    {"int"                   , sizeof(int)			},
    {"char"                  , sizeof(char)			},
    {"long int"              , sizeof(long int)			},
    {"unsigned int"          , sizeof(unsigned int)		},
    {"long unsigned int"     , sizeof(long unsigned int)	},
    {"long long int"         , sizeof(long long int)		},
    {"long long unsigned int", sizeof(long long unsigned int)	},
    {"short int"             , sizeof(short int)		},
    {"short unsigned int"    , sizeof(short unsigned int)	},
    {"signed char"           , sizeof(signed char)		},
    {"unsigned char"         , sizeof(unsigned char)		},
    {"float"                 , sizeof(float)			},
    {"double"                , sizeof(double)			},
    {"long double"           , sizeof(long double)		},
    { NULL		     , 0 				}
  };

/**
 * Local resolv table 
 */
hash_t resolved_size;

/* Global position variables */
elfshsect_t *stabs_sect;
elfshsect_t *stabs_sect_str;

edfmtstabsent_t *stabs_ent;

/* Index */
unsigned int stabs_index;
unsigned int stabs_num;
char *stabs_datastr;

/* File structure pointers */
edfmtstabsfile_t *root_file = NULL;
edfmtstabsfile_t *current_file = NULL;
edfmtstabsfile_t *last_file = NULL;

/**
 * Current function we are in 
 */
edfmtstabsfunc_t *current_func = NULL;

/* Temporary hash table used for type resolution */
hash_t type_ref;
hash_t type_cross_ref;

/**
 * Contain main informations 
 */
edfmtstabsinfo_t *stabs_info = NULL;

/**
 * Macro to gain time when fetch allocation pointer from the stabs pool
 */
#define STABS_GETPTR(_var, _size, _ret) 	      	\
do {						     	\
  _var = edfmt_alloc_pool(&(stabs_info->alloc_pool), 	\
		       &(stabs_info->alloc_pos), 	\
		       &(stabs_info->alloc_size), 	\
		       STABS_ALLOC_STEP, _size); 	\
if (_var == NULL)					\
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 	\
		 "Pool allocation failed", _ret);     	\
} while (0)


/**
 * Actual parsed file 
 */
elfshobj_t *afile = NULL;

/**
 * Retreive stabs info pointer 
 * @param file retrieve stabs information from this file
 */
edfmtstabsinfo_t 	*edfmt_stabs_getinfo(elfshobj_t *file)
{
  edfmtstabsinfo_t 	*fstabs = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  fstabs = (edfmtstabsinfo_t *) file->debug_format.stabs;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, fstabs);
}

/**
 * Init resolved_size hash table table 
 */
static int		edfmt_stabs_initrestable()
{
  int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (resolved_size.ent == NULL)
    {
      hash_init(&resolved_size, STABS_HNAME_RES_SIZE, 20, ASPECT_TYPE_UNKNOW);  

      for (index = 0; resolved_table[index].name != NULL; index++)
	hash_add(&resolved_size, strdup(resolved_table[index].name), 
		 (void *) &(resolved_table[index]));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Update a cross reference 
 * @param type watch if we got a type waiting for this type
 * @param token help to make an unique search name
 */
static int		edfmt_stabs_update_cref(edfmtstabstype_t *type, unsigned char token)
{
  char			buf[BUFSIZ];
  edfmtstabstype_t 	*seek_type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (type == NULL || type->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);
  
  snprintf(buf, BUFSIZ - 1, "%c%s", token, type->data->name);

  seek_type = (edfmtstabstype_t *) hash_get(&type_cross_ref, buf);

  if (seek_type)
    {
      seek_type->u.link = type;
      hash_del(&type_cross_ref, buf);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Add a new file on the list 
 * @param dir new directory
 * @parma file new filename
 */
int	    		edfmt_stabs_addfile(char *dir, char *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (stabs_info == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid stabs_info", -1);

  /* At the end of the list ? */
  if (stabs_info->index_list >= stabs_info->num_list)
    {
      stabs_info->num_list += STABS_FILE_IT;
      if (stabs_info->dir_list == NULL)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,stabs_info->dir_list, stabs_info->num_list * sizeof(char *), -1);
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,stabs_info->file_list, stabs_info->num_list * sizeof(char *), -1);
	}
      else
	{
	  XREALLOC(__FILE__, __FUNCTION__, __LINE__,stabs_info->dir_list, stabs_info->dir_list, 
		   stabs_info->num_list * sizeof(char *), -1);
	  XREALLOC(__FILE__, __FUNCTION__, __LINE__,stabs_info->file_list, stabs_info->file_list, 
		   stabs_info->num_list * sizeof(char *), -1);
	}
    }

  /* Update list pointers */
  stabs_info->dir_list[stabs_info->index_list] 	= dir;
  stabs_info->file_list[stabs_info->index_list] = file;

  stabs_info->index_list++;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Parse a function 
 * @param func store information on this structure 
 * @param str pointer on the string to parse
 */
int     		edfmt_stabs_func(edfmtstabsfunc_t *func, char **str)
{
  char			*my_str;
  unsigned int			arg_index = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (func == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* End of the current function ? */
  if (STABS_IVD_STR(str))
    {
      if (current_func)
	{
	  current_func->e_addr = func->s_addr + stabs_c_ent.value;
	  current_func = NULL;
	}

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  if (current_file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid current_file", NULL);

  current_func = func;

  /* Fill start infos */
  func->s_addr = stabs_c_ent.value;
  func->s_line = stabs_c_ent.desc;

  func->rettype = edfmt_stabs_type(str, NULL);

  /* Update pointer list */
  if (current_file->func == NULL)
    current_file->func = func;
  else
    current_file->last_func->next = func;

  current_file->last_func = func;

  /* Parse every param symbols next entries */
  for(stabs_index = stabs_index + 1;stabs_index < stabs_num; stabs_index++)
    {
      if (stabs_c_ent.type != STABS_TYPE_PSYM)
	{
	  stabs_index--;
	  break;
	}
      
      /* Ok we have a max arguments, but if you got more than 20 arguments, blame yourself !! */
      if (arg_index < STABS_MAX_ARGUMENTS)
	{
	  my_str = stabs_c_str;
	  func->args[arg_index++] = STABS_DATA(&my_str);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Parse a type range 
 * @param type type structure to fill
 * @param str pointer on the string to parse
 */
int	 	     	edfmt_stabs_range(edfmtstabstype_t *type, char **str)
{
  edfmtstabstype_t 	*rtype = NULL;
  long			low, high;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == type || STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  /* First we got a type */
  rtype = edfmt_stabs_type(str, NULL);

  if (**str == ';')
    (*str)++;

  low = high = 0;

  /* Then a low range number */
  edfmt_stabs_readnumber(str, ';', &low);

  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1);

  (*str)++;

  /* And a high range number */
  edfmt_stabs_readnumber(str, ';', &high);

  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1);

  (*str)++;

  /* Fill range felds, but we will use only link (mainly) */
  type->type = STABS_TYPE_RANGE;
  type->u.link = rtype;

  /**
   * We don't read correctly very big numbers as we didn't need them
   * but we still save those information
   */
  type->u.r.low = low;
  type->u.r.high = high;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Parse an array 
 * @param type structure to fill
 * @param str pointer on the string to parse
 * @param isString indicate if this array is a string
 * @param isVector indicate if this array is a vector
 */
int	      		edfmt_stabs_array(edfmtstabstype_t *type, char **str, 
				  unsigned char isString, unsigned char isVector)
{
  edfmtstabstype_t 	*rtype;
  long			low, high;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == type || STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  low = high = 0;

  if (**str == 'r')
    (*str)++;

  /* This type describe how index is managed .. we don't need it */
  edfmt_stabs_type(str, NULL);

  if (**str == ';')
    (*str)++;

  /* Should be zero */
  edfmt_stabs_readnumber(str, ';', &low);
  
  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1);
  
  (*str)++;

  /* Represent the size */
  edfmt_stabs_readnumber(str, ';', &high);
  
  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1); 

  (*str)++;

  /* Describe array type */
  rtype = edfmt_stabs_type(str, NULL);

  type->u.arr.low = low;
  type->u.arr.high = high;
  type->u.arr.link = rtype;

  if (isString)
    type->u.arr.type = STABS_ARRAY_STRING;

  if (isVector)
    type->u.arr.type |= STABS_ARRAY_VECTOR;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  
}

/**
 * Read a structure 
 * @param tstruct structure to fill
 * @param str pointer on the string to parse
 */
int	      		edfmt_stabs_struct(edfmtstabsstruct_t *tstruct, char **str)
{
  edfmtstabsattr_t 	*tattr = NULL, *lattr = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == tstruct || STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  /* Read structure size */
  edfmt_stabs_readnumber(str, 0, &tstruct->size);

  do {
    /* Allocate for a member */
    STABS_GETPTR(tattr, sizeof(edfmtstabsattr_t), -1);

    /* Update link between elements */
    if (tstruct->attr == NULL)
      tstruct->attr = tattr;
    else
      lattr->next = tattr;

    lattr = tattr;
    
    /* Member name */
    edfmt_stabs_readstr(tattr->name, STABS_NAME_SIZE, str, ':');

    if ((*str)[-1] != ':')
      break;

    /* Member type */
    tattr->type = edfmt_stabs_type(str, NULL);
    
    if (**str != ',')
      break;

    (*str)++;

    /* Start at */
    edfmt_stabs_readnumber(str, ',', &tattr->start);
    tattr->start /= 8;

    if (**str != ',')
      break;

    (*str)++;

    /* End at */
    edfmt_stabs_readnumber(str, ';', &tattr->size);
    tattr->size /= 8;

    if (**str != ';')
      break;

    (*str)++;
  } while (**str != ';');

  (*str)++;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Read an enum table 
 * @param str pointer on the string to parse
 * @return created structure that store enum informations
 */
edfmtstabsenum_t 	*edfmt_stabs_enum(char **str)
{
  edfmtstabsenum_t 	*root_attr = NULL;
  edfmtstabsenum_t 	*enum_attr = NULL;
  edfmtstabsenum_t 	*last_attr = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

  while (**str != ';')
    {
      STABS_GETPTR(enum_attr, sizeof(edfmtstabsenum_t), NULL);

      if (root_attr == NULL)
	root_attr = enum_attr;

      if (last_attr)
	last_attr->next = enum_attr;

      last_attr = enum_attr;

      /* Read name */
      edfmt_stabs_readstr(enum_attr->name, STABS_NAME_SIZE, str, ':');

      if ((*str)[-1] != ':')
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Wrong string pattern", NULL);

      /* Read value */
      if(edfmt_stabs_readnumber(str, ',', &enum_attr->value) != 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Wrong string pattern", NULL);

      if (**str == ',')
	(*str)++;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, root_attr);
}

/** 
 * Read a type, that an important fonction.
 * Stabs use type in an embedded form, a type can be declare in the center
 * of a structure
 * @param str pointer on the string to parse
 * @return the generated type structure or an existing structure
 */
edfmtstabstype_t 	*edfmt_stabs_type(char **str, char *link)
{
  char			*save_str;
  char			token;
  edfmtstabstypenum_t 	tnum;
  edfmtstabstype_t	*type;
  char			ctypenum[STABS_CTYPENUM_SIZE];
  char			name[STABS_NAME_SIZE];
  long			tsize = 0;
  unsigned char       		isString = 0;
  unsigned char		isVector = 0;
  int			addtype = 1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

  tnum.file = tnum.number = 0;

  /* Default case */
  if (link)
    *link = 0;

  /* Retrieve the type number */
  if (STABS_IS_SPECIAL(**str))
    {
      edfmt_stabs_typenum(&tnum, str);

      if (**str != '=')
	{
	  edfmt_stabs_ctypenum(ctypenum, STABS_CTYPENUM_SIZE, &tnum);
	  type = hash_get(&type_ref, ctypenum);

	  if (link)
	    *link = 1;

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
	}

      (*str)++;
    }
  else
    {
      /* Anonymous type */
      tnum.file = tnum.number = -1;
    }

  STABS_GETPTR(type, sizeof(edfmtstabstype_t), NULL);
  type->num = tnum;

 typestart:
  token = *(*str)++;

  /* Parse type descriptor */
  if (STABS_IS_SPECIAL(token))
    {
      (*str)--;
      save_str = *str;
      edfmt_stabs_typenum(&tnum, str);

      /* VOID case */
      if (**str != '=' && type->num.file >= 0
	  && type->num.file == tnum.file
	  && type->num.number == tnum.number)
	{
	  type->type = STABS_TYPE_VOID;
	}
      else
	{
	  /* Restore string position */
	  *str = save_str;
	  type->type = STABS_TYPE_LINK;
	  type->u.link = edfmt_stabs_type(str, NULL);
	  if (type->u.link)
	    type->u.link->parent_link = type;
	}
    }
  else
    {
      /**
       * Stabs is a really strange and difficult format 
       * You should read the documentation, you'll see that stabs can
       * be different depending of the architecture, which create a lot of issue.
       */
      type->type = STABS_TYPE_UNK;
      switch(token)
	{
	case STABS_STR_D_METHOD:
	  /**
	   * We don't support C++ for the moment 
	   * as clear representation isn't set for 
	   * the moment, we will see 
	   */
	  break;
	case STABS_STR_D_PTR:
	case STABS_STR_D_REF:
	case STABS_STR_D_TFUNC:
	case STABS_STR_D_VOLATILE:
	case STABS_STR_D_CONST:
	  switch(token)
	    {
	    case STABS_STR_D_PTR:
	      type->type = STABS_TYPE_PTR;
	      break;
	    case STABS_STR_D_REF:
	      type->type = STABS_TYPE_REF;
	      break;
	    case STABS_STR_D_TFUNC:
	      type->type = STABS_TYPE_FUNC;
	      break;
	    case STABS_STR_D_VOLATILE:
	      type->type = STABS_TYPE_VOLATILE;
	      break;
	    case STABS_STR_D_CONST:
	      type->type = STABS_TYPE_CONST;
	      break;
	    }
	  type->u.link = edfmt_stabs_type(str, NULL);
	  break;
	case STABS_STR_D_ATTR:
	  /* GNU C++ way - Class member*/
	  if (STABS_IS_SPECIAL(**str))
	    {
	      /** 
	       * We don't support C++ for the moment 
	       * as clear representation isn't set for 
	       * the moment, we will see 
	       */
	      break;
	    }
	  else
	    {
	      /* AIX way - Attribute */
	      switch (*(*str)++)
		{
		case STABS_STR_AIX_SIZE:
		  edfmt_stabs_readnumber(str, ';', &tsize);
		  
		  if (**str != ';')
		    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
				      "Invalid AIX attribute", NULL);

		  (*str)++;		  
		  break;
		case STABS_STR_AIX_STRING:
		  isString = 1;
		  break;
		case STABS_STR_AIX_VECTOR:
		  isVector = 1;
		  break;
		}
	      goto typestart;
	    }
	  break;
	case STABS_STR_D_CREF:
	  /* We get the name from with the first part, then we can search the correct type */
	  type->type = STABS_TYPE_CLINK;
	  edfmt_stabs_readstr(name, STABS_NAME_SIZE, str, STABS_STR_DELIM);
	  HASH_ADDX(&type_cross_ref, name, type);
	  break;
	case STABS_STR_D_ARRAY:
	  type->type = STABS_TYPE_ARRAY;
	  edfmt_stabs_array(type, str, isString, isVector);
	  break;
	case STABS_STR_D_ENUM:
	  type->type = STABS_TYPE_ENUM;
	  type->u.enum_attr = edfmt_stabs_enum(str);
	  break;
	case STABS_STR_D_RANGE:
	  edfmt_stabs_range(type, str);
	  break;
	case STABS_STR_D_STRUCT:
	case STABS_STR_D_UNION:
	  type->type = token == STABS_STR_D_STRUCT ? STABS_TYPE_STRUCT : STABS_TYPE_UNION;

	  edfmt_stabs_ctypenum(type->cnum, STABS_CTYPENUM_SIZE, &type->num);

	  HASH_ADDX(&type_ref, type->cnum, type);
	  addtype = 0;
      
	  edfmt_stabs_struct(&(type->u.struct_union), str);
	  break;
	case STABS_STR_D_BITSTRING:
	  break;
	}
    }

  /* Add it into hash table */
  if (type && addtype)
    {
      edfmt_stabs_ctypenum(type->cnum, STABS_CTYPENUM_SIZE, &type->num);
      HASH_ADDX(&type_ref, type->cnum, type);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

/**
 * Parse the string 
 * A data element represent the whole string
 * @param str pointer on the string to parse
 * @return return a data structure
 */
edfmtstabsdata_t 	*edfmt_stabs_data(char **str)
{
  edfmt_res_size_t 	*res;
  char			token, *save_str, link;
  char			name[STABS_NAME_SIZE];
  edfmtstabsdata_t 	*data;
  edfmtstabstype_t 	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

#if __DEBUG_STABS__
	  printf("[DEBUG_STABS] (0x%x) %s - %s - %s\n", 
		 stabs_c_ent.type,
		 current_file ? current_file->path : "?", 
		 current_file ? current_file->file : "?", 
		 *str);
#endif

  edfmt_stabs_readstr(name, STABS_NAME_SIZE, str, STABS_STR_DELIM);

  if ((*str)[-1] != STABS_STR_DELIM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Stabs type not found", NULL);

  STABS_GETPTR(data, sizeof(edfmtstabsdata_t), NULL);

  strncpy(data->name, name, STABS_NAME_SIZE);
  data->name[STABS_NAME_SIZE - 1] = 0x00;

  if (STABS_IS_SPECIAL(**str))
    {
      data->scope = STABS_SCOPE_LVAR;
      data->type = edfmt_stabs_type(str, NULL);
      data->u.stackpos = stabs_c_ent.value;
    }
  else
    {
      data->scope = STABS_SCOPE_UNK;

      token = *(*str)++;

      switch(token)
	{
	  /* A reference using a register */
	case STABS_STR_T_PARAM_RREG:
	  data->scope = STABS_SCOPE_ARGRREG;
	  data->u.reg = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;

	  /* A constant */
	case STABS_STR_T_CONST:
	  if (**str == '=')
	    (*str)++;

	  /* More information on the type */
	  switch(*(*str)++)
	    {
	    case STABS_STR_T_CONST_FLOAT:
	      data->scope = STABS_SCOPE_CONST_F;
	      data->u.fl = atof(*str);
	      break;
	    case STABS_STR_T_CONST_INT:
	      data->scope = STABS_SCOPE_CONST_I;
	      data->u.i = atoi(*str);
	      break;
	    case STABS_STR_T_CONST_ENUM:
	      data->scope = STABS_SCOPE_CONST_E;
	      data->type = edfmt_stabs_type(str, NULL);

	      if (**str == ',')
		(*str)++;

	      data->u.i = atoi(*str);
	      break;
	    }
	  break;

	  /* Local and global function */
	case STABS_STR_T_FFUNC:
	case STABS_STR_T_GFUNC:
	  data->scope = token == STABS_STR_T_FFUNC ? STABS_SCOPE_LFUNC : STABS_SCOPE_GFUNC;
	  data->u.func.data = data;

	  /* Parse function informations (arguments etc ..) */
	  edfmt_stabs_func(&data->u.func, str);
	  break;

	  /* Global symbol (like a global enum) */
	case STABS_STR_T_GVAR:
	  data->scope = STABS_SCOPE_GVAR;
	  data->type = edfmt_stabs_type(str, NULL);

	  if (data->type == NULL)
	    break;

	  data->u.addr = edfmt_lookup_addr(afile, data->name);
	  break;

	  /* Integer constant */
	case STABS_STR_T_ICONST:
	  data->scope = STABS_SCOPE_CONST_I;
	  data->u.i = atoi(*str);
	  break;

	  /* Function argument */
	case STABS_STR_T_ARGLIST:
	  data->scope = STABS_SCOPE_ARG;
	  data->u.stackpos = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;

	  /* Argument pass into a register */
	case STABS_STR_T_PARAM_REG:
	  data->scope = STABS_SCOPE_ARGREG;
	  data->u.reg = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;
	  
	  /* Variable stored in a register */
	case STABS_STR_T_VAR_REG:
	  data->scope = STABS_SCOPE_VARREG;
	  data->u.reg = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;

	  /* Static variable */
	case STABS_STR_T_VAR_FSCOPE:
	  data->scope = STABS_SCOPE_STATICVAR;
	  data->u.addr = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;

	  /* Local variable */
	case STABS_STR_T_VAR_LOCAL:
	  data->scope = STABS_SCOPE_LVAR;
	  data->type = edfmt_stabs_type(str, NULL);
	  data->u.stackpos = stabs_c_ent.value;
	  break;

	  /* New type */
	case STABS_STR_T_TYPE_STRUCT:
	case STABS_STR_T_TYPE_NAME:
	  data->scope = STABS_SCOPE_TYPE;
	  save_str = *str;

	  data->type = edfmt_stabs_type(str, &link);
	  
	  /* If its a link, we duplicate it and then avoid infinite loop
	     on type list of the file */
	  if (link)
	    {
	      type = data->type;
	      STABS_GETPTR(data->type, sizeof(edfmtstabstype_t), NULL);
	      memcpy(data->type, type, sizeof(edfmtstabstype_t));
	      data->type->next = NULL;
	    }

	  data->type->data = data;

	  /* Resolve size for range types */
	  if (data->type->type == STABS_TYPE_RANGE)
	    {
	      res = hash_get(&resolved_size, data->name);

	      if (res)
		data->type->u.r.size = res->size;
	    }

	  /* Cross reference types */
	  if (data->type->type == STABS_TYPE_STRUCT
	      || data->type->type == STABS_TYPE_UNION
	      || data->type->type == STABS_TYPE_ENUM)
	    {
	      switch(data->type->type)
		{
		case STABS_TYPE_STRUCT:
		  token = STABS_STR_D_STRUCT;
		  break;
		case STABS_TYPE_UNION:
		  token = STABS_STR_D_UNION;
		  break;
		case STABS_TYPE_ENUM:
		  token = STABS_STR_D_ENUM;
		  break;
		}

	      edfmt_stabs_update_cref(data->type, token);
	    }
	  break;

	  /* Argument passed by reference */
	case STABS_STR_T_PARAM_REF:
	  data->scope = STABS_SCOPE_ARGREF;
	  data->u.stackpos = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;

	  /* Procedure static variable */
	case STABS_STR_T_PROC_SVAR:
	  data->scope = STABS_SCOPE_PSTATICVAR;
	  data->u.addr = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str, NULL);
	  break;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, data);
}

/**
 * Main stabs function 
 * Contain the string and more global informations
 * @param file file to parse
 */
int			edfmt_stabs_parse(elfshobj_t *file)
{
  char			*str, *sfile;
  edfmtstabsfile_t 	*tmp;
  unsigned char		parse_str;
  unsigned char		inc;
  edfmtstabsline_t 	*line;
  edfmtstabsdata_t	*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (edfmt_stabs_getinfo(file) != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Retrieve stabs section */
  stabs_sect = edfmt_get_sect(file, ELFSH_SECTION_STAB, ELFSH_SECTION_NAME_STAB,
			      ELFSH_SECTION_STABSTR);

  if (NULL == stabs_sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "STABS section not found", -1);

  /* Retrieve stabs string section */
  stabs_sect_str = file->secthash[ELFSH_SECTION_STABSTR];

  if (NULL == stabs_sect_str)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "STABS string section not found", -1);

  afile = file;

  edfmt_stabs_initrestable();

  XALLOC(__FILE__, __FUNCTION__, __LINE__, stabs_info, sizeof(edfmtstabsinfo_t), -1);

  /* Init hash tables */
  if (type_ref.ent == NULL)
    hash_init(&type_ref, STABS_HNAME_TYPE_REF, 50, ASPECT_TYPE_UNKNOW);

  if (type_cross_ref.ent == NULL)
    hash_init(&type_cross_ref, STABS_HNAME_TYPE_CROSS_REF, 50, ASPECT_TYPE_UNKNOW);
  
  /* Initialise global variables */
  stabs_datastr = (char *) stabs_sect_str->data;
  stabs_ent = (edfmtstabsent_t *) stabs_sect->data;
  stabs_num = stabs_sect->shdr->sh_size / sizeof(edfmtstabsent_t);

  for (stabs_index = 0; stabs_index < stabs_num; stabs_index++)
    {
      inc = 0;
      str = stabs_c_str;
      parse_str = 1;

      switch(stabs_c_ent.type)
	{
	  /* Compiler information */
	case STABS_TYPE_OPT:
	  parse_str = 0;
	  break;
	  
	  /* Manage line numbers */
	case STABS_TYPE_DSLINE:
	case STABS_TYPE_BSLINE:
	case STABS_TYPE_FLINE:
	case STABS_TYPE_SLINE:	  
	  if (!current_file)
	    break;

	  sfile = current_file->file;
	  if (current_file->file && current_file->file[0])
	    {
	      sfile = strrchr(current_file->file, '\\');
	      
	      sfile = sfile && sfile[1] != '\0' ? sfile+1 : current_file->file;
	    }

	  STABS_GETPTR(line, sizeof(edfmtstabsline_t), -1);

	  /* Fill right informations */
	  line->line = (unsigned int) stabs_c_ent.desc;
	  line->addr = stabs_c_ent.value;
	      
	  /* Update pointers */
	  if (current_file->line == NULL)
	    current_file->line = line;
	  else
	    current_file->last_line->next = line;
	  
	  current_file->last_line = line;
	  break;

	case STABS_TYPE_BINCL:
	  if (!current_file)
	    break;

	  inc = 1;

	  /* Include or SO file */
	case STABS_TYPE_SO:
	  parse_str = 0;
	  if (!inc && (!str || !str[0]))
	    {
	      if (!current_file)
		break;

	      /* We have the end addr */
	      current_file->e_addr = stabs_c_ent.value;

	      /* Update pointers */
	      last_file = current_file;
	      current_file = NULL;

#if __DEBUG_STABS__
	      printf("[DEBUG_STABS] ------------------------------------------------\n");
#endif
	      break;
	    }

	  /* We just setup dir name, now it will be filename */
	  if (!inc && current_file != NULL && current_file->file == NULL)
	    {
	      current_file->file = str;
	      edfmt_stabs_addfile(current_file->path, current_file->file);
	      break;
	    }
	  else if (inc)
	    {
	      edfmt_stabs_addfile(NULL, str);
	    }

	  STABS_GETPTR(tmp, sizeof(edfmtstabsfile_t), -1);

	  if (inc)
	    tmp->file = str;
	  else
	    tmp->path = str;

#if __DEBUG_STABS__
	  printf("[DEBUG_STABS] - %s%s -----------------------------------------------\n",
		 inc ? "[i] " : "",
		 str);
#endif

	  tmp->s_addr = stabs_c_ent.value;
	  
	  if (!inc)
	    {
	      /* Update first file if needed */
	      if (stabs_info->file == NULL)
		stabs_info->file = tmp;
	  
	      /* Update current or last file */
	      if (current_file != NULL)
		{
		  last_file = current_file;

		  if (current_file->next == tmp)
		    break;

		  current_file->next = tmp;
		}
	      else if (last_file != NULL)
		{
		  last_file->next = tmp;
		}
	    }
	  else
	    {
	      if (!current_file)
		break;

	      /* Do we already have some include ? */
	      if (current_file->last_inc)
		{
		  if (current_file->last_inc->next == tmp)
		    break;

		  current_file->last_inc->next = tmp;
		  current_file->last_inc = tmp;
		}
	      else
		{
		  current_file->inc = tmp;
		  current_file->last_inc = tmp;
		}

	      tmp->parent = current_file;
	    }

	  current_file = tmp;
	  break;

	  /* End include */
	case STABS_TYPE_EINCL:
	  if (current_file && current_file->parent)
	    current_file = current_file->parent;
	  break;

	  /* Differents symbols */
	case STABS_TYPE_FUN:
	case STABS_TYPE_STSYM:
	case STABS_TYPE_SSYM:
	case STABS_TYPE_PSYM:
	case STABS_TYPE_GSYM:
	case STABS_TYPE_LSYM:
	  break;
	case STABS_TYPE_LBRAC:
	case STABS_TYPE_RBRAC:
	  /* We didn't use brac for the moment */
	  parse_str = 0;
	  break;
	case STABS_TYPE_EXCL:
	  /* Stabs manual: Place holder for a deleted include file
	     TODO: understand it, we already have EINCL for close an include file */
	  parse_str = 0;
	  break;
	default:
	  parse_str = 0;
	  break;
	}

      if (parse_str)
	{
	  data = STABS_DATA(&str);

	  if (data && current_file)
	    {
	      /* TODO: Add more informations  */
	      switch(data->scope)
		{
		case STABS_SCOPE_TYPE:
		  if (current_file->types == NULL)
		    current_file->types = data->type;
		  else 
		    {
		      if (current_file->ltype == data->type)
			break;
		      
		      current_file->ltype->next = data->type;
		    }
		  current_file->ltype = data->type;
		  break;
		case STABS_SCOPE_GVAR:
		  if (current_file->vars == NULL)
		    current_file->vars = data;
		  else 
		    {
		      if (current_file->lvar == data)
			break;

		      current_file->lvar->next = data;
		    }
		  current_file->lvar = data;		  
		  break;
		}
	    }
	}
      else
	{
#if __DEBUG_STABS__
	  printf("[DEBUG_STABS]!(0x%x) %s - %s - %s\n", 
		 stabs_c_ent.type,
		 current_file ? current_file->path : "?", 
		 current_file ? current_file->file : "?", 
		 str);
#endif
	}
    }

  /* Clean hash tables */
  hash_empty(STABS_HNAME_TYPE_REF);
  hash_empty(STABS_HNAME_TYPE_CROSS_REF);

  file->debug_format.stabs = (void *) stabs_info;
  stabs_info = NULL;
  root_file = current_file = last_file = NULL;
  current_func = NULL;
  afile = NULL;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Clean stabs allocated structures 
 * @param file to clean
 */
int			edfmt_stabs_clean(elfshobj_t *file)
{
  edfmtstabsinfo_t 	*info;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 
  info = edfmt_stabs_getinfo(file);

  if (!info)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (info->dir_list)
    XFREE(__FILE__, __FUNCTION__, __LINE__, info->dir_list);

  if (info->file_list)
    XFREE(__FILE__, __FUNCTION__, __LINE__, info->file_list);

  if (info->alloc_pool)
    edfmt_clean_pool(&(info->alloc_pool));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
