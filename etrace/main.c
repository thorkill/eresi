/*
** main.c for etrace
**
** Updated on  Wed Jan 03 17:51:04 2007 mxatone
**
** $Id: main.c,v 1.6 2007-03-27 20:56:03 mxatone Exp $
**
*/
#include "etrace.h"

void		vm_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   ETRACE_NAME, ETRACE_VERSION);
  vm_set_quit_msg(logbuf);
}

void		vm_create_prompt(char *buf, u_int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   vm_colorget("%s", "pspecial", "("),
	   vm_colorget("%s", "psname" , ETRACE_SNAME),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "pversion", ETRACE_VERSION),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "prelease", ETRACE_RELEASE),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "pedition", ETRACE_EDITION),
	   vm_colorget("%s", "pspecial", "@"),
	   vm_colorget("%s", "psname", world.curjob->ws.name),
	   vm_colorget("%s", "pspecial", ")"));
  vm_endline();
}

void 		vm_setup_prompt()
{
  vm_set_prompt(vm_create_prompt);
}

/* Print the etrace banner */
void		vm_print_etrace_banner()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   ETRACE_NAME,
	   ETRACE_VERSION,
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
int		vm_main(int ac, char **av)
{
  int		ret;
  char		**argv;
  u_int		argc;
  char		logbuf[BUFSIZ];
  char		*str;

  if (ac < 3)
    {
      vm_output("Invalid arguments number\n");
      snprintf(logbuf, BUFSIZ - 1, 
	       "Usage: %s <file> -t <func_name_1> <func_name_2> .. \n",
	       av[0]);
      vm_output(logbuf);
      return -1;
    }

  /* Interface tweak */
  vm_setup_quit_msg();
  vm_setup_prompt();
  vm_setup(ac, av, REVM_STATE_TRACER, 0);

  vm_print_etrace_banner();

  /* Parse first argument */
  if (vm_trans_speblank(av[1], &argv, &argc) < 0)
    return -1;

  world.state.input = argv[0];
  str = vm_lookup_string(world.state.input);

  if (!str)
    {
      vm_output("Unable to lookup file\n");
      return -1;
    }

  /* Load submited file */
  vm_output("\n");
  ret = vm_load_file(str, 0, NULL);
  vm_output("\n");

  if (ret < 0)
    return ret;

  if (argc > 1)
    vm_traces_add_arguments(argc - 1, argv+1);
  
  XFREE(__FILE__, __FUNCTION__, __LINE__, argv);
  
  /* Update pointer */
  ac--;
  av[1] = av[0];
  av++;

  vm_config();
  setup_local_cmdhash();
  return (vm_run(ac, av));
}


/* The main Etrace routine */
int		main(int ac, char **av)
{
  return (vm_main(ac, av));
}



