/*
** cmdline.c for elfsh
** 
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Last update Wed Jun 25 08:51:39 2003 mayhem
*/
#include "elfsh.h"


/* Bad parameter handler */
void	vm_badparam(char *str)
{
  fprintf(stderr, 
	  "\n [!] Bad parameters for command %s .::. "
	  "type 'help' for command list \n\n", str);
}

/* Unknow command handler */
void	vm_unknown(char *str)
{
  fprintf(stderr, "\n [!] Unknown command %s .::. "
	  "type 'help' for command list \n\n", str);
}

/* Open the script file */
int	vm_openscript(char *name, char *av0)
{
  int	fd;

  /* Open script file */
  XOPEN(fd, name, O_RDONLY, 0, -1);
  if (dup2(fd, 0) < 0)
    return (-1);
  return (0);
}

/* Print the banner */
void		vm_print_banner()
{
  printf("\n\n\t Welcome to The ELF shell %s .::. \n\n", ELFSH_VERSION);
  printf("\t .::. This software is under the General Public License \n"
	 "\t .::. Please visit http://www.gnu.org to know about "
	 "Free Software \n\n\n");
}

/* Print the Unknown buffer */
char		*vm_build_unknown(char *buf, const char *str, u_long type)
{
  snprintf(buf, ELFSH_MEANING, "%s %08X", str, (u_int) type);
  return (buf);
}


/* Retreive a file object giving its unique ID */
elfshobj_t	*vm_getfile(u_int index)
{
  elfshobj_t	*cur;

  for (cur = world.list; cur; cur = cur->next)
    if (cur->id == index)
      return (cur);
  return (NULL);
}

/* Retreive a module object giving its unique ID */
elfshmod_t	*vm_getmod(u_int index)
{
  elfshmod_t	*cur;

  for (cur = world.modlist; cur; cur = cur->next)
    if (cur->id == index)
      return (cur);
  return (NULL);
}

/* Print error depending on the state of the machine */
int		vm_doerror(void (*fct)(char *str), char *str)
{
  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
    fct(str);
  else
    {
      cmd_help();
      exit(-1);
    }
  return (-1);
}

/* Mark the current object SHT to be removed on savnig */
int             cmd_shtrm()
{
  world.current->shtrm = 1;
  if (!world.state.vm_quiet)
    puts(" [*] SHT marked as removed, you need to save\n");
  return (0);
}

/* Mark the file as stripped */
int		cmd_strip()
{
  world.current->strip = 1;
  if (!world.state.vm_quiet)
    puts(" [*] Object marked as stripped, you need to save\n");
  return (0);
}

/* Mark SHT as removed and file as stripped */
int		cmd_sstrip()
{
  world.current->strip = 1;
  world.current->shtrm = 1;
  if (!world.state.vm_quiet)
    puts(" [*] Object marked as sstripped, you need to save\n");
  return (0);
}

/* Change the VM state as QUIET */
int		cmd_quiet()
{
  world.state.vm_quiet = 1;
  return (0);
}

/* Change the VM state as VERBOSE (default) */
int		cmd_verb()
{
  printf(" [*] Switched to verbose mode \n\n");
  world.state.vm_quiet = 0;
  return (0);
}

/* Useful when you have only one terminal */
int	cmd_meta()
{
  if (!world.state.vm_quiet)
    printf(" [*] You are still in elfsh, exit bash "
	   "to get back to the regular prompt \n\n");
  return (system(ELFSH_SHELL));
}

/* Useful for debugging */
int	cmd_stop()
{
  return (raise(SIGSTOP));
}

/* Useful to differentiate 0 and a string */
int	vm_isnbr(char *string)
{
  size_t len = strlen(string);
  size_t ii;

  for (ii=0; ii < len; ii++)
    if (!isdigit((int) string[ii]))
      return (0);
  return (1);
}



