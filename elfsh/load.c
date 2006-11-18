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

/* Search the lib path for a specific file */
char	     	*vm_load_searchlib(char *name)
{
  int		len;
  char		libvar[BUFSIZ];
  char		*libpath;
  char	       	*retpath = NULL;
  char	 	*p;
  char		*f;
  int	        dlen;
  char		testpath[BUFSIZ];
  char		tmplibpath[BUFSIZ];
  FILE		*fp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  /* A variable start with ELFSH_VARPREF */
  snprintf(libvar, BUFSIZ - 1, "%c%s", 
	   ELFSH_VARPREF, ELFSH_LIBPATHVAR);

  libpath = vm_lookup_var(libvar);

  /* The variable doesn't exist */
  if (libpath == NULL || libpath == libvar)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "ELFSH_LIBPATH not found", NULL);

  /* We copy data because we will make some modifications */
  strncpy(tmplibpath, libpath, BUFSIZ -1);
  tmplibpath[BUFSIZ - 1] = '\0';

  len = strlen(tmplibpath);
  for (p = tmplibpath; p != NULL && p[0] != '\0'; p = f+1)
    {
      f = strchr(p, ';');

      /* If a ; is found, we seperate the tree */
      if (f != NULL)
	  f[0] = '\0';
      
      /* Trim ended / */
      for (dlen = strlen(p); p[dlen] == '/'; dlen--)
	p[dlen] = '\0';

      snprintf(testpath, BUFSIZ - 1,
	       "%s/%s", p, name);

      /* The file exists, then we set the pointer */
      if ((fp = fopen(testpath, "r")) != NULL)
	{
	  fclose(fp);
	  retpath = testpath;
	  break;
	}

      /* If we don't found the ;, it's the last path */
      if (f == NULL)
	break;
    }

  if (retpath != NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       elfsh_strdup(retpath));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/* Our own pow */
static int	pow(int a, int b)
{
  double res = 1;
  float f;

  if (b == 0)
    return 1;
  
  if (b < 0)
    {
      for (; b < 0; b++)
	res = res * a;

      f = 1 / res;

      return (int) f;
    }

   for (; b > 0; b--)
       res = res * a;

   return (int) res;
}

/* Search a dependence from an id and a loaded object */
elfshobj_t	*vm_is_depid(elfshobj_t *obj, int id)
{
  elfshobj_t	*child;
  elfshobj_t	*subchild;
  int		tid;
  int		oid;
  int		tcount;
  int		ocount;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!obj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramters", NULL);    

  /* We don't search a dependence or we are too far */
  if (id < ELFSH_CHILD_MAX || !obj->deplist || obj->id > id)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  

  /* Count how many digits */
  for (ocount = 0, oid = obj->id; oid > 0; ocount++)
    oid /= 10;

  /* Count how many digits */
  for (tcount = 0, tid = id; tid > 0; tcount++)
    tid /= 10;

  oid = obj->id;
  tid = id;

  /* That a small check to gain time, it won't work on any case */
  if (oid == tid / pow(10, tcount - ocount))
    {
      for (child = obj->deplist; child != NULL; child = child->next)
	{
	  if (child->id == id)
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, child);

	  /* Search for a child of the child ! */
	  if ((subchild = vm_is_depid(child, id)) != NULL)
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, subchild);
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  
}

/* Search a dependence from a lib name and a loaded object */
elfshobj_t	*vm_is_dep(elfshobj_t *obj, char *path)
{
  char		**hashkeys;
  int		index;
  elfshobj_t	*child;
  int		keys_num;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!obj || !path)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramters", NULL);    

  hashkeys = hash_get_keys(&obj->child_hash, &keys_num);

  for (index = 0; hashkeys != NULL && index < keys_num; index++)
    {
      child = (elfshobj_t *) hash_get(&obj->child_hash, hashkeys[index]);
      
      if (child)
	{
	  if (!strcmp(path, hashkeys[index]))
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, child);  
      
	  child = vm_is_dep(child, path);
				    
	  if (child)
	    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, child);  
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  
}

/* Enum dependences */
int		vm_load_enumdep(elfshobj_t *obj)
{
  elfsh_Dyn	*dyn_entrie;
  int		dyn_num;
  int		index;
  char		nbasename[BUFSIZ];
  char		*path;
  elfshobj_t	*parent;
  elfshobj_t	*child;
  char		**hashkeys;
  int		keys_num;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!obj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);

  /* Retrieve .dynamic table for DT_NEEDED entries */
  dyn_entrie = elfsh_get_dynamic(obj, &dyn_num);

  if (dyn_entrie == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No .dynamic information", -1);

  /* Loop on the section entries */
  for (index = 0; index < dyn_num && dyn_entrie[index].d_tag != DT_NULL; index++)
    {
      /* Only needed entries */
      if (dyn_entrie[index].d_tag != DT_NEEDED)
	continue;

      /* Retrieve the value */
      bzero(nbasename, sizeof(nbasename));
      vm_dynentinfo(obj, dyn_entrie + index, nbasename);

      if (!nbasename)
	continue;

      /* Search the path for this library */
      path = vm_load_searchlib(nbasename);

      if (!path)
	continue;

      /* Search the parent of all this objects */
      for (parent = obj; parent != NULL && parent->parent != NULL; parent = parent->parent);

      /* Search the path on all the childs */
      child = vm_is_dep(parent, path);
      
      if (!child)
	vm_load_dep(obj, path, NULL, NULL);
    }

  /* Retrieve all childs keys */
  hashkeys = hash_get_keys(&obj->child_hash, &keys_num);

  for (index = 0; hashkeys != NULL && index < keys_num; index++)
    {
      child = (elfshobj_t *) hash_get(&obj->child_hash, hashkeys[index]);

      /* Load dep for each child, this way we won't load a same dep twice */
      if (child)
	vm_load_enumdep(child);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Load a dependence which is different from a normal file */
int		vm_load_dep(elfshobj_t *parent, char *name, elfsh_Addr base, elfshlinkmap_t *lm)
{
  elfshobj_t	*new;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!parent || !name)
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
      snprintf(logbuf, BUFSIZ - 1, " [*] New object dependences loaded : %s\n",
	      name);
      vm_output(logbuf);
    }

  /* Set base address */
  new->rhdr.base = base;

  /* Set linkmap address */
  new->linkmap = lm;

  /* Add the object of child objects */
  if (parent->lastchildid >= ELFSH_CHILD_MAX)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't create another child !", -1);    

  /* Generate a child id */
  new->id = ELFSH_CHILD_NEW(parent);

  /* Update dep list */
  if (!parent->deplist)
      parent->deplist = new;
  else
      parent->deplistlast->next = new;

  parent->deplistlast = new;

  /* Link with its parent */
  new->parent = parent;

  /* Add dep hash */
  hash_add(&parent->child_hash, new->name, (void *) new);

  /* Init hash dep */
  hash_init(&new->child_hash, 20);  
    
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

   mjr_create_context_as_current(&world.mjr_session, new);
   mjr_setup_processor(&world.mjr_session);
#endif

  /* Add an entry into the loaded files hashtable */
  hash_add(&file_hash, new->name, (void *) new);

  /* Init hash dep */
  hash_init(&new->child_hash, 20);

  /* Load dependances */
  vm_load_enumdep(new);

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

  tmp = NULL;

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


