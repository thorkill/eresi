/*
** info.c for  in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Tue Feb 11 03:32:46 2003 mayhem
** Last update Thu Mar 13 08:29:34 2003 mayhem
*/
#include "elfsh.h"


int		cmd_info()
{
  char		*str;

  str = (world.state.vm_sregx ? world.state.vm_sregx : "Not precised");
  printf("\t\t The ELF shell 0.5 [extra help screen] \n\n"
	 " [*] D and X parameter syntax						          \n\n"
	 "   - Available formats             : regx, regx:rva, regx:rva%%size, regx%%size   \n"
	 "   - regx                          : Regular expression (mandatory)		    \n"
	 "   - rva                           : Byte offset from the beginning (optional)    \n"
	 "   - size                          : Bytes number limit (optional)		  \n\n"
	 " [*] Object access path format    					          \n\n"
	 "   - ELF header                             : filename%shdr%sfield				\n"
	 "   - got/ctors/dtors tables                 : filename%stable[index]				\n"
	 "   - pht/symtab/dynsym/dynamic/sht/sections : filename%stable[index]%sfield			\n"
	 "   - Sections data                          : filename%ssection[index]%smode[indexmode]	\n"
	 "   - Relocation tables                      : filename%srel[indextable][indexent]%sfield	\n\n"
	 " [*] Fields list \n\n"
	 "   - hdr           [ magic class type machine version entry phoff shoff flags ehsize		\n"
	 "                       phentsize shentsize phnum shnum shstrndx pax_pageexec pax_emultramp	\n"
	 "                       pax_mprotect pax_randmmap pax_randexec pax_segmexec ]			\n"
	 "   - sht           [ type offset addr size link info align entsize a w x s m l o ]		\n"
	 "   - pht           [ type offset paddr vaddr filesz memsz flags align ]			\n"
	 "   - symtab/dynsym [ name value size bind type other ]					\n"
	 "   - dynamic       [ val tag ]								\n"
	 "   - section       [ name raw ]								\n"
	 "   - rel           [ type sym offset ]							\n\n"
	 " [*] Current field separator       : %s					    \n"
	 " [*] Current global regular expr.  : %s					    \n"
	 " [*] Project page                  : http://devhell.org/projects/elfsh/	    \n"
	 " [*] Contact point                 : elfsh@devhell.org			  \n\n",
	 ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, 
	 ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, 
	 ELFSH_FIELD_SEP, ELFSH_FIELD_SEP, str);
  return (0);
}
