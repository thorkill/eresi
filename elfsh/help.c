/*
** help.c for elfsh
** 
** Started on  Sat Jan 25 11:19:18 2003 mayhem
** Last update Sun Jun 22 01:52:18 2003 mayhem
*/
#include "elfsh.h"


int	cmd_help()
{
  printf("\n\t\t The ELF shell %s					      \n\n"
	 " info                         .::. Extra details help screen info	\n"
	 " load          filepath       .::. Load input file			\n"
	 " save          outputfile     .::. Dump output file			\n"
	 " unload        obj{name,id}   .::. Unload object without saving	\n"
	 " switch        obj{name,id}   .::. Change the current working object	\n"
	 " modload       modpath        .::. Load an ELFsh module		\n"
	 " modunload     mod{name,id}   .::. Unload an ELFsh module		\n"
	 " list   		      .::. List loaded objects		\n"
	 " help			      .::. Print this screen		\n"
	 " fixup                        .::. Insert BSS section in corefile	\n"
	 " findrel		      .::. Dump absolute references list	\n"
	 " shtrm                        .::. Mark the SHT as removed		\n"
	 " quit/exit                    .::. Quit the shell without saving	\n"
	 " elf    [e]                   .::. Print ELF header		\n"
	 " interp [i]                   .::. Print .interp section		\n"
	 " pht    [p]    regx           .::. Print Program Header Table (PHT)	\n"
	 " got    [g]    regx           .::. Print Global Offset Table (GOT)	\n"
	 " sht    [s]    regx           .::. Print Section Header Table (SHT)   \n"
	 " rel    [r]    regx           .::. Print relocation tables entries    \n"
	 " notes  [n]    regx           .::. Print Notes sections entries	\n"
	 " dyn    [d]    regx           .::. Print .dynamic section entries     \n"
	 " dynsym [ds]   regx           .::. Print dynamic symbtab entries (.dynsym)   \n"
	 " sym    [st]   regx           .::. Print symtab entries (.symtab)     \n"
	 " stab   [sb]   regx           .::. Print raw stab entries (.stabs)    \n"
	 " ctors  [ct]   regx           .::. Print .ctors section               \n"
	 " dtors  [dt]   regx           .::. Print .dtors section               \n"
	 " disasm [D]    regx:rva%%size  .::. Disassembly the matching objects	        \n"
	 " hexa   [X]    regx:rva%%size  .::. Dump in hexadecimal the matching objects	\n"
	 " reladd objdst objsrc         .::. Insert ET_REL into ET_EXEC			\n"
	 " OP     objdst objop          .::. Arithmetics on objects (add/sub/mul/div/mod)\n"
	 " set    objdst objsrc         .::. Modify object value (see info)		\n"
	 " get    objdst                .::. Print object value				\n"
	 " append sctnam objsrc	      .::. Append data to section			\n"
	 " extend sctnam size	      .::. Extend section (fill with 0x00)		\n"
	 " write  objdst objsrc         .::. Transfer data between objects	        \n"
	 " print  obj1 obj2 ... objN    .::. Print sequence of objects or constants	\n"
	 " redir  func   func2|addr     .::. Redirect a function		      \n\n"
	 " Available prefixes :								\n"
	 " all    [a]  regx	      .::. Give a global weak regular expression	\n"
	 " sort   [sr] {a,s}            .::. Sort entries by address (a) or size (s)	\n" 
	 " quiet/verb                   .::. Toggle the verbosity flag			\n\n"
	 " Available modules :								\n"
	 " modtest                      .::. Sample test module				\n"
	 " modremap		      .::. ET_EXEC immediate relocator			\n"
	 " modflow		      .::. Control flow blocks grapher			\n\n",
	 ELFSH_VERSION);
  return (0);
}

