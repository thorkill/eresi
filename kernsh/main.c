/*
** main.c for kernsh
**
** $Id: main.c,v 1.5 2007-08-06 15:40:39 pouik Exp $
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

int kernsh_config()
{
  char          buff[BUFSIZ];
  char          *home;
  int           ret;
  static int    done = 0;
  revmargv_t    *new;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  ret = -1;
  home = getenv("HOME");
  if (home)
    {
      snprintf(buff, sizeof(buff), "%s/%s", home, KERNSH_CONFIG);
      XALLOC(__FILE__, __FUNCTION__, __LINE__,
             new, sizeof(revmargv_t), -1);
      memset(new, 0, sizeof(revmargv_t));
      world.curjob->curcmd = new;
      world.curjob->curcmd->param[0] = buff;
      ret = cmd_source();
      world.curjob->curcmd = NULL;
      XFREE(__FILE__, __FUNCTION__, __LINE__,new);
    }
  if (ret < 0)
    revm_output("\n [*] No configuration in ~/" KERNSH_CONFIG " \n\n");
  done = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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

  /* Interface tweak */
  kernsh_setup_quit_msg();
  kernsh_setup_prompt();
  
  revm_setup(ac, av, 0, 0);

  revm_config();
  setup_local_cmdhash();

  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    kernsh_banner_print(av[1]);
  
  if (strstr(ARCH, "i"))
    ret = kernsh_init_i386(OS, RELEASE);
  /* Other arch ?? */
  
  kernsh_config();
  return (revm_run(ac, av));
}


/* The main Kernsh routine */
int		main(int ac, char **av)
{
  return (kernsh_main(ac, av));
}



