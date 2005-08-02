/*
** cleanup.c for elfsh
**
** This command cleanup a file by marking as removed all injected sections
**
** Started on  Sat May 31 23:48:41 2005 mayhem
*/
#include "elfsh.h"




int		cmd_cleanup()
{
  elfshobj_t	*obj;
  elfshsect_t	*sect;
  int		index;
  uint32_t	nbr;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  obj = vm_lookup_file(world.curjob->curcmd->param[0]);
  if (!obj)
    ELFSH_SETERROR("[vm:cmd_cleanup] Invalid argument", -1);

  sect = world.curjob->current->sectlist;
  nbr = 0;
  for (index = 0; index < obj->hdr->e_shnum; index++, sect = sect->next)
    if (strstr(sect->name, ".o.") || strstr(sect->name, "elfsh"))
      {
	printf("Set section header %u as removed \n", index);
	sect->flags = sect->flags | ELFSH_SECTION_REMOVED;
	nbr++;
      }

  world.curjob->current->nbrm = nbr;
  //world.curjob->current->strip = 1;

  vm_output("\n [*] File marked for cleanup-on-save \n\n");
  return (0);
}
