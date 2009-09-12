/**
** @file flow.c
** @ingroup analysis
** @brief Various commands related to flow analysis
**
** $Id$
*/
#include "libstderesi.h"


/** 
 * Manually add function pointer information 
 */
int		cmd_addgoto(void)
{
  hash_t	*hash;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!world.curjob->curcmd->param[0] || !world.curjob->curcmd->param[1])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL Parameters", -1);
  
  snprintf(buf, sizeof(buf), " [*] at %s goto %s\n",
	   world.curjob->curcmd->param[0],
	   world.curjob->curcmd->param[1]);
  revm_output(buf);

  hash_del(&world.mjr_session.cur->goto_hash, 
	   world.curjob->curcmd->param[0]);
  hash_add(&world.mjr_session.cur->goto_hash, 
	   world.curjob->curcmd->param[0], world.curjob->curcmd->param[1]);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/**
 * Print the control information 
 */
int		cmd_control()
{
  elfshsect_t	*sect;
  elfshobj_t	*current;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  current = world.curjob->curfile;
  sect = elfsh_get_section_by_name(current, 
				   ELFSH_SECTION_NAME_EDFMT_BLOCKS,
				   0, 0, 0);
  if (!sect || !hash_size(&world.mjr_session.cur->blkhash))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "No control flow section found", -1);

  mjr_blocks_display(world.mjr_session.cur, 1);
  revm_output("\n [*] Control flow information dumped \n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/* Change the graph verbose level */
int	cmd_setgvl(void) 
{
  world.state.revm_gvl = atoi(world.curjob->curcmd->param[0]);
  return (0);
}
