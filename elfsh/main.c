/*
** main.c for elfsh
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Wed Jan 03 17:51:04 2007 mayhem
**
** $Id: main.c,v 1.17 2007-07-07 17:30:23 may Exp $
**
*/
#include "elfsh.h"



/* Prompt related functions for elfsh */
void		esh_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   ELFSH_NAME, REVM_VERSION);
  vm_set_quit_msg(logbuf);
}

void		esh_create_prompt(char *buf, u_int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   vm_colorget("%s", "pspecial", "("),
	   vm_colorget("%s", "psname" , ELFSH_SNAME),
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

void 		esh_setup_prompt()
{
  vm_set_prompt(esh_create_prompt);
}


/* Print the etrace banner */
void		esh_banner_print()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   ELFSH_NAME,
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
int		esh_main(int ac, char **av)
{
  esh_setup_quit_msg();
  esh_setup_prompt();
  vm_setup(ac, av, 0, 0);
  if (world.state.vm_mode != REVM_STATE_CMDLINE)
    esh_banner_print(av[1]);
  vm_config();
  setup_local_cmdhash();
  vm_output(" [*] Type help for regular commands \n\n");
  return (vm_run(ac, av));
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  return (esh_main(ac, av));
}



