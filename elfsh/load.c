/*
** load.c for elfsh
** 
** Started on  Sat Jan 25 11:21:18 2003 mayhem
*/
#include "elfsh.h"


/* return 1 if a file named 'name' is in the workspace */
int		vm_is_loaded(char *name)
{
  elfshobj_t	*obj;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  if (!world.curjob)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (!world.curjob->current)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  obj = world.curjob->list;
  while (obj)
    {
      if (!strcmp(name, obj->name))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
      obj = obj->next;
    }

  obj = world.shared;
  while (obj)
    {
      if (!strcmp(name, obj->name))
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
      
      obj = obj->next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Load a file in elfsh */
int		vm_load_file(char *name, elfsh_Addr base, elfshlinkmap_t *lm)
{
  elfshobj_t	*new;
  elfshpath_t	*tmp;
  char		logbuf[BUFSIZ];
  char		*timec;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!name)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);

  /* Map the standard ELF object */
  new = elfsh_map_obj(name);
  if (NULL == new)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot load object", -1);

  /* Print a msg if not in quiet mode */
  new->loadtime = time(&new->loadtime);
  if (!world.state.vm_quiet)
    {
      timec = ctime(&new->loadtime);
      timec[strlen(timec) - 1] = '\0';
      snprintf(logbuf, BUFSIZ - 1, " [*] %s - New object loaded : %s\n",
	      timec , name);
      vm_output(logbuf);
    }

  /* Set base address */
  new->rhdr.base = base;

  /* Set linkmap address */
  new->linkmap = lm;

  /* Add the object to the list of opened objects */
  new->id = ++world.state.lastid;

  world.curjob->current = new;

  tmp = hash_get(&vars_hash, "!");
  if (!tmp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "TROUBLE : Last loaded file inexistant", -1);
  tmp->immed_val.word = new->id;

  /* Support shared objects */
  if (world.state.vm_shared)
    {
      new->next = world.shared;
      world.shared = new;
      world.state.vm_shared = 0;
    }
  else
    {
      new->next = world.curjob->list;
      world.curjob->list = new;
    }
    
#if defined(USE_MJOLLNIR)
  XALLOC(world.curjob->mjr_session,sizeof(mjrSession),-1);
  if (!mjr_init_session(world.curjob->mjr_session))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
    		      "mjollnir session can't be initialized.", -1);
      world.curjob->mjr_session->obj = new;

   mjr_setup_processor(world.curjob->mjr_session);
#endif

  /* Add an entry into the loaded files hashtable */
  hash_add(&file_hash, new->name, (void *) new);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Insert an object in the list of opened elfsh descriptors */
int		cmd_load()
{
  elfshpath_t	*tmp;
  char		*str;
  int		was_dynamic = 0;
  int		ret; 

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_is_debug_mode())
    {
      was_dynamic = 1;
      elfsh_set_static_mode();
    }

  /* We cannot use vm_lookup_file() here because the file is not yet loaded */
  str = world.curjob->curcmd->param[0];
  if (*str == ELFSH_VARPREF)
    {
      str = vm_lookup_var(++str);
      if (!str)
	tmp = hash_get(&vars_hash, str);
      if (!tmp || tmp->type != ELFSH_OBJSTR)
	goto err;
      str = tmp->immed_val.str;
    }

  vm_output("\n");
  ret = vm_load_file(str, NULL, NULL);
  vm_output("\n");

  /* Restore dynamic mode */
  if (was_dynamic)
    {
      if (world.curjob->current && world.curjob->current->linkmap)
	elfsh_set_debug_mode();
      else 
	vm_output("\n [!] Loaded file not present in linkmap"
		  ", switching to STATIC mode\n\n");
    }
  else 
      elfsh_set_static_mode();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));

  /* We were not able to resolve the parameter */
 err:
  if (was_dynamic)
    elfsh_set_debug_mode();
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unknown file to load", (-1));
}


