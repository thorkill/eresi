/*
** @file list.c
** @ingroup libstderesi 
** Started on  Sat Jan 25 11:20:20 2003 jfv
**
** $Id: list.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"

/** 
 * Print dependence 
 * @param obj
 */
static int	revm_dolist_dep(elfshobj_t *obj)
{
  elfshobj_t	*actual;
  char		logbuf[20];
  char		**keys;
  int		keynbr;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);
  
  if (hash_size(&obj->child_hash))
    {
      keys = hash_get_keys(&obj->child_hash, &keynbr);
      revm_output("DEPS = [");
      for (index = 0; index < keynbr; index++)
	{
	  actual = hash_get(&obj->child_hash, keys[index]);
	  snprintf(logbuf, sizeof(logbuf), "%s%u", 
		   (index == 0 ? "" : ","), actual->id);
	  revm_output(logbuf);
	}
      revm_output("]");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * List all the loaded objects 
 */
int		cmd_dolist()
{
  elfshobj_t	*actual;
  int		index;
  char		*time;
  char		*nl;
  char		c;
  char		c2;
  char		logbuf[BUFSIZ];
  char		optbuf[BUFSIZ];
  char		**keys;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  index = 1;

  /* Private descriptors */
  if (hash_size(&world.curjob->loaded))
    {
      revm_output(" .::. Static Working files .::. \n");
      keys = hash_get_keys(&world.curjob->loaded, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  actual = hash_get(&world.curjob->loaded, keys[index]);
	  time = ctime(&actual->loadtime);
	  nl = strchr(time, '\n');
	  if (nl)
	    *nl = 0x00;
	  c = (world.curjob->curfile == actual ? '*' : ' ');
	  c2 = ((actual->linkmap || actual->rhdr.base) ? 'M' : ' ');
	  if (elfsh_is_runtime_mode())
	    snprintf(optbuf, BUFSIZ, "(" XFMT ")", actual->rhdr.base);
	  else
	    snprintf(optbuf, BUFSIZ, "%s", "");
	  
	  snprintf(logbuf, BUFSIZ - 1, " %s %c%c %s ID: %10u %s %-31s ", 
		   time, c, c2, optbuf, actual->id, 
		   elfsh_get_objtype(actual->hdr) == ET_REL  ? "ET_REL " : 
		   elfsh_get_objtype(actual->hdr) == ET_DYN  ? "ET_DYN " : 
		   elfsh_get_objtype(actual->hdr) == ET_EXEC ? "ET_EXEC" : 
		   elfsh_get_objtype(actual->hdr) == ET_CORE ? "ET_CORE" : 
		   "UNKNOWN", actual->name);
	  revm_output(logbuf);
	  revm_dolist_dep(actual);
	  revm_output("\n");

	  /* printf("-> Hashes for object : PAR[%u] ROOT[%u] CHILD[%u] \n",
	     hash_size(&actual->parent_hash),
	     hash_size(&actual->root_hash),
	     hash_size(&actual->child_hash));
	  */

	}
    }

  /* Shared descriptors */
  if (hash_size(&world.shared_hash))
    {
      revm_output("\n .::. Shared Working files .::. \n");
      keys = hash_get_keys(&world.shared_hash, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  actual = hash_get(&world.shared_hash, keys[index]);
	  time = ctime(&actual->loadtime);
	  nl = strchr(time, '\n');
	  if (nl)
	    *nl = 0x00;
	  c = (world.curjob->curfile == actual ? '*' : ' ');
	  c2 = (actual->linkmap ? 'L' : ' ');
	  if (elfsh_is_runtime_mode())
	    snprintf(optbuf, BUFSIZ, "(" XFMT ")", actual->rhdr.base);
	  else
	    snprintf(optbuf, BUFSIZ, "%s", "");
	  
	  snprintf(logbuf, BUFSIZ - 1, " [%02u] %s %c%c %s ID: %02u %-31s \n", 
		   index + 1, time, c, c2, optbuf, actual->id, actual->name);
	  revm_output(logbuf);
	}
    }

  if (!hash_size(&world.curjob->loaded) && !hash_size(&world.shared_hash))
    revm_output(" [*] No loaded file\n");
  revm_output("\n");
  revm_modlist();
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
