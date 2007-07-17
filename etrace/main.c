/*
** main.c for etrace
**
** Updated on  Wed Jan 03 17:51:04 2007 mxatone
**
** $Id: main.c,v 1.11 2007-07-17 18:11:24 may Exp $
**
*/
#include "etrace.h"

/* Prompt related functions */
void		etrace_setup_quit_msg()
{
  char		logbuf[BUFSIZ];

  snprintf(logbuf, BUFSIZ - 1, "\t .:: Bye -:: The %s %s \n",
	   ETRACE_NAME, REVM_VERSION);
  revm_quitmsg_set(logbuf);
}

void		etrace_create_prompt(char *buf, u_int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , ETRACE_SNAME),
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

void 		etrace_setup_prompt()
{
  revm_set_prompt(etrace_create_prompt);
}

/* Print the etrace banner */
void		etrace_banner_print()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   ETRACE_NAME,
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
  revm_output(logbuf);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* The real main function */
int		etrace_main(int ac, char **av)
{
  char		logbuf[BUFSIZ];
  int		ret;
  char		**argv;
  u_int		argc;
  char		*str;
  u_int		state;
  u_char	trace_all = 0;
  char		**exav = NULL;
  int		exac;
  u_int		index;

  /* Interface tweak */
  etrace_setup_quit_msg();
  etrace_setup_prompt();
  
  /* Which state ? */
  state =  REVM_STATE_INTERACTIVE;

  /* If we found the trace command we toggle to tracer state */
  if (ac > 2)
    {
      snprintf(logbuf, BUFSIZ - 1, "-%s", CMD_TRACEADD_CMDLINE);
      for (index = 2; index < ac; index++)
	{
	  if (!strcmp(av[index], logbuf))
	    {
	      state = REVM_STATE_TRACER;
	      break;
	    }
	}
    }

  revm_setup(ac, av, state, 0);

  /* In this case we will trace all function of the binary */
  if (ac > 1 && state == REVM_STATE_INTERACTIVE
      && world.state.revm_mode != REVM_STATE_SCRIPT)
    {
      trace_all = 1;
      exav = av;
      
      /* Switch to tracer mode */
      world.state.revm_mode = state = REVM_STATE_TRACER;
      exac = ac;
      ac += 2;

      /* Allocated new argument list */
      if ((av = (void*) calloc(sizeof(char*)*(ac+1), 1)) == NULL)
	{
	  write(1, "Out of memory\n", 14);
	  exit(1);
	}

      memcpy(av, exav, sizeof(char*)*exac);

      /* As we done a etrace <file> -t .* */
      av[exac] = "-t";
      av[exac+1] = ".*";
    }

  etrace_banner_print();

  /* Tracer state parsing */
  if (state == REVM_STATE_TRACER)
    {
      /* Parse first argument */
      if (revm_trans_speblank(av[1], &argv, &argc) < 0)
	return -1;

      world.state.input = argv[0];
      str = revm_lookup_string(world.state.input);

      if (!str)
	{
	  revm_output("Unable to lookup file\n");
	  return -1;
	}

      /* Load submited file */
      revm_output("\n");
      ret = revm_file_load(str, 0, NULL);
      revm_output("\n");
      
      if (ret < 0)
	return ret;
      
      if (argc > 1)
	revm_traces_add_arguments(argc - 1, argv+1);
      
      XFREE(__FILE__, __FUNCTION__, __LINE__, argv);

      /* Update pointer */
      ac--;
      av[1] = av[0];
      av++;
    }

  revm_config();
  setup_local_cmdhash();
  revm_output(" [*] Type help for regular commands \n\n");
  return (revm_run(ac, av));
}


/* The main Etrace routine */
int		main(int ac, char **av)
{
  return (etrace_main(ac, av));
}



