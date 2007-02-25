/*
** api.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Jan 26 2007 11:54:22 mxatone
**
*/

#include "libedfmt.h"

/* Unique debug format base variable */
edfmtinfo_t *uniinfo = NULL;
edfmttype_t *uni_lasttype = NULL;
edfmtvar_t *uni_lastvar = NULL;

u_char file_active = 0;
elfshobj_t *cu_obj = NULL;

#define EDFMT_NEW_TYPE(_type) \
do { XALLOC(__FILE__, __FUNCTION__, __LINE__,_type, sizeof(edfmttype_t), NULL); } while(0)

#define EDFMT_NEW_VAR(_var) \
do { XALLOC(__FILE__, __FUNCTION__, __LINE__,_var, sizeof(edfmtvar_t), NULL); } while(0)

#define EDFMT_COPY_NAME(_dest, _source) 			\
do { 								\
  if (strlen(_source) >= EDFMT_NAME_SIZE)			\
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 	\
		       "Invalid name size", NULL);		\
  strcpy(_dest->name, _source);					\
} while (0)

static char *edfmt_uni_typename(u_int type)
{
  switch(type)
    {
    case EDFMT_TYPE_UNK:
      return "?";
      break;
    case EDFMT_TYPE_BASIC:
      return "basic";
      break;
    case EDFMT_TYPE_ARRAY:
      return "array";
      break;
    case EDFMT_TYPE_PTR:
      return "ptr";
      break;
    case EDFMT_TYPE_STRUCT:
      return "struct";
      break;
    case EDFMT_TYPE_UNION:
      return "union";
      break;
    case EDFMT_TYPE_ATTR:
      return "attr";
      break;
    case EDFMT_TYPE_VOID:
      return "void";
      break;
    case EDFMT_TYPE_LINK:
      return "link";
      break;
    }

  return "?????";
}

static int		edfmt_uni_print_var(edfmtvar_t *vars)
{
  edfmtvar_t		*var;

  for (var = vars; var != NULL; var = var->next)
    {
      printf("[%02d] %s: %08x => %s\n",
	     var->scope, var->name, var->addr, var->type->name);
    }

  return 0;
}

static int		edfmt_uni_print_type(edfmttype_t *types, u_int tab)
{
  edfmttype_t		*type;
  u_int			index;

  for (type = types; type != NULL; type = type->next)
    {
      for (index = 0; index < tab; index++)
	printf("\t");

      printf("%s - %s %d:%d ", edfmt_uni_typename(type->type), 
	     type->name, type->start, type->size);

      if (type->type != EDFMT_TYPE_ATTR && type->parent)
	printf("parent = 0x%p:%s ", type->parent, type->parent->name);

      if (type->type == EDFMT_TYPE_ATTR && type->child)
	printf("child = 0%p:%s ", type->child, type->child->name);

      printf("\n");
      if (type->type != EDFMT_TYPE_ATTR && type->child)
	{
	  printf(" => Childs:\n");
	  edfmt_uni_print_type(type->child, tab+1);
	}
    }

  return 0;
}

static int		edfmt_unit_print_file(edfmtfile_t *files)
{
  edfmtfile_t		*file;

  for (file = files; file != NULL; file = file->next)
    {
      printf(" ==== %s ===== %s\n", file->name,
	     file->parent ? file->parent->name : "-");

      printf(" .: Types :. \n");
      edfmt_uni_print_type(file->types, 0);
      printf("\n .: Vars :. \n");
      edfmt_uni_print_var(file->vars);

      if (file->child)
	edfmt_unit_print_file(file->child);

      printf(" ============\n");
    }

  return 0;
}

