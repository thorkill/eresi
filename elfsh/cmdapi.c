/*
** cmdapi.c for elfsh
** 
** Started on  Mon Feb 24 15:52:06 2003 mayhem
** Last update Thu Mar 11 14:39:06 2004 mayhem
*/
#include "elfsh.h"



/* Create the cmdhandler passed to hash_add() */
elfshcmd_t	*vm_create_CMDENT(int (*exec)(void *file, void *av),
				  int (*reg)(u_int index, u_int argc, char **argv),
				  int flags)
{
  elfshcmd_t	*new;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(new, sizeof(elfshcmd_t), NULL);
  new->exec   = exec;
  new->reg    = reg;
  new->wflags = flags;
  return (new);
}


/* Change the handler for an ELFsh command */
int		vm_setcmd(char *cmd, void *exec, void *reg, u_int needcur)
{
  hashent_t	*ent;
  elfshcmd_t	*act;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ent = hash_get_ent(&cmd_hash, cmd);
  if (!ent)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [!] Unknown command %s \n\n", world.curjob->curcmd->param[0]);
      vm_output(logbuf);
      return (-1);
    }
  act = ent->data;
  if (reg != ELFSH_ORIG)
    act->reg = reg;
  if (exec != ELFSH_ORIG)
    act->exec = exec;
  if (needcur != (u_int) ELFSH_ORIG)
    act->wflags = needcur;
  return (0);
}

/* Add a command */
int		vm_addcmd(char *cmd, void *exec, void *reg, u_int needfile)
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_add(&cmd_hash, cmd , (void *) vm_create_CMDENT(exec, reg, needfile));
  return (0);
}


/* Delete a command */
int		vm_delcmd(char *cmd)
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_del(&cmd_hash, cmd);
  return (0);
}
