/*
** stabs.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Jan 01 2007 21:30:13 mxatone
**
*/

#include "libedfmt.h"

/* This file implements stab support for libedfmt.
specificitation @ http://wwwcdf.pd.infn.it/localdoc/stabs.pdf 
-
This version of stabs format parsing has been approved for x86
and gentoo linux, some bugs can append on Sparc, more test will
be done to make this work correctly */

#define STABS_HNAME_TYPE_REF "stabs_type_reference"

/* Global position variables */
elfshsect_t *stabs_sect;
elfshsect_t *stabs_sect_str;

edfmtstabsent_t *stabs_ent;

/* Index */
u_int stabs_index;
u_int stabs_num;
char *stabs_datastr;

/* File structure pointers */
edfmtstabsfile_t *root_file = NULL;
edfmtstabsfile_t *current_file = NULL;
edfmtstabsfile_t *last_file = NULL;

/* Current function we are in */
edfmtstabsfunc_t *current_func = NULL;

/* Temporary hash table used for type resolution */
hash_t type_ref;

/* Contain main informations */
edfmtstabsinfo_t *stabs_info = NULL;

/* Actual parsed file */
elfshobj_t *afile = NULL;

edfmtstabsinfo_t 	*edfmt_stabs_getinfo(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (edfmtstabsinfo_t *) file->debug_format.stabs);
}

int	    		edfmt_stabs_addfile(char *dir, char *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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

  /* Update list */
  stabs_info->dir_list[stabs_info->index_list] 	= dir;
  stabs_info->file_list[stabs_info->index_list] = file;

  stabs_info->index_list++;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse a function */
int     		edfmt_stabs_func(edfmtstabsfunc_t *func, char **str)
{
  char			*my_str;
  u_int			arg_index = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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

  current_func = func;

  /* Fill start infos */
  func->s_addr = stabs_c_ent.value;
  func->s_line = stabs_c_ent.desc;

  func->rettype = edfmt_stabs_type(str);

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
      
      /* Ok we have a max arguments, but if you got more than 20 arguments, 
	 blame yourself !! */
      if (arg_index < STABS_MAX_ARGUMENTS)
	{
	  my_str = stabs_c_str;
	  func->args[arg_index++] = edfmt_stabs_data(&my_str);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse a type range */
int	 	     	edfmt_stabs_range(edfmtstabstype_t *type, char **str)
{
  edfmtstabstype_t 	*rtype = NULL;
  long			low, high;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == type || STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

  rtype = edfmt_stabs_type(str);

  if (**str == ';')
    (*str)++;

  low = high = 0;

  edfmt_stabs_readnumber(str, ';', &low);

  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1);

  (*str)++;

  edfmt_stabs_readnumber(str, ';', &high);

  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1);

  *(str)++;

  /* Fill range felds, but we will use only link (mainly) */
  type->type = STABS_TYPE_RANGE;
  type->u.link = rtype;
  type->u.r.low = low;
  type->u.r.high = high;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int	      		edfmt_stabs_array(edfmtstabstype_t *type, char **str, 
				  u_char isString, u_char isVector)
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
  edfmt_stabs_type(str);

  if (**str == ';')
    (*str)++;

  edfmt_stabs_readnumber(str, ';', &low);
  
  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1);
  
  (*str)++;
  
  edfmt_stabs_readnumber(str, ';', &high);
  
  if (**str != ';')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid str informations", -1); 

  (*str)++;

  /* Describe array type */
  rtype = edfmt_stabs_type(str);

  type->u.arr.low = low;
  type->u.arr.high = high;
  type->u.arr.link = rtype;

  if (isString)
    type->u.arr.type = STABS_ARRAY_STRING;

  if (isVector)
    type->u.arr.type |= STABS_ARRAY_VECTOR;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  
}

