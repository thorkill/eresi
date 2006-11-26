/*
** unload.c for elfsh
** 
** Started on  Sat Jan 25 11:21:52 2003 mayhem
** Last update Thu Mar 11 07:56:36 2004 mayhem
*/
#include "elfsh.h"



/* Unload a file from the shell */
int		cmd_unload()
{
  elfshobj_t	*cur;
  time_t	uloadt;
  char		logbuf[BUFSIZ];
  u_int		id;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

   /* Get the file structure */
  id = atoi(world.curjob->curcmd->param[0]);
  cur = vm_lookup_file(world.curjob->curcmd->param[0]);
  if (cur == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Object not loaded", -1);

  /* Do not unload dependences of files or objects with linkmap entry */
  if (hash_size(&cur->parent_hash))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unload parent object first", -1);
  if (cur->linkmap)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "You cannot unload a mapped object", -1);
  ret = vm_unload_dep(cur, cur);
  if (!world.state.vm_quiet)
    {
      time(&uloadt);
      snprintf(logbuf, BUFSIZ - 1, "%s [*] Object %s unloaded on %s \n",
	       (ret ? "" : "\n"), cur->name, ctime(&uloadt));
      vm_output(logbuf);
    }

  /* Clean various hash tables of this binary entry and return OK */
  hash_del(&file_hash, cur->name);
  if (hash_get(&world.shared_hash, cur->name))
    hash_del(&world.shared_hash, cur->name);
  else
    hash_del(&world.curjob->loaded, cur->name);
  elfsh_unload_obj(cur);
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



