/*
** @file main.c
** @ingroup kernsh
** $Id: main.c,v 1.7 2007-10-11 18:25:17 pouik Exp $
**
*/
#include "kernsh.h"
#include "info.h"

#include "libkernsh.h"

void		kernsh_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   KERNSH_NAME, REVM_VERSION);
  revm_quitmsg_set(logbuf);
}

void		kernsh_create_prompt(char *buf, unsigned int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , KERNSH_SNAME),
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

  memset(logbuf, '\0', sizeof(logbuf));
  snprintf(logbuf, 
	   sizeof(logbuf),
	   "\t%s\n \t%s\n \t%s\n \t%s\n \t%s\n \t%s\n\n",
	   revm_colorget("%s", "psname","    __                             ___"),
	   revm_colorget("%s", "psname","   /  \\ /\\ ____ ____  ____  ____  /   /"),
	   revm_colorget("%s", "pversion","   \\   /  /   _\\    \\/    \\/  __\\/   /_ "),
	   revm_colorget("%s", "pversion","   /   \\  \\ ___/  \\_/   \\  \\__   \\  \\  \\"),
	   revm_colorget("%s", "pedition","   \\__\\   /___ \\__/ /___/  /\\__  /__/  /"),
	   revm_colorget("%s", "pedition","        \\/    \\/         \\/    \\/    \\/"));
  revm_output(logbuf);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* The real main function */
int		kernsh_main(int ac, char **av)
{
  int		ret;
  char		logbuf[BUFSIZ];

  memset(logbuf, '\0', sizeof(logbuf));

  /* Interface tweak */
  kernsh_setup_quit_msg();
  kernsh_setup_prompt();
  
  revm_setup(ac, av, 0, 0);

  setup_local_cmdhash();
  
  if (strstr(ARCH, "i"))
    {
      ret = kernsh_init_ia32(OS, RELEASE);
      if (ret == -1)
	{
	  snprintf(logbuf, 
		   sizeof(logbuf),
		   "Sorry, your os is not yet supported");
	  revm_output(logbuf);
	  exit(-1);
	}
    }
  /* Other arch ?? */
  else
    {
      snprintf(logbuf, 
	       sizeof(logbuf),
	       "Sorry, your architecture is not yet supported");
      revm_output(logbuf);
      exit(-1);
    }

  revm_config(KERNSH_CONFIG);

  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    kernsh_banner_print(av[1]);

  revm_run(ac, av);
  return (0);
}


/* The main Kernsh routine */
int		main(int ac, char **av)
{
  return (kernsh_main(ac, av));
}



