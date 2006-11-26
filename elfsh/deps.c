/*
** deps for elfsh
**
** All functions regarding dependences support in the naming scheme and the
** loading/unloading facilities of the shell
**
** Started on Sat Nov 25 11:21:18 2006 mayhem
*/
#include "elfsh.h"
  

/* Our own pow */
static int	__eint_pow(int a, int b)
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


/* Find a different element than 'file' in the parent hash table */
static void*		vm_get_another_parent(hash_t *hash, elfshobj_t *file) 
{
  char		**keys;
  int		index;
  int		idx;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!hash || hash_size(hash) <= 1)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Unable to find another parent", NULL);
  keys = hash_get_keys(hash, &index);
  for (idx = 0; idx < index; idx++)
    if (strcmp(keys[idx], file->name))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
			 hash_get(hash, keys[idx]));
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to find another parent", NULL);
}


/* 
** Merge the root dependences information for all the children, when a new
** program with dependences is (un)loaded and some dependences also depends
** from other roots.
*/
static int		vm_update_depinfo(elfshobj_t	*child, 
					  elfshobj_t	*root,
					  char		action)
{
  char		**key;
  int		index;
  int		keynbr;
  elfshobj_t	*cur;
  int		newid;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  /* Change ID of object before recursing */
  if (action & ELFSH_NEWID)
    {
      cur = vm_get_another_parent(&child->root_hash, root);
      newid = ELFSH_CHILD_NEW(cur);
      child->id = newid;
    }

  /* ... including on the child objects */
  key = hash_get_keys(&child->child_hash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(&child->child_hash, key[index]);
      if (vm_update_depinfo(cur, root, action))
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Dependence failed to perform action", -1);
    }

  /* Remove the roots after recursion, so avoid unwanted unloading */
  if (action & ELFSH_MERGE)
    hash_merge(&child->root_hash, &root->root_hash);
  else if (action & ELFSH_UNMERGE)
    hash_unmerge(&child->root_hash, &root->root_hash);

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

  



/* Search a dependence from an id and a loaded object */
elfshobj_t	*vm_is_depid(elfshobj_t *obj, int id)
{
  elfshobj_t	*child;
  elfshobj_t	*subchild;
  int		tid;
  int		oid;
  int		tcount;
  int		ocount;
  int		keynums;
  char		**keys;
  int		index;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!obj)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramters", NULL);    

  /* We don't search a dependence or we are too far */
  if (id < ELFSH_CHILD_MAX || !hash_size(&obj->child_hash) || obj->id > id)
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
  if (oid == tid / __eint_pow(10, tcount - ocount))
    {
      keys = hash_get_keys(&obj->child_hash, &keynums);
      for (index = 0; index < keynums; index++)
	{
	  child = hash_get(&obj->child_hash, keys[index]);
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

  /* Loop on the .dynamic section entries */
  for (index = 0; index < dyn_num && dyn_entrie[index].d_tag != DT_NULL; index++)
    {
      /* Only needed entries */
      if (dyn_entrie[index].d_tag != DT_NEEDED)
	continue;

      /* Retrieve the value */
      vm_dynentinfo(obj, dyn_entrie + index, nbasename);
      if (*nbasename == 0x00)
	continue;

      /* Search the path for this library */      
      path = vm_load_searchlib(nbasename);
      if (!path)
	continue;

      child = hash_get(&world.curjob->loaded, path);
      if (!child)
	vm_load_dep(obj, path, NULL, NULL);
      else
	{
	  if (!hash_get(&obj->child_hash, child->name))
	    hash_add(&obj->child_hash, child->name, child);
	  if (!hash_get(&child->parent_hash, obj->name))
	    hash_add(&child->parent_hash, obj->name, obj);
	  vm_update_depinfo(child, obj, ELFSH_MERGE);
	}
    }

  /* Retrieve all childs keys then load dep for each child */
  hashkeys = hash_get_keys(&obj->child_hash, &keys_num);
  for (index = 0; hashkeys != NULL && index < keys_num; index++)
    {
      child = (elfshobj_t *) hash_get(&obj->child_hash, hashkeys[index]);
      if (child)
	vm_load_enumdep(child);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Load a dependence which is different from a normal file */
int		vm_load_dep(elfshobj_t *parent, char *name, 
			    elfsh_Addr base, elfshlinkmap_t *lm)
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

  /* Set base address and linkmap addr */
  new->rhdr.base = base;
  new->linkmap = lm;

  /* Add the object of child objects */
  if (parent->lastchildid >= ELFSH_CHILD_MAX)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create more child", -1);    

  /* Generate a child id */
  new->id = ELFSH_CHILD_NEW(parent);

  /* Init hash tables */
  hash_init(&new->child_hash , 11);  
  hash_init(&new->parent_hash, 11);  
  hash_init(&new->root_hash  , 11);  

  /* Link with its parent */
  /* Link with all the additional root files of its parent */
  /* Add reference to the private (or shared) list */
  hash_add(&new->parent_hash, parent->name, parent);
  hash_add(&parent->child_hash, new->name, new);
  if (world.state.vm_shared)
    hash_add(&world.shared_hash, new->name, new);
  else
    hash_add(&world.curjob->loaded, new->name, new);
  hash_add(&file_hash, new->name, new);
  vm_update_depinfo(new, parent, ELFSH_MERGE);
    
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Unload all dependence for an object */
int		vm_unload_dep(elfshobj_t *obj, elfshobj_t *root)
{
  elfshobj_t	*actual;
  char		logbuf[BUFSIZ];
  char		**keys;
  int		index;
  int		keynbr;
  time_t	uloadt;
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Unload each dependence */
  keys = hash_get_keys(&obj->child_hash, &keynbr);
  for (ret = index = 0; index < keynbr; index++)
    {
      actual = hash_get(&obj->child_hash, keys[index]);
      if (!actual)
	continue;

      /* Case where we should -not- unload this object and its dependences */
      /* Renew the object id, if its father changes */
      if (hash_size(&actual->root_hash) > 1 || 
	  !hash_get(&actual->root_hash, root->name))
	{
	  snprintf(logbuf, BUFSIZ - 1, " [*] Object %s -NOT- unloaded\n",
		   actual->name);
	  vm_output(logbuf);	  

	  /* 
	  ** If the object was already unlinked from its parent at another 
	  ** iteration of this function, do not remove the roots and change
	  ** its id again -may
	  */
	  if (hash_get(&actual->root_hash, root->name))
	    {
	      hash_del(&actual->parent_hash, obj->name);
	      vm_update_depinfo(actual, root, ELFSH_NEWID | ELFSH_UNMERGE);
	    }
	  continue;
	}

      /* The current pointer must be NULL if it was us */
      if (world.curjob->current && world.curjob->current->id == actual->id)
	{
	  world.curjob->current = hash_get_one(&world.curjob->loaded);
	  if (!world.curjob->current)
	    world.curjob->current = hash_get_one(&world.shared_hash);
	}

      /* A dependence can have its own dependences */
      vm_unload_dep(actual, root);
      hash_del(&file_hash, actual->name);
      if (hash_get(&world.shared_hash, actual->name))
	hash_del(&world.shared_hash, actual->name);
      else
	hash_del(&world.curjob->loaded, actual->name);

      /* Print message of removing */
      if (!world.state.vm_quiet)
	{
	  time(&uloadt);
	  snprintf(logbuf, BUFSIZ - 1, " [*] Object %s unloaded on %s",
		   actual->name, ctime(&uloadt));
	  vm_output(logbuf);	  
	}

      elfsh_unload_obj(actual);
      ret++;
    }

  /* Disable the force flag and return OK */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