int	      		edfmt_stabs_struct(edfmtstabsstruct_t *tstruct, char **str)
{
  edfmtstabsattr_t 	*tattr = NULL, *lattr = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == tstruct || STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  edfmt_stabs_readnumber(str, 0, &tstruct->size);

  do {
    XALLOC(__FILE__, __FUNCTION__, __LINE__,tattr, sizeof(edfmtstabsattr_t), -1);

    if (tstruct->attr == NULL)
      tstruct->attr = tattr;
    else
      lattr->next = tattr;

    lattr = tattr;
    
    edfmt_stabs_readstr(tattr->name, STABS_NAME_SIZE, str, ':');

    if ((*str)[-1] != ':')
      break;

    tattr->type = edfmt_stabs_type(str);
    
    if (**str != ',')
      break;

    (*str)++;

    edfmt_stabs_readnumber(str, ',', &tattr->start);

    if (**str != ',')
      break;

    (*str)++;

    edfmt_stabs_readnumber(str, ';', &tattr->size);

    if (**str != ';')
      break;

    (*str)++;
  } while (**str != ';');

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

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
      XALLOC(__FILE__, __FUNCTION__, __LINE__,enum_attr, sizeof(edfmtstabsenum_t), NULL);

      if (root_attr == NULL)
	root_attr = enum_attr;

      if (last_attr)
	last_attr->next = enum_attr;

      last_attr = enum_attr;

      edfmt_stabs_readstr(enum_attr->name, STABS_NAME_SIZE, str, ':');

      if ((*str)[-1] != ':')
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Wrong string pattern", NULL);

      if(edfmt_stabs_readnumber(str, ',', &enum_attr->value) != 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Wrong string pattern", NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, root_attr);
}

edfmtstabstype_t 	*edfmt_stabs_type(char **str)
{
  char			*save_str;
  char			token;
  edfmtstabstypenum_t 	tnum;
  edfmtstabstype_t	 *type;
  char			ctypenum[STABS_CTYPENUM_SIZE];
  long			tsize = 0;
  u_char       		isString = 0;
  u_char		isVector = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

  tnum.file = tnum.number = 0;

  /* Retrieve the type number */
  if (STABS_IS_SPECIAL(**str))
    {
      edfmt_stabs_typenum(&tnum, str);

      if (**str != '=')
	{
	  edfmt_stabs_ctypenum(ctypenum, STABS_CTYPENUM_SIZE, &tnum);
	  type = hash_get(&type_ref, ctypenum);

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
	}

      (*str)++;
    }
  else
    {
      /* Anonymous type */
      tnum.file = tnum.number = -1;
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__,type, sizeof(edfmtstabstype_t), NULL);
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
	  type->u.link = edfmt_stabs_type(str);
	}
    }
  else
    {
      type->type = STABS_TYPE_UNK;
      switch(token)
	{
	case STABS_STR_D_METHOD:
	  /* We don't support C++ for the moment 
	     as clear representation isn't set for 
	     the moment, we will see */
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
	  type->u.link = edfmt_stabs_type(str);
	  break;
	case STABS_STR_D_ATTR:
	  /* GNU C++ way - Class member*/
	  if (STABS_IS_SPECIAL(**str))
	    {
	      /* We don't support C++ for the moment 
		 as clear representation isn't set for 
		 the moment, we will see */
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
	  type->u.r.size = tsize;
	  break;
	case STABS_STR_D_STRUCT:
	case STABS_STR_D_UNION:
	  type->type = token == STABS_STR_D_STRUCT ? STABS_TYPE_STRUCT : STABS_TYPE_UNION;
	  edfmt_stabs_struct(&type->u.struct_union, str);
	  break;
	case STABS_STR_D_BITSTRING:
	  break;
	}
    }

  /* Add it into hash table */
  edfmt_stabs_ctypenum(type->cnum, STABS_CTYPENUM_SIZE, &type->num);
  HASH_ADDX(&type_ref, type->cnum, type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

/* Parse the string */
edfmtstabsdata_t 	*edfmt_stabs_data(char **str)
{
  char			token;
  char			name[STABS_NAME_SIZE];
  edfmtstabsdata_t 	*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", NULL);

  edfmt_stabs_readstr(name, STABS_NAME_SIZE, str, STABS_STR_DELIM);

  if ((*str)[-1] != STABS_STR_DELIM)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Stabs type not found", NULL);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,data, sizeof(edfmtstabsdata_t), NULL);

  strncpy(data->name, name, STABS_NAME_SIZE);
  data->name[STABS_NAME_SIZE - 1] = 0x00;

  if (STABS_IS_SPECIAL(**str))
    {
      data->scope = STABS_SCOPE_LVAR;
      data->type = edfmt_stabs_type(str);
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
	  data->type = edfmt_stabs_type(str);
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
	      data->type = edfmt_stabs_type(str);

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
	  data->type = edfmt_stabs_type(str);

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
	  data->type = edfmt_stabs_type(str);
	  break;

	  /* Argument pass into a register */
	case STABS_STR_T_PARAM_REG:
	  data->scope = STABS_SCOPE_ARGREG;
	  data->u.reg = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str);
	  break;
	  
	  /* Variable stored in a register */
	case STABS_STR_T_VAR_REG:
	  data->scope = STABS_SCOPE_VARREG;
	  data->u.reg = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str);
	  break;

	  /* Static variable */
	case STABS_STR_T_VAR_FSCOPE:
	  data->scope = STABS_SCOPE_STATICVAR;
	  data->u.addr = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str);
	  break;

	  /* Local variable */
	case STABS_STR_T_VAR_LOCAL:
	  data->scope = STABS_SCOPE_LVAR;
	  data->type = edfmt_stabs_type(str);
	  data->u.stackpos = stabs_c_ent.value;
	  break;

	  /* New type */
	case STABS_STR_T_TYPE_STRUCT:
	case STABS_STR_T_TYPE_NAME:
	  data->scope = STABS_SCOPE_TYPE;
	  data->type = edfmt_stabs_type(str);
	  data->type->data = data;
	  break;

	  /* Argument passed by reference */
	case STABS_STR_T_PARAM_REF:
	  data->scope = STABS_SCOPE_ARGREF;
	  data->u.stackpos = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str);
	  break;

	  /* Procedure static variable */
	case STABS_STR_T_PROC_SVAR:
	  data->scope = STABS_SCOPE_PSTATICVAR;
	  data->u.addr = stabs_c_ent.value;
	  data->type = edfmt_stabs_type(str);
	  break;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, data);
}

