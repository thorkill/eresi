/*
** tracer.c for testsuite in elfsh
** 
** Started on  Sat March 21 22:49:23 2007 mxatone
**
**
*/
#include	"revm.h"

#define 	HOST_FILE 	"a.out"
#define		TRACED_FILE 	"fake_aout"

int		main(int argc, char **argv)
{
  elfshobj_t	*host;
  char		*critical[] = {"critical", NULL};
  char		*mainnode[] = {"mainnodes", NULL};
  char		logbuf[BUFSIZ];

  revm_tables_setup();

  /* Map host file */
  host = elfsh_map_obj(HOST_FILE);
  if (NULL == host)
    goto err;

  /* Setup world current */
  world.curjob->curfile = host;

  /* Parse debugging informations */
  revm_edfmt_parse(host);

  /* Init hash dep */
  revm_load_init_dephash(host, host->name);

  /* Load dependences */
  revm_load_enumdep(host);

  /* global scope */
  traces_add(host, "main", NULL);

  /* critical scope */
  traces_create(host, critical[0], NULL);
  traces_add(host, "crypt", critical);
  
  /* mainnodes scope */
  traces_create(host, mainnode[0], NULL);
  traces_add(host, "firstfunc", mainnode);
  traces_add(host, "testcrypt", mainnode);

  /* Save it */
  if (elfsh_save_obj(host, TRACED_FILE) < 0)
    goto err;

  printf("[*] %s traced into %s\n", HOST_FILE, TRACED_FILE);

  return 0;

 err:
  printf("[!] Failed to trace %s into %s\n", HOST_FILE, TRACED_FILE);
  return -1;
}
