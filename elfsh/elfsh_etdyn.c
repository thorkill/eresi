/*
** elfsh_etdyn.c
** 
** Started on  Tue Feb 11 21:17:33 2003 mayhem
** Last update Wed Feb 12 06:22:59 2003 mayhem
*/
#include "elfsh.h"


/* Add an interp section to the library */
const char __invoke_dynamic_linker__[] 

__attribute__ ((section (".interp"))) = "/lib/ld-linux.so.2";