/* Main stabs function */
int			edfmt_stabs_parse(elfshobj_t *file)
{
  char			*str, *sfile;
  edfmtstabsfile_t 	*tmp;
  u_char		parse_str;
  u_char		inc;
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,stabs_info, sizeof(edfmtstabsinfo_t), -1);

  if (type_ref.ent == NULL)
    hash_init(&type_ref, STABS_HNAME_TYPE_REF, 50, ASPECT_TYPE_UNKNOW);
  
  /* Initialise global variables */
  stabs_datastr = (char *) elfsh_get_raw(stabs_sect_str);
  stabs_ent = (edfmtstabsent_t *) elfsh_get_raw(stabs_sect);
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

	  XALLOC(__FILE__, __FUNCTION__, __LINE__,line, sizeof(edfmtstabsline_t), -1);

	  /* Fill right informations */
	  line->line = (u_int) stabs_c_ent.desc;
	  line->addr = stabs_c_ent.value;
	      
	  /* Update pointers */
	  if (current_file->line == NULL)
	    current_file->line = line;
	  else
	    current_file->last_line->next = line;
	  
	  current_file->last_line = line;
	  break;
	case STABS_TYPE_BINCL:
	  inc = 1;
	case STABS_TYPE_SO:
	  /* Include or SO file */
	  parse_str = 0;
	  if (!inc && (!str || !str[0]))
	    {
	      /* We have the end addr */
	      current_file->e_addr = stabs_c_ent.value;

	      /* Update pointers */
	      last_file = current_file;
	      current_file = NULL;

#if __DEBUG_STABS_
	      printf("------------------------------------------------\n");
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

	  XALLOC(__FILE__, __FUNCTION__, __LINE__,tmp, sizeof(edfmtstabsfile_t), -1);
	  
	  if (inc)
	    tmp->file = str;
	  else
	    tmp->path = str;

#if __DEBUG_STABS_
	  printf("- %s%s -----------------------------------------------\n",
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
		  current_file->next = tmp;
		}
	      else if (last_file != NULL)
		{
		  last_file->next = tmp;
		}
	    }
	  else
	    {
	      /* Do we already have some include ? */
	      if (current_file->last_inc)
		{
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
#if __DEBUG_STABS_
	  printf("%s - %s - %s\n", current_file->path, current_file->file, str);
#endif

	  data = edfmt_stabs_data(&str);

	  if (data)
	    {
	      /* TODO: Add more informations  */
	      switch(data->scope)
		{
		case STABS_SCOPE_TYPE:
		  if (current_file->types == NULL)
		    current_file->types = data->type;
		  else 
		    current_file->ltype->next = data->type;
		  current_file->ltype = data->type;
		  break;
		case STABS_SCOPE_GVAR:
		  if (current_file->vars == NULL)
		    current_file->vars = data;
		  else 
		    current_file->lvar->next = data;
		  current_file->lvar = data;		  
		  break;
		}
	    }
	}
    }

  hash_empty(STABS_HNAME_TYPE_REF);

  file->debug_format.stabs = (void *) stabs_info;
  stabs_info = NULL;
  root_file = current_file = last_file = NULL;
  current_func = NULL;
  afile = NULL;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
