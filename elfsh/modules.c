/*
** modules.c for elfsh
**
** Started on  Wed Feb 19 04:42:47 2003 mayhem
** Last update Sat Aug 16 00:21:23 2003 jv
*/
#include "elfsh.h"


/* Guess if the file is an ELFsh module or not */
int		cmd_modload()
{
  elfshmod_t	*new;
  char		buf[BUFSIZ];
  char		*errmsg;
  char		*name;

  /* Verify if the file exists */
  if (access(world.args.param[0], R_OK) != 0)
    {
      snprintf(buf, sizeof(buf), "%s%s", ELFSH_MODPATH, world.args.param[0]);
      if (access(buf, R_OK) != 0)
	{
	  snprintf(buf, sizeof(buf), "%s%s.so",
		   ELFSH_MODPATH, world.args.param[0]);
	  if (access(buf, R_OK) != 0)
	    ELFSH_SETERROR("[elfsh:modload] Cannot find module\n", -1);
	}
      name = buf;
    }
  else
    name = world.args.param[0];

  /* See if the module isnt already loaded */
  new = hash_get(&mod_hash, name);
  if (new != NULL)
    ELFSH_SETERROR("[elfsh:modload] Module already loaded\n", -1);

  /* Load the module and create elfshmod_t entry */
  XALLOC(new, sizeof(elfshmod_t), NULL);
  new->handler = dlopen(name, RTLD_NOW);
  if (new->handler == NULL)
    {
      errmsg = "[elfsh:modload] Cannot load module";
      goto err;
    }

  /* Find constructor and (optional) destructor */
  new->init = dlsym(new->handler, ELFSH_INIT);
  if (new->init == NULL)
    {
      errmsg = "[elfsh:modload] Cannot find init handler";
      goto err;
    }
  new->fini = dlsym(new->handler, ELFSH_FINI);
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
  snprintf(buf, sizeof(buf), "%s: %s\n", errmsg, dlerror());
  ELFSH_SETERROR(buf, -1);
}


/* Unload an elfsh module */
int		cmd_modunload()
{
  elfshmod_t	*cur;
  elfshmod_t	*act;
  elfshmod_t	*todel;
  time_t	unload_time;
  u_int		id;

  /* Check if the object is in the module list */
  id = atoi(world.args.param[0]);
  cur = (id ? vm_getmod(id) : hash_get(&mod_hash, world.args.param[0]));
  if (cur == NULL)
    goto bad;

  /* Call the handlers */
  if (cur->fini != NULL)
    cur->fini();
  dlclose(cur->handler);

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
  printf(" [*] Module %s unloaded on %s \n", todel->path, ctime(&unload_time));
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

  puts(" .::. ELFsh modules .::. ");
  index = 1;
  for (actual = world.modlist; actual != NULL; actual = actual->next, index++)
    {
      time = ctime(&actual->loadtime);
      nl = strchr(time, '\n');
      if (nl)
	*nl = 0x00;
      printf(" [%03u] %s   ID: %u [%s] \n",
	     index, time, actual->id, actual->path);
    }
  if (world.modlist == NULL)
    puts(" [*] No loaded module");
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
