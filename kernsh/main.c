/*
** main.c for kernsh
**
** $Id: main.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "kernsh.h"
#include "info.h"

#include "libkernsh.h"

void		kernsh_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   KERNSH_NAME, KERNSH_VERSION);
  revm_quitmsg_set(logbuf);
}

void		kernsh_create_prompt(char *buf, u_int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , KERNSH_SNAME),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pversion", KERNSH_VERSION),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "prelease", KERNSH_RELEASE),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pedition", KERNSH_EDITION),
	   revm_colorget("%s", "pspecial", "@"),
	   revm_colorget("%s", "psname", world.curjob->ws.name),
	   revm_colorget("%s", "pspecial", ")"));
  revm_endline();
}

void 		kernsh_setup_prompt()
{
  revm_set_prompt(kernsh_create_prompt);
}

/* Print the kernsh banner */
void		kernsh_banner_print()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   KERNSH_NAME,
	   KERNSH_VERSION,
#if defined(ELFSH32)
	   "32 bits built",
#elif defined(ELFSH64)
	   "64 bits built",
#else
	   "Unknown built",
#endif
	   "\t .::. This software is under the General Public License V.2 \n"
	   "\t .::. Please visit http://www.gnu.org \n\n");
  revm_output(logbuf);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* The real main function */
int		kernsh_main(int ac, char **av)
{
  /* Interface tweak */
  kernsh_setup_quit_msg();
  kernsh_setup_prompt();
  
  revm_setup(ac, av, 0, 0);

  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    kernsh_banner_print(av[1]);

  revm_config();
  setup_local_cmdhash();
  
  if (strstr(ARCH, "i"))
    kernsh_init_i386(OS, RELEASE);
  
  kernsh_config();
  return (revm_run(ac, av));
}


/* The main Kernsh routine */
int		main(int ac, char **av)
{
  return (kernsh_main(ac, av));
}



