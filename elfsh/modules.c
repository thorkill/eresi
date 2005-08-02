/*
** modules.c for elfsh
**
** Started on  Wed Feb 19 04:42:47 2003 mayhem
*/
#include "elfsh.h"


/* Guess if the file is an ELFsh module or not */
int		cmd_modload()
{
  elfshmod_t	*new;
  char		buf[BUFSIZ];
  char		*errmsg;
  char		*name;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Verify if the file exists */
  if (access(world.curjob->curcmd->param[0], R_OK) != 0)
    {
      snprintf(buf, sizeof(buf), "%s%s", ELFSH_MODPATH, world.curjob->curcmd->param[0]);
      if (access(buf, R_OK) != 0)
	{


	  //fprintf(stderr, "Failed to load %s, trying to load next match .. \n", buf);


	  snprintf(buf, sizeof(buf), "%s%s.so",
		   ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	  if (access(buf, R_OK) != 0)
	    ELFSH_SETERROR("[elfsh:modload] Cannot find module\n", -1);
	}
      name = buf;
    }
  else
    name = world.curjob->curcmd->param[0];
  
  /* See if the module isnt already loaded */
  new = hash_get(&mod_hash, name);
  if (new != NULL)
    ELFSH_SETERROR("[elfsh:modload] Module already loaded\n", -1);

  /* Load the module and create elfshmod_t entry */
  XALLOC(new, sizeof(elfshmod_t), NULL);
#ifdef __BEOS__
  new->handler = load_add_on(name);
  if (new->handler == B_ERROR)
#else
  fprintf(stderr, "name : %s\n",name);
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
  return (0);

  /* Error handling */
 err:
  free(new);
#ifdef __BEOS__
  ELFSH_SETERROR(errmsg, -1);
#else
  snprintf(buf, sizeof(buf), "%s: %s\n", errmsg, dlerror());
  ELFSH_SETERROR(buf, -1);
#endif
}


/* Unload an elfsh module */
int		cmd_modunload()
{
  elfshmod_t	*cur;
  elfshmod_t	*act;
  elfshmod_t	*todel;
  time_t	unload_time;
  u_int		id;
  char		buf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  id = atoi(world.curjob->curcmd->param[0]);
  cur = 0;
  if (!id)
    {
      if (access(world.curjob->curcmd->param[0], R_OK) != 0)
      {
	snprintf(buf, sizeof(buf), "%s%s", ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	if (access(buf, R_OK) != 0)
	  {
	    snprintf(buf, sizeof(buf), "%s%s.so",
		     ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	    if (access(buf, R_OK) != 0)
	      ELFSH_SETERROR("[elfsh:modload] Cannot find module\n", -1);
	  }
	else
	  cur = hash_get(&mod_hash, buf);
      }
    }
  else
    cur = vm_getmod(id);
  
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
  ELFSH_SETERROR("[elfsh:modunload] Module is not loaded\n", -1);

  /* We found the module */
 end:
  unload_time = time(&unload_time);
  snprintf(buf, BUFSIZ - 1,
	   " [*] Module %s unloaded on %s \n",
	   todel->path, ctime(&unload_time));
  vm_output(buf);
  hash_del(&mod_hash, todel->path);
  free(todel->path);
  free(todel);
  return (0);
}



/* List the loaded ELFsh modules */
int		vm_modlist()
{
  elfshmod_t	*actual;
  u_int		index;
  char		*time;
  char		*nl;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output(" .::. ELFsh modules .::. \n");
  index = 1;
  for (actual = world.modlist; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
	*nl = 0x00;
     snprintf(logbuf, BUFSIZ - 1, " [%03u] %s   ID: %u [%s] \n",
	     index, time, actual->id, actual->path);
     vm_output(logbuf);
    }
  if (world.modlist == NULL)
    vm_output(" [*] No loaded module\n");
  return (0);
}




/* In case linked in static */
#if USE_STATIC

void * dlopen(const char *pathname, int mode)
{
  return (NULL);
}

void *dlsym(void *handle, const char *name)
{
  return (NULL);
}

int dlclose(void *handle)
{
  return (1);
}

char *dlerror(void)
{
  return (NULL);
}

#endif 
