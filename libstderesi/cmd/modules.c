/**
** @file modules.c
** @ingroup libstderesi
** Started on  Wed Feb 19 04:42:47 2003 jfv
**
** $Id: modules.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"


/** 
 * Guess if the file is an ELFsh module or not 
 */
int		cmd_modload()
{
  revmmod_t	*new;
  char		buf[BUFSIZ];
  char		*errmsg;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Verify if the file exists */
  if (access(world.curjob->curcmd->param[0], R_OK) != 0)
    {
      snprintf(buf, sizeof(buf), "%s%s", ERESI_MODPATH, world.curjob->curcmd->param[0]);
      if (access(buf, R_OK) != 0)
	{
	  snprintf(buf, sizeof(buf), "%s%s.so",
		   ERESI_MODPATH, world.curjob->curcmd->param[0]);
	  if (access(buf, R_OK) != 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cannot find module", -1);
	}
      name = buf;
    }
  else
    name = world.curjob->curcmd->param[0];
  
  /* See if the module isnt already loaded */
  new = hash_get(&mod_hash, name);
  if (new != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Module already loaded", -1);

  /* Load the module and create revmmod_t entry */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmmod_t), -1);
#ifdef __BEOS__
  new->handler = load_add_on(name);
  if (new->handler == B_ERROR)
#else
  new->handler = dlopen(name, RTLD_NOW);
  if (new->handler == NULL)
#endif
    {
      errmsg = "[elfsh:modload] Cannot load module";
      goto err;
    }
  /* Find constructor and (optional) destructor */
#ifdef __BEOS__
  if( get_image_symbol(new->handler, ELFSH_INIT, 
        B_SYMBOL_TYPE_TEXT, (void**)(&(new->init))) != B_OK)
#else
  new->init = dlsym(new->handler, ELFSH_INIT);
  if (new->init == NULL)
#endif
    {
      errmsg = "[elfsh:modload] Cannot find init handler";
      goto err;
    }
#ifdef __BEOS__
  get_image_symbol(new->handler, ELFSH_FINI, 
        B_SYMBOL_TYPE_TEXT, (void**)(&(new->fini)));
#else
  new->fini = dlsym(new->handler, ELFSH_FINI);
  new->help = dlsym(new->handler, ELFSH_HELP);
#endif
  new->id   = ++world.state.lastid;
  new->path = strdup(name);
 
  time(&new->loadtime);
  new->next = world.modlist;
  world.modlist = new;
  hash_add(&mod_hash, new->path, new);
  new->init();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Error handling */
 err:
  XFREE(__FILE__, __FUNCTION__, __LINE__,new);
#ifdef __BEOS__
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, errmsg, -1);
#else
  snprintf(buf, sizeof(buf), "%s: %s", errmsg, dlerror());
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, buf, -1);
#endif
}


/** 
 * Unload an elfsh module 
 */
int		cmd_modunload()
{
  revmmod_t	*cur;
  revmmod_t	*act;
  revmmod_t	*todel;
  time_t	unload_time;
  unsigned int		id;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  id = atoi(world.curjob->curcmd->param[0]);
  cur = 0;
  if (!id)
    {
      if (access(world.curjob->curcmd->param[0], R_OK) != 0)
      {
	snprintf(buf, sizeof(buf), "%s%s", ERESI_MODPATH, world.curjob->curcmd->param[0]);
	if (access(buf, R_OK) != 0)
	  {
	    snprintf(buf, sizeof(buf), "%s%s.so",
		     ERESI_MODPATH, world.curjob->curcmd->param[0]);
	    if (access(buf, R_OK) != 0)
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
				"Cannot find module", -1);
	  }
	else
	  cur = hash_get(&mod_hash, buf);
      }
    }
  else
    cur = revm_getmod(id);
  
  /* Check if the object is in the module list */
  if (cur == NULL)
    goto bad;

  /* Call the handlers */
  if (cur->fini != NULL)
    cur->fini();
#ifdef __BEOS__
  unload_add_on(cur->handler);
#else
  dlclose(cur->handler);
#endif

  /* If the element is the first of the list, update the head pointer ... */
  if (world.modlist->id == cur->id)
    {
      todel = world.modlist;
      world.modlist = world.modlist->next;
      goto end;
    }

  /* ... else find the object in the list */
  for (act = world.modlist; act != NULL && act->next != NULL; act = act->next)
    if (act->next->id == cur->id)
      {
	todel = act->next;
	act->next = act->next->next;
	goto end;
      }

 bad:
  /* We didnt find the module */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Module is not loaded", -1);

  /* We found the module */
 end:
  unload_time = time(&unload_time);
  snprintf(buf, BUFSIZ - 1,
	   " [*] Module %s unloaded on %s \n",
	   todel->path, ctime(&unload_time));
  revm_output(buf);
  hash_del(&mod_hash, todel->path);
  XFREE(__FILE__, __FUNCTION__, __LINE__,todel->path);
  XFREE(__FILE__, __FUNCTION__, __LINE__,todel);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* In case linked in static */
#if USE_STATIC && !defined(__FreeBSD__)

void * dlopen(const char *pathname, int mode)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "DLOPEN unavailable in static built", (NULL));
}

void *dlsym(void *handle, const char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "DLSYM unavailable in static built", (NULL));
}

int dlclose(void *handle)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "DLCLOSE unavailable in static built", (1));
}

char *dlerror(void)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "DLERROR unavailable in static built", (NULL));
}

#endif 
