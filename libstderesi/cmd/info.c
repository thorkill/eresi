/*
** @file info.c
** @ingroup libstderesi
** Made by jfv
** Login   <jfv@devhell.org>
** 
** Started on  Tue Feb 11 03:32:46 2003 jfv
** Last update Thu Mar 11 07:59:32 2004 jfv
**
** $Id: info.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"

/**
 *
 */
int		cmd_info()
{
  char		*str;
  char		*alt;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  str = (world.state.revm_sregx  ? world.state.revm_sregx  : "NONE");
  alt = (world.state.revm_salert ? world.state.revm_salert : "NONE");

  snprintf(logbuf, BUFSIZ - 1,
	   "\t\t The Reflective ERESI Vector Machine %s [extra help screen] \n\n"
	   " [*] Object access path format    					          \n\n"
	   "   - ELF header                             : file{name,id}%shdr%sfield	        \n"
	   "   - got/ctors/dtors tables                 : file{name,id}%stable[idx]		\n"
	   "   - pht/symtab/dynsym/dynamic/sht/sections : file{name,id}%stable[idx]%sfield      \n"
	   "   - Sections data                          : file{name,id}%ssection[idx:rva:sz]%sraw	\n"
	   "   - Relocation/got tables                  : file{name,id}%srel[idxtab][idxent]%sfield	\n\n"
	   " [*] Fields list \n\n"
	   "   - hdr           [ magic class type machine version entry phoff shoff flags ehsize	\n"
	   "                     phentsize shentsize phnum shnum shstrndx pax_pageexec pax_emultramp	\n"
	   "                     pax_mprotect pax_randmmap pax_randexec pax_segmexec ]			\n"
	   "   - sht           [ type offset addr size link info align entsize a w x s m l o ]		\n"
	   "   - pht           [ type offset paddr vaddr filesz memsz flags align ]			\n"
	   "   - symtab/dynsym [ name value size bind type other ]					\n"
	   "   - dynamic       [ val tag ]								\n"
	   "   - section       [ name raw ]								\n"
	   "   - rel           [ type sym offset ]							\n"
	   "   - version       [ value ]                                                                \n"
	   "   - verneed       [ index name hash flag next file count aux ]                             \n"
	   "   - verdef        [ index count hash flag aux next name ]                                  \n"
	   "   - hashbucket    [ value nbucket ]                                                        \n"
	   "   - hashchain     [ value nchain ]                                                         \n"
	   "   - got           [ value addr ]    							\n\n"
	   " [*] Current field separator       : %s					    \n"
	   " [*] Current global regular expr.  : %s					    \n"
	   " [*] Current alert  regular expr.  : %s					    \n"
	   " [*] Project page                  : http://www.eresi-project.org		    \n"
	   " [*] Contact point                 : team@eresi-project.org			  \n\n",
	   REVM_VERSION, REVM_SEP, REVM_SEP, REVM_SEP, REVM_SEP, REVM_SEP,
	   REVM_SEP, REVM_SEP, REVM_SEP, REVM_SEP, REVM_SEP, str, alt);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
