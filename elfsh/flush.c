/*
** flush.c for elfsh
**
** Flush the binary of any injected section 
** Restore the original PLT
** Flush the excedentary BSS zones
**
** Started on  Nov 24 2003 mayhem

*/
#include "elfsh.h"



int		cmd_flush()
{
  elfshsect_t	*plt;
  elfshsect_t	*origplt;
  elfshsect_t	*act;
  elfshsect_t	*next;
  elfsh_Phdr	*interp;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Restore original PLT if any */
  plt = elfsh_get_plt(world.curjob->current, NULL);
  origplt = world.curjob->current->secthash[ELFSH_SECTION_ALTPLT];
  if (origplt && plt)
    memcpy(elfsh_get_raw(plt), elfsh_get_raw(origplt), plt->shdr->sh_size);

  /* Remove pre-interp injected sections */
  interp = elfsh_get_segment_by_type(world.curjob->current, PT_INTERP, 0);
  if (!interp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find PT_INTERP", -1);
  for (act = world.curjob->current->sectlist; act; act = next)
    {
      if (act->shdr->sh_addr >= interp->p_vaddr)
	break;
      next = act->next;

      /* Avoid removin the NULL section */
      if (*act->name && 
	  elfsh_remove_section(world.curjob->current, act->name) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed to remove section", (-1));
    }
  
  /* Remove post-bss injected sections */
  plt = elfsh_get_section_by_name(world.curjob->current, 
				  ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (!plt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "PLT section not found", -1);

  act = plt->next;
  while (act && act->shdr->sh_addr)
    {
      next = act->next;
      if (elfsh_remove_section(world.curjob->current, act->name) < 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Section removal failed", -1);	
    }

  /* Remove excedentary BSS zones */
  if (elfsh_flush_bss(world.curjob->current) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to flush BSS", -1);	

  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Object %s flushed succesfully.\n\n",
	       world.curjob->current->name);
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