int			edfmt_uni_print(elfshobj_t *file)
{
  edfmtinfo_t		*info;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  info = edfmt_get_uniinfo(file);

  if (!info)
    {
      printf("Uniinfo = NULL\n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"No informations", -1);
    }

  printf(" .: Global Types :. \n");
  edfmt_uni_print_type(info->types, 0);
  printf("\n .: Global Vars :. \n");
  edfmt_uni_print_var(info->vars);

  edfmt_unit_print_file(info->files);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Init an add context */
int			edfmt_add_init(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  cu_obj = file;

  if (!file)							
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid file object", NULL);

  /* Init the object if needed */
  if (file->debug_format.uni == NULL) 
    {				
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     file->debug_format.uni, sizeof(edfmtinfo_t), NULL);	
      uniinfo = file->debug_format.uni;

      hash_init(&(uniinfo->hvar), NULL, 30, ASPECT_TYPE_UNKNOW);
      hash_init(&(uniinfo->htype), NULL, 30, ASPECT_TYPE_UNKNOW);
    } 
  else 
    {							
      uniinfo = file->debug_format.uni; 
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* End an add context */
int			edfmt_add_end()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  while (uniinfo && uniinfo->lfile && uniinfo->lfile->parent != NULL)
    uniinfo->lfile = uniinfo->lfile->parent;

  /* Reset global pointers */
  cu_obj = NULL;
  uniinfo = NULL;
  file_active = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Update type list */
int			edfmt_update_type(edfmttype_t *type)
{
  edfmttype_t		*seek_type;
  hash_t		*htable;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  type->file = uniinfo->lfile;

  /* Setup hash table depending of current scope */
  htable = uniinfo->lfile == NULL ? &(uniinfo->htype) : &(uniinfo->lfile->htype);

  /* Research on the right table */
  seek_type = (edfmttype_t *) hash_get(htable, type->name);
  
  /* We already have this type */
  if (seek_type)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Global scope */
  if (uniinfo->lfile == NULL)
    {
      if (uniinfo->types == NULL)
	uniinfo->types = type;
      else
	uniinfo->ltype->next = type;
      
      uniinfo->ltype = type;
    }
  else /* Local scope */
    {
      if (uniinfo->lfile->types == NULL)
	uniinfo->lfile->types = type;
      else
	uniinfo->lfile->ltype->next = type;
      
      uniinfo->lfile->ltype = type;
    }

  hash_add(htable, aproxy_strdup(type->name), (void *) type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Update var list */
int			edfmt_update_var(edfmtvar_t *var)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Global pointer uninitialise", NULL);

  /* Global scope */
  if (uniinfo->lfile == NULL || file_active == 0)
    {
      if (uniinfo->vars == NULL)
	uniinfo->vars = var;
      else
	uniinfo->lvar->next = var;

      uniinfo->lvar = var;
    }
  else
    {
      /* Local scope */
      if (uniinfo->lfile->vars == NULL)
	uniinfo->lfile->vars = var;
      else
	uniinfo->lfile->lvar->next = var;

      uniinfo->lfile->lvar = var;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Retrieve uniinfo pointer */
edfmtinfo_t		*edfmt_get_uniinfo(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramter", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (edfmtinfo_t *) file->debug_format.uni);
}

int			edfmt_change_type_nfile(edfmttype_t *type)
{
  edfmttype_t		*tmp_type = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramter", -1);
  
  /* Delete for its file */
  if (type->file == NULL)
    {
      if (uniinfo->types == type)
	uniinfo->types = type->next;
      else
	tmp_type = uniinfo->types;
    }
  else
    {
      if (type->file->types == type)
	type->file->types = type->next;
      else
	tmp_type = type->file->types;
    }

  if (tmp_type != NULL)
    {
      /* Find our type in the file */
      while (tmp_type && tmp_type->next != type)
	tmp_type = tmp_type->next;
      
      /* We update the precedent file */
      if (tmp_type && tmp_type->next)
	tmp_type->next = type->next;
    }

  /* Add to the current file */
  edfmt_update_type(type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Restore parent file */
edfmtfile_t		*edfmt_restore_parent_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (uniinfo->lfile == NULL || uniinfo->lfile->parent == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No parents", NULL);

  uniinfo->lfile = uniinfo->lfile->parent;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, uniinfo->lfile);
}

edfmtfile_t		*edfmt_get_current_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!uniinfo)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No uniform informations", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, uniinfo->lfile);
}

/* Search for a specific included file */
edfmtfile_t		*edfmt_get_inc_file(edfmtfile_t *file, char *name)
{
  edfmtfile_t 		*inc;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (inc = file->child; inc != NULL; inc = inc->next)
    {
      if (!strcmp(name, inc->name))
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, inc);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Add a new file */
edfmtfile_t		*edfmt_add_file(edfmtfile_t *parent_file, char *name, 
					elfsh_Addr start, elfsh_Addr end)
{
  edfmtfile_t 		*file, *already_file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramter", NULL);

  if (parent_file != NULL)
    {
      already_file = edfmt_get_inc_file(parent_file, name);

      if (already_file != NULL)
	{
	  uniinfo->lfile = already_file;
	  file_active = 1;	  

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, already_file);
	}
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__,file, sizeof(edfmtfile_t), NULL);

  EDFMT_COPY_NAME(file, name);
  file->start = start;
  file->end = end;

  hash_init(&(file->hvar), NULL, 30, ASPECT_TYPE_UNKNOW);
  hash_init(&(file->htype), NULL, 30, ASPECT_TYPE_UNKNOW);

  /* Update pointers depending if he get a parrent */
  if (parent_file != NULL)
    {
      if (parent_file->child == NULL)
	parent_file->child = file;
      else
	parent_file->lchild->next = file;

      parent_file->lchild = file;
      file->parent = parent_file;
    }
  else
    {
      if (uniinfo->files == NULL)
	uniinfo->files = file;
      else
	while (uniinfo->files->parent != NULL)
	  uniinfo->files = uniinfo->files->parent;
      
      if (uniinfo->lfile != NULL)
	uniinfo->lfile->next = file;
    }

  uniinfo->lfile = file;
  file_active = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file);
}

/* Reset file activity */
int			edfmt_reset_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  file_active = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Reactive file activity */
int			edfmt_reactive_file()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  file_active = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create an unknown type */
edfmttype_t		*edfmt_add_type_unk(char *name)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_UNK;
  EDFMT_COPY_NAME(ltype, name);

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a basic type */
edfmttype_t		*edfmt_add_type_basic(char *name, int size)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_BASIC;
  EDFMT_COPY_NAME(ltype, name);
  ltype->size = size;

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create an array type */
edfmttype_t		*edfmt_add_type_array(char *name, 
					      int size, edfmttype_t *type)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_ARRAY;
  EDFMT_COPY_NAME(ltype, name);
  ltype->parent = type;
  ltype->size = size;

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a ptr type */
edfmttype_t		*edfmt_add_type_ptr(char *name, edfmttype_t *type)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_PTR;
  EDFMT_COPY_NAME(ltype, name);
  ltype->parent = type;

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a structure type */
edfmttype_t		*edfmt_add_type_struct(char *name, int size)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_STRUCT;
  EDFMT_COPY_NAME(ltype, name);
  ltype->size = size;

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create an union type */
edfmttype_t		*edfmt_add_type_union(char *name, int size)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_UNION;
  EDFMT_COPY_NAME(ltype, name);
  ltype->size = size;

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

 /* Create an attribute type */
edfmttype_t		*edfmt_add_type_attr(edfmttype_t *tstruct, char *name, 
					     int start, int size, edfmttype_t *type)
{
  edfmttype_t		*ltype, *last;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!tstruct || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_ATTR;
  EDFMT_COPY_NAME(ltype, name);
  ltype->parent = tstruct;
  ltype->child = type;
  ltype->start = start;
  ltype->size = size ? size : tstruct->size - start;

  if (type && type->size > 0)
    ltype->size = type->size;

  if (tstruct->child == NULL)
    tstruct->child = ltype;
  else
    {
      /* Iterate to find the end */
      for (last = tstruct->child; last && last->next; last = last->next);

      if (last)
	{
	  if (tstruct->type != EDFMT_TYPE_UNION && last->size == tstruct->size - last->start)
	    last->size = start - last->start;

	  last->next = ltype;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a void type */
edfmttype_t		*edfmt_add_type_void(char *name)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_VOID;
  EDFMT_COPY_NAME(ltype, name);

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

/* Create a link type */
edfmttype_t		*edfmt_add_type_link(char *name, edfmttype_t *type)
{
  edfmttype_t		*ltype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_TYPE(ltype);
  ltype->type = EDFMT_TYPE_LINK;
  EDFMT_COPY_NAME(ltype, name);
  ltype->parent = type;

  /* Update type list */
  edfmt_update_type(ltype);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ltype);
}

edfmtvar_t		*edfmt_add_var_global(edfmttype_t *type, char *name, elfsh_Addr addr)
{
  edfmtvar_t		*lvar;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!type || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", NULL);

  EDFMT_NEW_VAR(lvar);
  lvar->scope = EDFMT_SCOPE_GLOBAL;
  lvar->addr = addr;
  EDFMT_COPY_NAME(lvar, name);
  lvar->type = type;

  edfmt_update_var(lvar);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, lvar);
}
