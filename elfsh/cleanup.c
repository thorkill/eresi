/*
** cleanup.c for elfsh
**
** This command cleanup a file by marking as removed all injected sections
**
** Started on  Sat May 31 23:48:41 2005 mayhem
**
** $Id: cleanup.c,v 1.4 2007-07-17 18:11:24 may Exp $
**
*/
#include "elfsh.h"




int		cmd_cleanup()
{
  elfshobj_t	*obj;
  elfshsect_t	*sect;
  int		index;
  uint32_t	nbr;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  obj = revm_lookup_file(world.curjob->curcmd->param[0]);
  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);

  sect = world.curjob->current->sectlist;
  nbr = 0;
  for (index = 0; index < obj->hdr->e_shnum; index++, sect = sect->next)
    if (strstr(sect->name, ".o.") || strstr(sect->name, "elfsh"))
      {
	snprintf(buf, BUFSIZ, "Set section header %u as removed \n", index);
	revm_output(buf);
	sect->flags = sect->flags | ELFSH_SECTION_REMOVED;
	nbr++;
      }

  world.curjob->current->nbrm = nbr;
  //world.curjob->current->strip = 1;

  revm_output("\n [*] File marked for cleanup-on-save \n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
