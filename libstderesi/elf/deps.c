/**
** @file deps.c
**
** @brief All functions regarding dependences support in the naming scheme and the
** loading/unloading facilities of the shell
**
** Started on Sat Nov 25 11:21:18 2006 jfv
**
** $Id: deps.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"
  

/** 
 * Our own pow 
 * @param a
 * @param b
 * @return
 */
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


/** 
 * Find a different element than 'file' in the parent hash table 
 * @param hash
 * @param file
 * @return
 */
static void*	revm_get_another_parent(hash_t *hash, elfshobj_t *file) 
{
  char		**keys;
  int		index;
  int		idx;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!hash || hash_size(hash) <= 1)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Unable to find another parent", NULL);
  keys = hash_get_keys(hash, &index);
  for (idx = 0; idx < index; idx++)
    if (strcmp(keys[idx], file->name))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
			 hash_get(hash, keys[idx]));
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to find another parent", NULL);
}


/** 
 * Merge the root dependences information for all the children, when a new
 * program with dependences is (un)loaded and some dependences also depends
 * from other roots.
 * @param child
 * @param root
 * @param dephash
 * @param action
 * @return
 */
static int	revm_update_depinfo(elfshobj_t	*child, 
				    elfshobj_t	*root,
				    hash_t	*dephash,
				    char	action)
{
  char		**key;
  int		index;
  int		keynbr;
  elfshobj_t	*cur;
  int		newid;
  void		*present;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  /* Cut the recursion in case of circular dependencies */
  present = hash_get(dephash, child->name);
  if (present)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  hash_add(dephash, strdup(child->name), (void *) 1);

  /* Change ID of object before recursing */
  if (action & REVM_CREATE_NEWID)
    {
      cur = revm_get_another_parent(&child->root_hash, root);
      newid = ELFSH_CHILD_NEW(cur);
      child->id = newid;
    }

  /* ... including on the child objects */
  key = hash_get_keys(&child->child_hash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(&child->child_hash, key[index]);
      if (revm_update_depinfo(cur, root, dephash, action))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Dependence failed to perform action", -1);
    }

  /* Remove the roots after recursion, so avoid unwanted unloading */
  if (action & REVM_HASH_MERGE)
    hash_merge(&child->root_hash, &root->root_hash);
  else if (action & REVM_HASH_UNMERGE)
    hash_unmerge(&child->root_hash, &root->root_hash);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Enum dependences 
 * @param obj
 * @param rechash
 * @param dephash
 * @return
 *
 */
static int	revm_load_enumdep_rec(elfshobj_t *obj, hash_t *rechash, hash_t *dephash)
{
  elfsh_Dyn	*dyn_entrie;
  unsigned int		dyn_num;
  unsigned int		index;
  char		nbasename[BUFSIZ];
  char		*path;
  elfshobj_t	*child;
  char		**hashkeys;
  int		keys_num;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  /* Avoid invalid arguments and cut recursion when needed */
  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid argument", -1);
  if (hash_get(rechash, obj->name))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  hash_add(rechash, strdup(obj->name), (void *) 1);

  /* Retrieve .dynamic table for DT_NEEDED entries */
  dyn_entrie = elfsh_get_dynamic(obj, &dyn_num);
  if (dyn_entrie == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No .dynamic information", -1);

  /* Loop on the .dynamic section entries */
  for (index = 0; index < dyn_num && dyn_entrie[index].d_tag != DT_NULL; index++)
    {
      /* Only needed entries */
      if (dyn_entrie[index].d_tag != DT_NEEDED)
	continue;

      /* Retrieve the value */
      revm_dynentinfo(obj, dyn_entrie + index, nbasename);
      if (*nbasename == 0x00)
	continue;

      /* Search the path for this library */      
      path = revm_load_searchlib(nbasename);
      if (!path)
	continue;

      child = hash_get(&world.curjob->loaded, path);
      if (!child)
	revm_load_dep(obj, path, 0, 0, dephash);
      else
	{
	  if (!hash_get(&obj->child_hash, child->name))
	    hash_add(&obj->child_hash, child->name, child);
	  if (!hash_get(&child->parent_hash, obj->name))
	    hash_add(&child->parent_hash, obj->name, obj);
	  revm_update_depinfo(child, obj, dephash, REVM_HASH_MERGE);
	}
    }

  /* Retrieve all childs keys then load dep for each child */
  hashkeys = hash_get_keys(&obj->child_hash, &keys_num);
  for (index = 0; hashkeys != NULL && index < keys_num; index++)
    {
      child = (elfshobj_t *) hash_get(&obj->child_hash, hashkeys[index]);
      if (child)
	revm_load_enumdep_rec(child, rechash, dephash);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * This is a front end function for the recursive function above 
 * @param obj
 * @return
 */
int		revm_load_enumdep(elfshobj_t *obj)
{
  hash_t	rechash;
  hash_t	dephash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bzero(&rechash, sizeof(rechash));
  bzero(&dephash, sizeof(dephash));
  hash_init(&rechash, "loaded_dependences_files", 20, ASPECT_TYPE_UNKNOW);
  hash_init(&dephash, "recursed_dependences_files", 20, ASPECT_TYPE_UNKNOW);
  revm_load_enumdep_rec(obj, &rechash, &dephash);
  hash_destroy(&rechash);
  hash_destroy(&dephash);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Search the lib path for a specific file 
 * @param name Filename to search
 * @return
 */
char	     	*revm_load_searchlib(char *name)
{
  int		len;
  char		*libpath;
  char	       	*retpath = NULL;
  char	 	*p;
  char		*f, *f2;
  int	        dlen;
  char		testpath[BUFSIZ];
  char		tmplibpath[BUFSIZ];
  FILE		*fp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  libpath = revm_lookup_string(REVM_VAR_LIBPATH);

  /* The variable doesn't exist */
  if (libpath == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "ERESI_LIBPATH not found", NULL);

  /* We copy data because we will make some modifications */
  strncpy(tmplibpath, libpath, BUFSIZ -1);
  tmplibpath[BUFSIZ - 1] = '\0';

  len = strlen(tmplibpath);
  for (p = tmplibpath; p != NULL && p[0] != '\0'; p = f + 1)
    {
      f  = strchr(p, ';');
      f2 = strchr(p, ':');
      if ((!f && f2) || (f2 && f - f2 > 0))
	f = f2;

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
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       strdup(retpath));
 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/** 
 * Search a dependence from an id and a loaded object. 
 * @param obj
 * @param id
 * @return 
 */
elfshobj_t	*revm_is_depid(elfshobj_t *obj, int id)
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
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramters", NULL);    

  /* We don't search a dependence or we are too far */
  if (id < ELFSH_CHILD_MAX || !hash_size(&obj->child_hash) || obj->id > id)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  

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
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);

	  /* Search for a child of the child ! */
	  if ((subchild = revm_is_depid(child, id)) != NULL)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, subchild);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  
}




/** 
 * Search a dependence from a lib name and a loaded object 
 * @param obj
 * @param path
 * @return
 */
elfshobj_t	*revm_is_dep(elfshobj_t *obj, char *path)
{
  char		**hashkeys;
  int		index;
  elfshobj_t	*child;
  int		keys_num;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!obj || !path)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid paramters", NULL);    

  hashkeys = hash_get_keys(&obj->child_hash, &keys_num);

  for (index = 0; hashkeys != NULL && index < keys_num; index++)
    {
      child = (elfshobj_t *) hash_get(&obj->child_hash, hashkeys[index]);
      
      if (child)
	{
	  if (!strcmp(path, hashkeys[index]))
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);  
      
	  child = revm_is_dep(child, path);				    
	  if (child)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, child);  
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);  
}

/** 
 * Load a dependence which is different from a normal file.
 * @param parent
 * @param name
 * @param base
 * @param lm
 * @param dephash
 * @return
 */
int		revm_load_dep(elfshobj_t *parent, char *name, 
			      eresi_Addr base, elfshlinkmap_t *lm,
			      hash_t *dephash)
{
  elfshobj_t	*new;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!parent || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);

  /* Map the standard ELF object */
  new = elfsh_map_obj(name);
  if (NULL == new)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot load object", -1);
  if (elfsh_get_arch(new->hdr) != elfsh_get_arch(parent->hdr))
    {
      snprintf(logbuf, sizeof (logbuf) - 1, "Parent file and dependance %s"
	       " architecture unmatched", name);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			logbuf, -1);
    }
  
  /* Print a msg if not in quiet mode */
  new->loadtime = time(&new->loadtime);
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] New object dependences loaded : %s\n",
	      name);
      revm_output(logbuf);
    }

  /* Set base address and linkmap addr */
  new->rhdr.base = base;
  new->linkmap = lm;

  /* Add the object of child objects */
  if (parent->lastchildid >= ELFSH_CHILD_MAX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot create more child", -1);    

  /* Generate a child id */
  new->id = ELFSH_CHILD_NEW(parent);

  /* Init search hash tables */
  elfsh_init_symbol_hashtables(new);

  /* Parse debugging informations */
  revm_edfmt_parse(new);

  /* Initialize hash tables */
  snprintf(logbuf, sizeof(logbuf), "%s_children", name);
  hash_init(&new->child_hash, strdup(logbuf), 11, ASPECT_TYPE_UNKNOW);
 
  snprintf(logbuf, sizeof(logbuf), "%s_parents", name);
  hash_init(&new->parent_hash, strdup(logbuf), 11, ASPECT_TYPE_UNKNOW);  
 
  snprintf(logbuf, sizeof(logbuf), "%s_roots", name);
  hash_init(&new->root_hash, strdup(logbuf), 11, ASPECT_TYPE_UNKNOW);  
 

  /* Link with its parent */
  /* Link with all the additional root files of its parent */
  /* Add reference to the private (or shared) list */
  hash_add(&new->parent_hash, parent->name, parent);
  hash_add(&parent->child_hash, new->name, new);
  if (world.state.revm_shared)
    hash_add(&world.shared_hash, new->name, new);
  else
    hash_add(&world.curjob->loaded, new->name, new);
  hash_add(&file_hash, new->name, new);
  revm_update_depinfo(new, parent, dephash, REVM_HASH_MERGE);
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Unload all dependence for an object.
 * @param obj
 * @param root
 * @param rechash
 * @param dephash
 * @return
 */
static int	revm_unload_dep_rec(elfshobj_t *obj, elfshobj_t *root, 
				    hash_t *rechash, hash_t *dephash)
{
  elfshobj_t	*actual;
  char		logbuf[BUFSIZ];
  char		**keys;
  int		index;
  int		keynbr;
  time_t	uloadt;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (hash_get(rechash, obj->name))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  hash_add(rechash, strdup(obj->name), (void *) 1);

  /* Unload each dependence */
  keys = hash_get_keys(&obj->child_hash, &keynbr);
  for (ret = index = 0; index < keynbr; index++)
    {
      actual = hash_get(&obj->child_hash, keys[index]);
      if (!actual || !actual->name || hash_get(rechash, actual->name))
	continue;

      //printf("Unloading dependence %s \n", actual->name);

      /* Case where we should -not- unload this object and its dependences */
      /* Renew the object id, if its father changes */
      if (hash_size(&actual->root_hash) > 1 || 
	  !hash_get(&actual->root_hash, root->name))
	{
	  snprintf(logbuf, BUFSIZ - 1, " [*] Object %s -NOT- unloaded\n",
		   actual->name);
	  revm_output(logbuf);	  

	  /* 
	  ** If the object was already unlinked from its parent at another 
	  ** iteration of this function, do not remove the roots and change
	  ** its id again -may
	  */
	  if (hash_get(&actual->root_hash, root->name))
	    {
	      hash_del(&actual->parent_hash, obj->name);
	      revm_update_depinfo(actual, root, dephash, REVM_CREATE_NEWID | REVM_HASH_UNMERGE);
	    }
	  continue;
	}

      /* The current pointer must be NULL if it was us */
      if (world.curjob->curfile && world.curjob->curfile->id == actual->id)
	{
	  world.curjob->curfile = hash_get_one(&world.curjob->loaded);
	  if (!world.curjob->curfile)
	    world.curjob->curfile = hash_get_one(&world.shared_hash);
	}

      /* A dependence can have its own dependences */
      hash_del(&file_hash, actual->name);
      if (hash_get(&world.shared_hash, actual->name))
	hash_del(&world.shared_hash, actual->name);
      else
	hash_del(&world.curjob->loaded, actual->name);
      revm_unload_dep_rec(actual, root, rechash, dephash);

      /* Print message of removing */
      if (!world.state.revm_quiet)
	{
	  time(&uloadt);
	  snprintf(logbuf, BUFSIZ - 1, " [*] Object %-40s unloaded on %s",
		   actual->name, ctime(&uloadt));
	  revm_output(logbuf);	  
	}
      elfsh_unload_obj(actual);
      ret++;
    }

  /* Disable the force flag and return OK */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/** 
 * This is a front end function for the recursive function above.
 * @param obj
 * @param root
 * @return Always 0
 */
int		revm_unload_dep(elfshobj_t *obj, elfshobj_t *root)
{
  hash_t	rechash;
  hash_t	dephash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bzero(&rechash, sizeof(rechash));
  bzero(&dephash, sizeof(dephash));
  hash_init(&rechash, "unloaded_dependences_files", 20, ASPECT_TYPE_UNKNOW);
  hash_init(&dephash, "unrecursed_dependences_files", 20, ASPECT_TYPE_UNKNOW);
  revm_unload_dep_rec(obj, root, &rechash, &dephash);
  hash_destroy(&rechash);
  hash_destroy(&dephash);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
