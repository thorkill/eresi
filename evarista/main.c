/*
** main.c for evarista
**
** The entry point of the static analyzer
**
** Started on  Fri Jun 22 14:42:36 2007 mayhem
** $Id: main.c,v 1.2 2007-07-07 17:30:24 may Exp $
*/
#include "evarista.h"


/* Prompt related functions for evarista */
void		eva_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   EVARISTA_NAME, REVM_VERSION);
  vm_set_quit_msg(logbuf);
}

void		eva_create_prompt(char *buf, u_int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   vm_colorget("%s", "pspecial", "("),
	   vm_colorget("%s", "psname" , EVARISTA_SNAME),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "pversion", REVM_VERSION),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "prelease", REVM_RELEASE),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "pedition", REVM_EDITION),
	   vm_colorget("%s", "pspecial", "@"),
	   vm_colorget("%s", "psname", world.curjob->ws.name),
	   vm_colorget("%s", "pspecial", ")"));
  vm_endline();
}

void 		eva_setup_prompt()
{
  vm_set_prompt(eva_create_prompt);
}


/* Print the etrace banner */
void		eva_banner_print()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   EVARISTA_NAME,
	   REVM_VERSION,
#if defined(ELFSH32)
	   "32 bits built",
#elif defined(ELFSH64)
	   "64 bits built",
#else
	   "Unknown built",
#endif
	   "\t .::. This software is under the General Public License V.2 \n"
	   "\t .::. Please visit http://www.gnu.org \n\n");
  vm_output(logbuf);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* The real main function */
int		eva_main(int ac, char **av)
{
  /* Interface tweak */
  eva_setup_quit_msg();
  eva_setup_prompt();

  vm_setup(ac, av, 0, 0);
  if (world.state.vm_mode != REVM_STATE_CMDLINE)
    eva_banner_print(av[1]);
  
  vm_config();
  setup_local_cmdhash();
  vm_output(" [*] Type help for regular commands \n\n");
  return (vm_run(ac, av));
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  return (eva_main(ac, av));
}



