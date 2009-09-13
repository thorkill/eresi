/**
* @file libstderesi/elf/flush.c
** @ingroup elf
*/
/**
 *
**
** @brief Flush the binary of any injected section 
** Restore the original PLT
** Flush the excedentary BSS zones
**
** Started on  Nov 24 2003 jfv
** 
**
** $Id$
**
*/
#include "libstderesi.h"

/**
 * Need doxygen comment.
 *
 */
int		cmd_flush()
{
  elfshsect_t	*plt;
  elfshsect_t	*origplt;
  elfshsect_t	*act;
  elfshsect_t	*next;
  elfsh_Phdr	*interp;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Restore original PLT if any */
  plt = elfsh_get_plt(world.curjob->curfile, NULL);
  origplt = world.curjob->curfile->secthash[ELFSH_SECTION_ALTPLT];
  if (origplt && plt)
    memcpy(elfsh_readmem(plt), elfsh_readmem(origplt), plt->shdr->sh_size);

  /* Remove pre-interp injected sections */
  interp = elfsh_get_segment_by_type(world.curjob->curfile, PT_INTERP, 0);
  if (!interp)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find PT_INTERP", -1);
  for (act = world.curjob->curfile->sectlist; act; act = next)
    {
      if (act->shdr->sh_addr >= interp->p_vaddr)
	break;
      next = act->next;

      /* Avoid removin the NULL section */
      if (*act->name && 
	  elfsh_remove_section(world.curjob->curfile, act->name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed to remove section", (-1));
    }
  
  /* Remove post-bss injected sections */
  plt = elfsh_get_section_by_name(world.curjob->curfile, 
				  ELFSH_SECTION_NAME_BSS,
				  NULL, NULL, NULL);
  if (!plt)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "PLT section not found", -1);

  act = plt->next;
  while (act && act->shdr->sh_addr)
    {
      next = act->next;
      if (elfsh_remove_section(world.curjob->curfile, act->name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Section removal failed", -1);	
    }

  /* Remove excedentary BSS zones */
  if (elfsh_flush_bss(world.curjob->curfile) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to flush BSS", -1);	

  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Object %s flushed succesfully.\n\n",
	       world.curjob->curfile->name);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
