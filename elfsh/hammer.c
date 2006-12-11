/*
** hammer.c for elfsh
**
** Contains libmjollnir commands bindings
**
** Last update December 10 2006 may
*/
#include "elfsh.h"


/* To rebuild symtab use this command */
int	cmd_unstrip() 
{
  char logbuf[BUFSIZ];
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure started\n");
  vm_output(logbuf);
  if (!world.mjr_session.cur->analysed_calls)
    cmd_analyse();
  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure %s\n",
	   (mjr_symtab_rebuild(&world.mjr_session)) ? "completed successfully" : "faild");
  vm_output(logbuf);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* This command fill the blocks hash */
int	cmd_analyse() 
{
 char	logbuf[BUFSIZ];
 int	ret = 0;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 memset(logbuf,0x0,BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : performing object analysis\n");
 vm_output(logbuf);
 memset(logbuf,0x0,BUFSIZ);
 ret = mjr_analyse(&world.mjr_session, NULL);
 snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : object analysis %s.\n",
	  (!ret ? "completed successfully" : "failed"));
 vm_output(logbuf);
 if (ret) 
   {
     memset(logbuf,0x0,BUFSIZ);
     snprintf(logbuf, BUFSIZ - 1, 
	      " .: mjollnir : calls seen: %d, recognized destination: %d\n",
	      world.mjr_session.cur->st_calls_seen,
	      world.mjr_session.cur->st_calls_found);
     vm_output(logbuf);
   }
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Shortcut command to rename symbols */
int	cmd_rename() 
{
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 mjr_symbol_rename(&world.mjr_session,
		   world.curjob->curcmd->param[0],
		   world.curjob->curcmd->param[1]);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
