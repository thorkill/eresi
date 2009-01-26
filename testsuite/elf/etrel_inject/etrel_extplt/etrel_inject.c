/*
** etrel_inject.c for elf testsuite in ERESI
** 
** Started on  Sat May 10 07:01:59 2003 jfv
** $Id$
*/
#include	"libelfsh.h"


#if ERESI32
 #define         TROJANED_FILE   "hijackme32"
 #define	 RELOC_FILE	 "rel.32.o"
 #define         OUTPUT_FILE     "fake_aout32"
#elif ERESI64
 #define         TROJANED_FILE   "hijackme64"
 #define	 RELOC_FILE	 "rel.64.o"
 #define         OUTPUT_FILE     "fake_aout64"
#endif

int		main(int argc, char **argv)
{
  elfshobj_t	*host;
  elfshobj_t	*rel;
  elfshsect_t	*txtsect;
  elfsh_Sym	*puts_troj;
  elfsh_Sym	*hook_func;
  int		idx;
  u_long	addr;

  /* Map host file and relocatable file */
  rel = elfsh_map_obj(RELOC_FILE);
  if (NULL == rel)
    goto err;
  host = elfsh_map_obj(TROJANED_FILE);
  if (NULL == host)
    goto err;

  /* Inject etrel */
  idx = elfsh_inject_etrel(host, rel);
  if (idx < 0)
    goto err;

  /* Get injected's section info */
  txtsect = elfsh_get_section_by_name(host, RELOC_FILE".text", NULL, NULL, NULL);
  if (txtsect == NULL)
    goto err;

  puts_troj = elfsh_get_symbol_by_name(host, "puts_troj");
  idx = elfsh_hijack_function_by_name(host, ELFSH_HIJACK_TYPE_PLT,
				      "puts", puts_troj->st_value, 
				      NULL);
  if (idx < 0)
    goto err;

  hook_func = elfsh_get_symbol_by_name(host, "hook_func");
  idx = elfsh_hijack_function_by_name(host, ELFSH_HIJACK_TYPE_FLOW,
				      "legit_func", hook_func->st_value, 
				      NULL);
  if (idx < 0)
    goto err;


  /* Save it */
  idx = elfsh_save_obj(host, OUTPUT_FILE);
  if (idx < 0)
    goto err;

  puts("[*] ET_REL injected");
  return (0);
 err:
  elfsh_error();
  return (-1);
}
