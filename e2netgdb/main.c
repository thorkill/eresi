#include "revm.h"
//#include "gdbwrapper.h"

int		netgdb_main(int argc, char **argv)
{
  //  revm_config(".e2netgdbrc");
  printf("Miiiiiiiii");
  fflush(stdout);
  /* The "1" stands for interactive. */
  revm_setup(1, argv, 0, 0);
  revm_config(".e2netgdbrc");

  revm_run(argc, argv);
  return 0;
}


int		main(int argc, char **argv)
{
  if (argc != 3)
    {
      printf("\nUsage: ./netgdb <server> <port>\n");
      return -1;
    }
  //gdbwrap_simpleconnect(argv[1], atoi(argv[2]));
  netgdb_main(argc, argv);
  return 0;
}



