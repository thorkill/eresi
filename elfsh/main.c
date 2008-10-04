/**
 * @file main.c
 * @ingroup elfsh
 * Started on  Wed Feb 21 22:02:36 2001 jfv
 * Updated on  Wed Jan 03 17:51:04 2007 jfv
 *
 * $Id: main.c,v 1.21 2008-02-16 12:31:34 thor Exp $
 *
 */
#include "elfsh.h"



/**
 * Prompt related functions for elfsh 
 */
void		esh_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   ELFSH_NAME, REVM_VERSION);
  revm_quitmsg_set(logbuf);
}

/**
 * FIXME
 * @param buf
 * @param size
 */
void		esh_create_prompt(char *buf, unsigned int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , ELFSH_SNAME),
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

/**
 * FIXME
 */
void 		esh_setup_prompt()
{
  revm_set_prompt(esh_create_prompt);
}


/**
 * Print the etrace banner 
 */
void		esh_banner_print()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   ELFSH_NAME,
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

/** 
 * The real main function
 * @param ac
 * @param av
 * @return
 */
int		esh_main(int ac, char **av)
{
  esh_setup_quit_msg();
  esh_setup_prompt();
  revm_setup(ac, av, 0, 0);
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    esh_banner_print(av[1]);
  revm_config(ELFSH_CONFIG);
  setup_local_cmdhash();
  revm_output(" [*] Type help for regular commands \n\n");
  revm_run(ac, av);
  return (0);
}


/** 
 * The main ELFsh routine 
 * @param ac
 * @param av
 * @return
 */
int		main(int ac, char **av)
{
  return (esh_main(ac, av));
}



