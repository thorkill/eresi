/*
** load.c for elfsh
** 
** Started on  Sat Jan 25 11:21:18 2003 mayhem
** Last update Mon Apr 21 00:16:26 2003 mayhem
*/
#include "elfsh.h"




/* Insert an object in the list of opened elfsh descriptors */
int		cmd_load()
{
  elfshobj_t	*new;

  /* Map the standard ELF object */
  new = elfsh_map_obj(world.args.param[0]);
  if (NULL == new)
    {
      fprintf(stderr, "\n [!] Cannot load object ");
      PERROR_RET(world.args.param[0], -1);
    }
  
  /* Print a msg if not in quiet mode */
  new->loadtime = time(&new->loadtime);
  if (!world.state.vm_quiet)
    printf("\n [*] New object %s loaded on %s \n\n", 
	   world.args.param[0], ctime(&new->loadtime));

  /* Add the object to the list of opened objects */
  new->id = ++world.state.lastid;
  world.current = new;
  new->next = world.list;
  world.list = new;

  /* Add an entry into the loaded files hashtable */
  hash_add(&file_hash, new->name, (void *) new);
  return (0);
}


