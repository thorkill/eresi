/*
** unload.c for elfsh
** 
** Started on  Sat Jan 25 11:21:52 2003 mayhem
** Last update Thu Jun  5 16:13:42 2003 mayhem
*/
#include "elfsh.h"



/* Unload a file from the shell */
int		cmd_unload()
{
  elfshobj_t	*cur;
  elfshobj_t	*act;
  elfshobj_t	*todel;
  time_t	uloadt;
  u_int		id;

  /* Get the file structure */
  id = atoi(world.args.param[0]);
  cur = (id ? vm_getfile(id) : hash_get(&file_hash, world.args.param[0]));
  if (cur == NULL)
    goto bad;

  /* If the element is the first of the list, update the head pointer ... */
  if (world.list->id == cur->id)
    {
      if (world.current == world.list)
	world.current = world.list->next;
      todel = world.list;
      world.list = world.list->next;
      goto end;
    }

  /* ... else find the object where it is in the list */
  for (act = world.list; act && act->next; act = act->next)
    if (act->next->id == cur->id)
      {
	if (world.current == act->next)
	  world.current = act;
	todel = act->next;
	act->next = act->next->next;
	goto end;
      }

  /* Error msg */
 bad:
  ELFSH_SETERROR("[elfsh:cmd_unload] Object not loaded \n", -1);
  
  /* OK msg */
 end:
  time(&uloadt);
  if (!world.state.vm_quiet)
    printf("\n [*] Object %s unloaded on %s \n\n", todel->name, ctime(&uloadt));
  hash_del(&file_hash, todel->name);
  elfsh_unload_obj(todel);
  return (0);
}



