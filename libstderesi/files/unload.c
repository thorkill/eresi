/**
 * @file unload.c
 * @ingroup libstderesi
 * Started on  Sat Jan 25 11:21:52 2003 jfv
 * Last update Thu Mar 11 07:56:36 2004 jfv
 *
 * $Id: unload.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"


/** 
 * Unload a file from the shell 
 */
int		cmd_unload()
{
  hash_t	*loaded;
  elfshobj_t	*cur;
  time_t	uloadt;
  char		logbuf[BUFSIZ];
  int		ret;
  unsigned int		id;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

   /* Get the file structure */
  id = atoi(world.curjob->curcmd->param[0]);
  cur = revm_lookup_file(world.curjob->curcmd->param[0]);
  if (cur == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Object not loaded", -1);

  /* Do not unload dependences of files or objects with linkmap entry */
  if (hash_size(&cur->parent_hash))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unload parent object first", -1);
  if (cur->linkmap)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "You cannot unload a mapped object", -1);
  ret = revm_unload_dep(cur, cur);
  if (!world.state.revm_quiet)
    {
      time(&uloadt);
      snprintf(logbuf, BUFSIZ - 1, "%s [*] Object %-40s unloaded on %s \n",
	       (ret ? "" : "\n"), cur->name, ctime(&uloadt));
      revm_output(logbuf);
    }

  /* Clean various hash tables of this binary entry and return OK */
  hash_del(&file_hash, cur->name);
  if (hash_get(&world.shared_hash, cur->name))
    hash_del(&world.shared_hash, cur->name);
  else
    hash_del(&world.curjob->loaded, cur->name);
  elfsh_unload_obj(cur);
  revm_output("\n");

  /* If the file is the current working one, swap to another one */
  if (cur == world.curjob->curfile)
    {
      loaded = (hash_size(&world.curjob->loaded) ? 
		&world.curjob->loaded : hash_size(&world.shared_hash) ? 
		&world.shared_hash : NULL);
      if (!loaded)
	world.curjob->curfile = NULL;
      else 
	do cur = hash_get_one(loaded);
	while (cur == world.curjob->curfile);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



