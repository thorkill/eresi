/*
** main.c for evarista
**
** The entry point of the static analyzer
**
** Started on  Fri Jun 22 14:42:36 2007 jfv
** $Id: main.c,v 1.5 2007-07-31 23:30:35 may Exp $
*/
#include "evarista.h"


/* Prompt related functions for evarista */
void		eva_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   EVARISTA_NAME, REVM_VERSION);
  revm_quitmsg_set(logbuf);
}

void		eva_create_prompt(char *buf, unsigned int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , EVARISTA_SNAME),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pversion", REVM_VERSION),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "prelease", REVM_RELEASE),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pedition", REVM_EDITION),
	   revm_colorget("%s", "pspecial", "@"),
	   revm_colorget("%s", "psname", world.curjob->ws.name),
	   revm_colorget("%s", "pspecial", ")"));
  revm_endline();
}

void 		eva_setup_prompt()
{
  revm_set_prompt(eva_create_prompt);
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
#if defined(ERESI32)
	   "32 bits built",
#elif defined(ERESI64)
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
int		eva_main(int ac, char **av)
{
  /* Interface tweak */
  eva_setup_quit_msg();
  eva_setup_prompt();

  revm_setup(ac, av, 0, 0);
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    eva_banner_print(av[1]);
  
  revm_config(EVARISTA_CONFIG);
  setup_local_cmdhash();
  revm_output(" [*] Type help for regular commands \n\n");
  revm_run(ac, av);
  return (0);
}


/* The main ELFsh routine */
int		main(int ac, char **av)
{
  return (eva_main(ac, av));
}



