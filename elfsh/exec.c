/*
** exec.c for elfsh 
** 
** Started on  Tue Feb 18 13:03:14 2003 mayhem
** Last update Sun Apr  6 00:43:52 2003 mayhem
*/
#include "elfsh.h"

/* Fork and execve the stuff */
int		cmd_exec()
{
  pid_t		pid;
  int		status;

  fflush(stdout);
  pid = fork();
  if (!pid)
    exit(execvp(world.args.param[0], world.args.param));
  waitpid(pid, &status, 0);
  switch (status)
    {
    case 0:
      puts("\n [*] Command executed successfully \n");
      break;
    default:
      perror("\n [*] Command failed");
      puts("");
    }
  return (status);
}
