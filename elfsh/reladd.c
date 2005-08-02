/*
** relinject.c for elfsh
** 
** Started on  Fri Mar 28 14:58:57 2003 mayhem
** Last update Thu Mar 11 14:39:48 2004 mayhem
*/
#include "elfsh.h"


/* Inject a .o into an executable */
int		cmd_relinject()
{
  elfshobj_t	*host;
  elfshobj_t	*rel;
  int		idx;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Load host file */
  idx = atoi(world.curjob->curcmd->param[0]);
  host = (idx ? vm_getfile(idx) : hash_get(&file_hash, world.curjob->curcmd->param[0]));
  if (host == NULL)
    {
      host = elfsh_map_obj(world.curjob->curcmd->param[0]);
      if (host == NULL)
	ELFSH_SETERROR("[elfsh:cmd_relinject] Cannot map host file\n", -1);
    }

  /* Load relocatable file */
  idx = atoi(world.curjob->curcmd->param[1]);
  rel = (idx > 0 ? vm_getfile(idx) : hash_get(&file_hash, world.curjob->curcmd->param[1]));
  if (rel == NULL)
    {
      rel = elfsh_map_obj(world.curjob->curcmd->param[1]);
      if (rel == NULL)
	ELFSH_SETERROR("[elfsh:cmd_relinject] Cannot map relocatable file\n", 
		       -1);
    }

  /* Call libelfsh relocatable object injector */
  idx = elfsh_inject_etrel_withlist(host, rel, world.curjob->list, world.shared);
  if (idx < 0)
    return (-1);

  /* Success : put the modified object as current */
  world.curjob->current = host;
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [*] ET_REL %s injected succesfully in %s %s\n\n", 
	       rel->name, 
	       (host->hdr->e_type == ET_EXEC ? "ET_EXEC" : 
		host->hdr->e_type == ET_DYN  ? "ET_DYN"  : 
		"unknown host file"),
		host->name);
      vm_output(logbuf);
    }
  return (0);
}
