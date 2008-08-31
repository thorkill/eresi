/**
 * @file load.c
 * @ingroup libstderesi 
 * Started on  Sat Jan 25 11:21:18 2003 jfv
 *
 * $Id: load.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"

/** 
 * return 1 if a file named 'name' is in the workspace 
 * @param name
 */
int		revm_is_loaded(char *name)
{
  listent_t	*actual;
  elfshobj_t	*obj;
  int		index;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!name)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (!world.curjob)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (!world.curjob->curfile)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  for (index = 0; index < world.curjob->loaded.size; index++)
    for (actual = &world.curjob->loaded.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	obj = actual->data;
	if (!strcmp(name, obj->name))
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
      }
  
  for (index = 0; index < world.shared_hash.size; index++)
    for (actual = &world.shared_hash.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	obj = actual->data;
	if (!strcmp(name, obj->name))
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
      }
  
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment.
 * @param file
 * @param name
 */
int		revm_load_init_dephash(elfshobj_t *file, char *name)
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  
  /* Init hash dep */
  snprintf(logbuf, sizeof(logbuf), "%s_children", name);
  hash_init(&file->child_hash , strdup(logbuf), 20, ASPECT_TYPE_UNKNOW);
 
  snprintf(logbuf, sizeof(logbuf), "%s_roots", name);
  hash_init(&file->root_hash  , strdup(logbuf), 20, ASPECT_TYPE_UNKNOW);
 
  snprintf(logbuf, sizeof(logbuf), "%s_parents", name);
  hash_init(&file->parent_hash, strdup(logbuf), 20, ASPECT_TYPE_UNKNOW);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment.
 * @param regex
 */
int		revm_file_loads(char *regex)
{
  // split for dir/files part
  // open dir
  // read dir
  // match against regex
  // if matched load files
  return (0);
}


/** 
 * @brief Load a file in e2dbg.
 * @param name  File path.
 * @param base Object memory base address.
 * @param lm Linkmap entry for that object.
 * @return Success (0) or Error (-1).
 */
int		revm_file_load(char *name, eresi_Addr base, elfshlinkmap_t *lm)
{
  elfshobj_t	*new;
  revmexpr_t	*expr;
  revmobj_t	*tmp;
  char		logbuf[BUFSIZ];
  char		*timec;
  hash_t	*filehash;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid argument", -1);

  /* Reset the id counter in case we have unloaded all our files */
  filehash = hash_find("files");
  if (!hash_size(filehash))
    world.state.lastid = 0;
  
  /* Map the standard ELF object */
  new = elfsh_map_obj(name);
  if (NULL == new)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot load object", -1);
  
  /* Print a msg if not in quiet mode */
  new->loadtime = time(&new->loadtime);
  if (!world.state.revm_quiet)
    {
      timec = ctime(&new->loadtime);
      timec[strlen(timec) - 1] = '\0';
      snprintf(logbuf, BUFSIZ - 1, " [*] %s - New object loaded : %s\n",
	       timec , name);
      revm_output(logbuf);
    }
  
  /* Set base address */
  new->rhdr.base = base;
  
  /* Set linkmap address */
  if (new->hdr->e_type != ET_CORE)
    new->linkmap = lm;
  else 
    new->linkmap = NULL;
  
  /* Add the object to the list of opened objects */
  new->id = ++world.state.lastid;
  world.curjob->curfile = new;
  expr = revm_expr_get(REVM_VAR_LOAD);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "TROUBLE : Last loaded file inexistant", -1);
  tmp = expr->value;
  tmp->immed_val.word = new->id;
  
  /* Support shared objects */
  if (world.state.revm_shared)
    hash_add(&world.shared_hash, new->name, new);
  else
    hash_add(&world.curjob->loaded, new->name, new);
  
  /* Add an entry into the loaded files hashtable */
  hash_add(&file_hash, new->name, (void *) new);

  /* Init hash dep */
  revm_load_init_dephash(new, name);
  
  if (new->hdr->e_type == ET_CORE) 
    {
      hash_add(&new->root_hash, new->name, new);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Create libmjollnir context for this binary */
  mjr_create_context_as_current(&world.mjr_session, new);
  mjr_setup_processor(&world.mjr_session);

  /* Init search hash tables */
  elfsh_init_symbol_hashtables(new);

  /* Parse debugging informations */
  revm_edfmt_parse(new);

  /* Load dependances */
  if (new->hdr->e_type == ET_EXEC)
    hash_add(&new->root_hash, new->name, new);
  
  /* We use a different dependences technique for mapped files 
     in the debugger. Just load dependences here for unmapped files */
  if (!elfsh_is_debug_mode())
    revm_load_enumdep(new);
  
  /* Load EDFMT debug sections */
  if ((int) config_get_data(ERESI_VMCONFIG_ONLOAD_RCONTROL)) 
    {
      if (mjr_functions_get(world.mjr_session.cur) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Analyzed functions edfmt section cannot be restored", -1);
      if (mjr_blocks_get(world.mjr_session.cur) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Analyzed blocks edfmt section cannot be restored", -1);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 *  Insert an object in the list of opened elfsh descriptors 
 */
int		cmd_load()
{
  revmexpr_t	*expr;
  revmobj_t	*tmp;
  char		*str;
  int		was_dynamic;
  int		ret; 
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Make sure we are or not in the debugger */
  was_dynamic = 0;
  if (elfsh_is_debug_mode())
    {
      was_dynamic = 1;
      elfsh_set_static_mode();
    }
  
  /* Find which file we need to load */
  expr = revm_lookup_param(world.curjob->curcmd->param[0]);
  if (!expr || !expr->value)
    {
      if (was_dynamic)
	elfsh_set_debug_mode();
      world.state.revm_shared = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unknown file to load", (-1));
    }
  tmp = expr->value;
  if (!tmp->otype)
    {
      revm_expr_destroy(expr->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Missing parameter", (-1));
    }

  if (tmp->otype->type != ASPECT_TYPE_STR)
    {
      revm_convert_object(expr, ASPECT_TYPE_STR);
      if (tmp->otype->type != ASPECT_TYPE_STR)
	{
	  revm_expr_destroy(expr->label);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid file to load", (-1));
	}
    }
  str = (tmp->immed ? tmp->immed_val.str : tmp->get_name(tmp->root, tmp->parent));
  if (hash_get(&world.curjob->loaded, str) || hash_get(&world.shared_hash, str))
    {
      revm_expr_destroy(expr->label);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Cannot load file many times in the same workspace", (-1));
    }
  
  /* Really load the file */
  revm_output("\n");
  ret = revm_file_load(str, 0, NULL);
  revm_output("\n");
  
  /* Restore dynamic mode */
  if (was_dynamic)
    {
      if (world.curjob->curfile && world.curjob->curfile->linkmap)
	elfsh_set_debug_mode();
      else 
	revm_output("\n [!] Loaded file not present in linkmap"
		  ", switching to STATIC mode\n\n");
    }
  else 
    elfsh_set_static_mode();

  /* Everything was OK */
  world.state.revm_shared = 0;
  revm_expr_destroy(expr->label);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


