/*
** @file hammer.c
**
** @brief Contains libmjollnir commands bindings
**
** Last update December 10 2006 may
**
** $Id: hammer.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"


/* To rebuild symtab use this command */
int	cmd_unstrip() 
{
  char logbuf[BUFSIZ];
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure started\n");
  revm_output(logbuf);
  if (!world.mjr_session.cur->analysed)
    cmd_analyse();
  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure %s\n",
	   (mjr_symtab_rebuild(&world.mjr_session)) ? "completed successfully" : "faild");
  revm_output(logbuf);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* This command fill the blocks hash */
int		cmd_analyse() 
{
 char		logbuf[BUFSIZ];
 int		ret;
 char		**keys;
 int		index;
 int		nbr;
 container_t	*container;
 aspectype_t	*curtype;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 /* Analyzing object */
 memset(logbuf, 0x0, BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, " [*] Now performing Control Flow Analysis\n");
 revm_output(logbuf);
 memset(logbuf, 0x0, BUFSIZ);
 ret = mjr_analyse(&world.mjr_session, 0);

 snprintf(logbuf, BUFSIZ - 1, " [*] Control Flow Analysis %s.\n",
	  (!ret ? "completed successfully" : "failed"));
 revm_output(logbuf);
 if (ret) 
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		"Error during control flow analysis", -1);

 memset(logbuf, 0x0, BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, 
	  " [*] Calls seen: %d, Recognized destination: %d\n",
	  world.mjr_session.cur->calls_seen,
	  world.mjr_session.cur->calls_found);
 revm_output(logbuf);

 /* Now informing about all existing blocks and functions */
 curtype = aspect_type_get_by_name("container_t");
 if (curtype)
   {
     keys = hash_get_keys(&world.mjr_session.cur->funchash, &nbr);
     for (index = 1; index < nbr; index++)
       {
	 container = hash_get(&world.mjr_session.cur->funchash, keys[index]);
	 snprintf(logbuf, sizeof(logbuf), "func_%08X", *(elfsh_Addr *) container->data);
	 revm_inform_type_addr(curtype->name, logbuf, (elfsh_Addr) container, NULL, 0, 1);
       }
     hash_free_keys(keys);
     keys = hash_get_keys(&world.mjr_session.cur->blkhash, &nbr);
     for (index = 1; index < nbr; index++)
       {
	 container = hash_get(&world.mjr_session.cur->blkhash, keys[index]);
	 snprintf(logbuf, sizeof(logbuf), "bloc_%08X", *(elfsh_Addr *) container->data);
	 revm_inform_type_addr(curtype->name, logbuf, (elfsh_Addr) container, NULL, 0, 1);
       }
     hash_free_keys(keys);
   }
 else
   revm_output(" [*] No reflection was performed (no container type definition found)\n\n");

 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Shortcut command to rename symbols */
int	cmd_rename() 
{
 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 mjr_symbol_rename(&world.mjr_session,
		   world.curjob->curcmd->param[0],
		   world.curjob->curcmd->param[1]);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
