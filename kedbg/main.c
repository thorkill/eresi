#include "revm.h"
#include "libe2dbg.h"
#include "gdbwrapper.h"

#define  ERROR_CONNECTION  "problem when connecting... exiting\n"
#define  USAGE             "Usage: ./netgdb <server> <port> <file>\n"
#define  E2NETGDBNAME      "kedbg"

void		netgdb_create_prompt(char *buf, unsigned int size)
{
  snprintf(buf, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   revm_colorget("%s", "psname" , E2NETGDBNAME),
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

int		netgdb_main(int argc, char **argv)
{
  //  elfshobj_t	*file;

  /* The "1" stands for interactive. */
  revm_setup(1, argv, REVM_STATE_CMDLINE, REVM_SIDE_CLIENT);
  revm_config(".e2netgdbrc");
  revm_set_prompt(netgdb_create_prompt);
  e2dbg_register_command();
  elfsh_set_debug_mode();
  fprintf(stderr, "Micosacholomo: %s\n", world.curjob->ws.name);
  fflush(stderr);
  
/*   file = elfsh_map_obj(argv[3]); */
/*   revm_file_load(argv[3], 0, NULL); */
  revm_run(argc, argv);
  fprintf(stderr, "Double mouche");
  fflush(stderr);
  return 0;
}


int		main(int argc, char **argv)
{
  int           ret;

  if (argc != 4)
    {
      printf(USAGE);
      return -1;
    }

  ret = gdbwrap_simpleconnect(argv[1], atoi(argv[2]));

  if (ret == -1)
    {
      fprintf(stderr, ERROR_CONNECTION);
      return -1;
    }

  netgdb_main(argc, argv);
  return 0;
}


int             cmd_linkmap(void)
{
  return 0;
}


int             e2dbg_linkmap_load(char *name)
{
  name = NULL;
  return 0;
}
