#include "libe2dbg.h"


/**
 *  Dump the linkmap 
 * @param file
 * @return
 */
int		 e2dbg_linkmap_print(elfshobj_t *file)
{
  char		 logbuf[BUFSIZ];
#if defined(sun)
  Link_map	*actual;
#else
  elfshlinkmap_t *actual;
#endif
  int		 i = 1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", -1);
  if (!file->linkmap || file->linkmap == E2DBG_ABSENT_LINKMAP)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No linkmap", -1);

  e2dbg_output(" .::. Linkmap entries .::. \n");

  for (actual = elfsh_linkmap_get_lprev(file->linkmap); actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [%02u] addr : " XFMT " dyn : " XFMT " - %s\n", i, 
	       (eresi_Addr) elfsh_linkmap_get_laddr(actual), 
	       (eresi_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      e2dbg_output(logbuf);
    }

  for (actual = file->linkmap; actual != NULL; 
       actual = elfsh_linkmap_get_lnext(actual), i++)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " [%02u] addr : "XFMT" dyn : "XFMT" - %s\n", i, 
	       (eresi_Addr) elfsh_linkmap_get_laddr(actual), 
	       (eresi_Addr) elfsh_linkmap_get_lld(actual),
	       elfsh_linkmap_get_lname(actual));
      e2dbg_output(logbuf);
         }

  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Linkmap dump command 
 * @return
*/
int		cmd_linkmap()
{
  int		old;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_is_runtime_mode())
    {
      if (!world.curjob)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "No current workspace", -1);
      
      if (!world.curjob->curfile)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "No current file", -1);
      
      old = world.curjob->curfile->id;

      /* switch to the main program file */
      revm_doswitch(1);
      ret = e2dbg_linkmap_print(world.curjob->curfile);
      revm_doswitch(old);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Debug mode only command", (-1));
}


