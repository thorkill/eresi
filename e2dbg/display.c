/*                                                                                                                                                          
** display.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"





/* Print all display commands */
void		vm_print_display()
{
  char		buff[BUFSIZ];
  int		idx;
  int		printed;

  hashent_t     *actual;
  elfshbp_t     *curbp;
  int		idx2;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Global displays */
  printed = 0;
  for (idx = 0; idx < E2DBG_STEPCMD_MAX; idx++)
    {
      if (!e2dbgworld.displaycmd[idx])
	continue;
      if (!printed)
	{
	  vm_output("\n\t .:: e2dbg global displayed commands \n");
	  printed = 1;
	}
      snprintf(buff, BUFSIZ, "\t %u: %s \n", 
	       idx, e2dbgworld.displaycmd[idx]);
      vm_output(buff);
    }
  if (!printed)
    vm_output("\n\t .:: e2dbg currently has no global displays \n\n");
  
  /* Display breakpoint commands */
  printed = 0;
  for (idx = 0; idx < e2dbgworld.bp.size; idx++)
    for (actual = e2dbgworld.bp.ent + idx;
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	curbp = (elfshbp_t *) actual->data;
	for (idx2 = 0; idx2 < 10; idx2++)
	  if (curbp->cmd[idx2])
	    {
	      if (!printed)
		{
		  snprintf(buff, BUFSIZ, 
			   "\n\t .:: e2dbg display for breakpoint %u [" XFMT "] %s \n",
			   curbp->id, curbp->addr, curbp->symname);
		  vm_output(buff);
		  printed = 1;
		}
	      snprintf(buff, BUFSIZ, "\t %u: %s \n",
		       idx2, curbp->cmd[idx2]);
	      vm_output(buff);
	    }
	printed = 0;
      }

  vm_output("\n");
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Execute all displayed commands for a breakpoint or a step */
int		vm_display(char **cmd, u_int nbr)
{
  int		idx;
  elfshargv_t	*cur;
  char		*str;
  char		buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  for (idx = 0; idx < nbr; idx++)
    if (cmd[idx])
      {
	
	snprintf(buf, BUFSIZ, 
		 "\t .:: Display %u [%s] result ::. \n", 
		 idx, cmd[idx]);
	vm_output(buf);
	
	/* Register displayed command in the script control flow */
	str = elfsh_strdup(cmd[idx]);
	if (vm_exec_str(cmd[idx]) < 0)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Display execrequest failed", -1);
	cmd[idx] = str;
	
	/* Execute displayed command */
	cur = world.curjob->curcmd;
	world.curjob->curcmd = world.curjob->script[world.curjob->sourced]; // instead of script 0
	if (vm_execmd() < 0)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "Display execution failed", -1);
	world.curjob->curcmd = cur;
      }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Display on breakpoint or step */
int		cmd_display()
{
  elfshbp_t	*bp;
  char		buf[BUFSIZ];
  char		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* No parameter, list displays */
  if (!world.curjob->curcmd->param[0])
    {
      vm_print_display();
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  





  }
  
  /* Add a global display */
  if (!vm_isnbr(world.curjob->curcmd->param[0]))
    {
      if (e2dbgworld.displaynbr >= E2DBG_STEPCMD_MAX)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Too many global displays", NULL);
      str = vm_get_string(world.curjob->curcmd->param);
      snprintf(buf, BUFSIZ, "\n [*] Adding global display %u [%s] \n\n", 
	       e2dbgworld.displaynbr, str);
      vm_output(buf);
      e2dbgworld.displaycmd[e2dbgworld.displaynbr++] = str;
    }
  
  /* Add a local display on breakpoint */
  else
    {
     
      bp = vm_lookup_bp(world.curjob->curcmd->param[0]);
      if (!bp)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot find specified breakpoint", NULL);
      
      if (bp->cmdnbr >= 10)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Too many local displays", NULL);
      
      str = vm_get_string(&world.curjob->curcmd->param[1]);
      snprintf(buf, BUFSIZ, "\n [*] Adding display %u [%s] for breakpoint %u "
	       "[" XFMT "] %s \n\n", bp->cmdnbr, str, 
	       bp->id, bp->addr, bp->symname);
               
      vm_output(buf);
      bp->cmd[(int) bp->cmdnbr++] = str;
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Delete displays */
int		cmd_undisplay()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);



  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
