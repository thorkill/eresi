/*                                                                                                                                                          
** linkmap.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "libe2dbg.h"


/* Dump the linkmap */
int		 e2dbg_linkmap(elfshobj_t *file)
{
  char		 logbuf[BUFSIZ];
  elfshlinkmap_t *actual;
  int		 i = 1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);

  if (!file->linkmap)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No linkmap", -1);

  e2dbg_output(" .::. Linkmap entries .::. \n");

  for (actual = elfsh_linkmap_get_lprev(file->linkmap); actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [%02u] addr : " XFMT " dyn : " XFMT " - %s\n", i, 
	       (elfsh_Addr) elfsh_linkmap_get_laddr(actual), 
	       (elfsh_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      e2dbg_output(logbuf);
    }

  for (actual = file->linkmap; actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [%02u] addr : "XFMT" dyn : "XFMT" - %s\n", i, 
	       (elfsh_Addr) elfsh_linkmap_get_laddr(actual), 
	       (elfsh_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      e2dbg_output(logbuf);
    }

  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Linkmap dump command */
int		cmd_linkmap()
{
  int		old;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_is_debug_mode())
    {
      if (!world.curjob)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No current workspace", -1);
      
      if (!world.curjob->current)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No current file", -1);
      
      old = world.curjob->current->id;

      /* switch to the main program file */
      vm_doswitch(1);
      ret = e2dbg_linkmap(world.curjob->current);
      vm_doswitch(old);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Debug mode only command", (-1));
}





