/*
** cmdapi.c for elfsh
** 
** Started on  Mon Feb 24 15:52:06 2003 mayhem
** Last update Mon Feb 24 21:10:19 2003 mayhem
*/
#include "elfsh.h"



/* Create the cmdhandler passed to hash_add() */
elfshcmd_t	*vm_create_CMDENT(int (*exec)(void *file, void *av),
				  int (*reg)(u_int index, u_int argc, char **argv),
				  int flags)
{
  elfshcmd_t	*new;

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

  ent = hash_get_ent(&cmd_hash, cmd);
  if (!ent)
    {
      fprintf(stderr, "\n [!] Unknown command %s \n\n", world.args.param[0]);
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
  hash_add(&cmd_hash, cmd , (void *) vm_create_CMDENT(exec, reg, needfile));
  return (0);
}


/* Delete a command */
int		vm_delcmd(char *cmd)
{
  hash_del(&cmd_hash, cmd);
  return (0);
}
