/*
** relinject.c for elfsh
** 
** Started on  Fri Mar 28 14:58:57 2003 mayhem
** Last update Wed Apr 16 16:24:17 2003 mayhem
*/
#include "elfsh.h"


/* Inject a .o into an executable */
int		cmd_relinject()
{
  elfshobj_t	*host;
  elfshobj_t	*rel;
  int		idx;

  /* Load host file */
  idx = atoi(world.args.param[0]);
  host = (idx ? vm_getfile(idx) : hash_get(&file_hash, world.args.param[0]));
  if (host == NULL)
    {
      host = elfsh_map_obj(world.args.param[0]);
      if (host == NULL)
	ELFSH_SETERROR("[elfsh:cmd_relinject] Cannot map host file\n", -1);
    }

  /* Load relocatable file */
  idx = atoi(world.args.param[1]);
  rel = (idx > 0 ? vm_getfile(idx) : hash_get(&file_hash, world.args.param[1]));
  if (rel == NULL)
    {
      rel = elfsh_map_obj(world.args.param[1]);
      if (rel == NULL)
	ELFSH_SETERROR("[elfsh:cmd_relinject] Cannot map relocatable file\n", 
		       -1);
    }

  /* Call libelfsh relocatable object injector */
  idx = elfsh_inject_etrel(host, rel);
  if (idx < 0)
    return (-1);

  /* Success : put the modified object as current */
  world.current = host;
  if (!world.state.vm_quiet)
    printf(" [*] ET_REL %s injected succesfully in ET_EXEC %s\n\n", 
	   rel->name, host->name);
  return (0);
}
