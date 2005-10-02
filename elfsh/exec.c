/*
** exec.c for elfsh 
** 
** Started on  Tue Feb 18 13:03:14 2003 mayhem
*/
#include "elfsh.h"

/* Fork and execve the stuff */
int		cmd_exec()
{
  int       status;
  char      buf[BUFSIZ] = "";
  int       i = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);


#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_deprep_terminal();
#endif

  while (world.curjob->curcmd->param[i])
  {
      strncat (buf, " ", BUFSIZ); 
      strncat (buf, world.curjob->curcmd->param[i], BUFSIZ);
      i++;
  }
  
  switch (status = vm_system (buf))
  {
      case 0:
          vm_output("\n [*] Command executed successfully \n\n");
          break;
      default:
          vm_output("\n [E] Command failed\n\n");
  }

#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER ||
      world.state.vm_mode == ELFSH_VMSTATE_IMODE)
    rl_prep_terminal(1);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     status);
}
