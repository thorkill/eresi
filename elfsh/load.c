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

  if (!name)
    return (0);
  
  if (!world.curjob)
    return (0);

  if (!world.curjob->current)
    return (0);

  obj = world.curjob->current;
  
  while(obj)
    {
      if (!strcmp(name, obj->name))
	return (1);
      
      obj = obj->next;
    }

  obj = world.shared;
  
  while(obj)
    {
      if (!strcmp(name, obj->name))
	return (1);
      
      obj = obj->next;
    }


  return (0);
}


/* Load a file in elfsh */
int		vm_load_file(char *name, elfsh_Addr base, elfshlinkmap_t *lm)
{
  elfshobj_t	*new;
  elfshpath_t	*tmp;
  char		logbuf[BUFSIZ];
  char		*timec;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!name)
    ELFSH_SETERROR("[elfsh:load_file] invalid argument\n", -1);

  /* Map the standard ELF object */
  new = elfsh_map_obj(name);
  if (NULL == new)
    {
      snprintf(logbuf, BUFSIZ - 1, " [!] Cannot load object ");
      vm_output(logbuf);
      PERROR_RET(name, -1);
    }

  /* Print a msg if not in quiet mode */
  new->loadtime = time(&new->loadtime);
  if (!world.state.vm_quiet)
    {
      timec = ctime(&new->loadtime);
      timec[strlen(timec) - 1] = '\0';
      snprintf(logbuf, BUFSIZ - 1, " [*] %s - New object %s loaded\n",
	      timec , name);
      vm_output(logbuf);
    }

  /* Set base address */
  new->base = base;

  /* Set linkmap address */
  new->linkmap = lm;

  /* Add the object to the list of opened objects */
  new->id = ++world.state.lastid;
  world.curjob->current = new;
  tmp = hash_get(&vars_hash, "!");
  if (!tmp)
    ELFSH_SETERROR("[elfsh:cmd_load] TROUBLE : Last loaded file inexistant\n", -1);
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

  /* Add an entry into the loaded files hashtable */
  hash_add(&file_hash, new->name, (void *) new);

  return (0);
}


/* Insert an object in the list of opened elfsh descriptors */
int		cmd_load()
{
  elfshpath_t	*tmp;
  char		*str;
  int		was_dynamic = 0;
  int		ret; 

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_is_debug_mode())
    {
      was_dynamic = 1;
      elfsh_set_static_mode();
    }

  /* Cannot use vm_lookup_file() here because the file is not yet loaded */
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

  if (was_dynamic)
    {
      if (world.curjob->current->linkmap)
	elfsh_set_debug_mode();
      else 
	vm_output("\n [!!] Loaded file is not the linkmap, switching to STATIC mode\n\n");
    }
  else 
      elfsh_set_static_mode();

  return (ret);

 err:
  if (was_dynamic)
    elfsh_set_debug_mode();
  return (-1);
}


