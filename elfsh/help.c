/*
** help.c for elfsh
** 
** Started on  Sat Jan 25 11:19:18 2003 mayhem
*/
#include "elfsh.h"


int		cmd_modhelp()
{
  elfshmod_t	*mod;
  u_int		id;
  char		buf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mod = 0;
  id = atoi(world.curjob->curcmd->param[0]);
  if (!id)
    {
      if (access(world.curjob->curcmd->param[0], R_OK) != 0)
	{
	  snprintf(buf, sizeof(buf), "%s%s", ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	  if (access(buf, R_OK) != 0)
	    {
	      snprintf(buf, sizeof(buf), "%s%s.so",
		       ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	      if (access(buf, R_OK) != 0)
		ELFSH_SETERROR("[elfsh:modload] Cannot find module\n", -1);
	    }
	  mod = hash_get(&mod_hash, buf);
	}
    }
  else
    mod = vm_getmod(id);
  
  if (mod == NULL || mod->help == NULL)
    ELFSH_SETERROR(" [elfsh:modhelp] Module unavailable\n", -1);
  mod->help();
  return (0);
}



int	cmd_help()
{
  char	logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\t\t The ELF shell %s (compiled for %s objects)		        \n\n"

	   " +help			       .::. Print this screen		  \n"
	   " +info                         .::. Extra details help screen info    \n"
	   " +cat           file           .::. Cat command	                  \n"
	   " +sdir          path           .::. Specify the scripts' directory 	  \n"
	   " +lscripts                     .::. List existing script commands	  \n"
	   " +profile                      .::. Enable/disable cmd profiling mode \n"
	   " +quit/exit                    .::. Quit the shell without saving	\n\n"

	   " +JCC    offset|label          .::. Loops (branchement depends on $_ value)	   \n"
	   " +cmp    obj1   obj2	       .::. Compare objects (modify $_ variable) \n\n"

	   " +[shared] load filepath       .::. Load a (shared or not) input file	\n"
	   " *save          outputfile     .::. Dump output file			\n"
	   " +unload        obj{name,id}   .::. Unload object without saving	        \n"
	   " +switch        obj{name,id}   .::. Change the current working object	\n"
	   " +list                         .::. List loaded objects		        \n"
	   " +workspace     [name]         .::. List or switch workspaces             \n\n"

	   " +modload       modpath        .::. Load an ELFsh module		\n"
	   " +modunload     mod{name,id}   .::. Unload an ELFsh module		\n"
	   " +modhelp       mod{name,id}   .::. Print help for an ELFsh module	\n\n"

	   " +net                          .::. Activate net capabilities       \n"
	   " +netlist                      .::. List connected clients	        \n"
	   " +netkill       ip_address     .::. Kill the connection from the ip address   \n"
	   " +connect       ip_address     .::. Add an DUMP connection to given host      \n"
	   " +disconnect    ip_address     .::. Kill the DUMP connection to the ip address\n"
	   " +peerslist                    .::. Kill the connection from the ip address   \n\n"
	   
	   " ~b                            .::. List breakpoints \n"
	   " ~b             hexaddr        .::. Put a breapoint on address \n"			   
	   " ~delete        hexaddr	   .::. Delete a breakpoint on address \n"	   
	   " ~continue                     .::. Continue after breakpoint \n"
	   " ~dumpregs			   .::. Dump registers at last breakpoint \n"
	   " ~dumpstack     num		   .::. Display num words from stack pointer \n" 
	   " ~bt			   .::. Backtrace current process (need fp) \n"
	   " ~linkmap                      .::. Display the linkmap linked list \n"

	   " -fixup                        .::. Insert BSS section ondisk	\n"
	   " *flush                        .::. Remove all injected modules	\n"
	   " +findrel		       .::. Dump absolute references list	\n"
	   " -shtrm                        .::. Mark the SHT as removed		\n"
	   " -strip                        .::. Remove all unmapped sections	\n"
	   " -sstrip                       .::. Same as strip + shtrm		\n"
	   " +elf    [e]                   .::. Print ELF header		\n"
	   " +interp [i]                   .::. Print .interp section		\n"
	   " +pht    [p]    regx           .::. Print Program Header Table (PHT)\n"
	   " +got    [g]    regx           .::. Print Global Offset Table (GOT)	\n"
	   " +sht    [s]    regx           .::. Print Section Header Table (SHT)   \n"
	   " +rel    [r]    regx           .::. Print relocation tables entries    \n"
	   " +notes  [n]    regx           .::. Print Notes sections entries	   \n"
	   " +dyn    [d]    regx           .::. Print .dynamic section entries     \n"
	   " +dynsym [ds]   regx           .::. Print dynamic symtab entries (.dynsym)   \n"
	   " -sym    [st]   regx           .::. Print symtab entries (.symtab)     \n"
	   " -stab   [sb]   regx           .::. Print raw stab entries (.stabs)    \n"
	   " +ctors  [ct]   regx           .::. Print .ctors section               \n"
	   " +dtors  [dt]   regx           .::. Print .dtors section               \n"
	   " +disasm [D]    regx:rva%%size  .::. Disassembly the matching objects	   \n"
	   " +hexa   [X]    regx:rva%%size  .::. Dump in hexadecimal the matching objects  \n"
	   " *reladd objdst objsrc         .::. Insert ET_REL into ET_EXEC		   \n"
	   " +OP     objdst objop          .::. Arithmetics on objects                     \n"
	   " +set    objdst objsrc         .::. Modify object value (see 'info', change $_)\n"
	   " +get    objdst                .::. Print object value (see 'info')		   \n"
	   " *append sctnam objsrc	       .::. Append data to section		   \n"
	   " *extend sctnam size	       .::. Extend section (fill with zeros)	   \n"
	   " +write  objdst objsrc [sz]    .::. Transfer data between objects	           \n"
	   " +print  obj1 obj2 ... objN    .::. Print sequence of objects or constants	   \n"
	   " *redir  func  func2|addr      .::. Redirect a function (need .plt entry)      \n"
	   " *insert [ins] params          .::. Inject new object in current working file  \n"
	   " *remove [rm]  params          .::. Remove object in current working file \n\n"

	   " Available prefixes :							   \n"
	   " all    [a]  regx	       .::. Give a global weak regular expression	   \n"
	   " sort   [sr] {a,s}             .::. Sort entries by address (a) or size (s)	   \n" 
	   " quiet/verb                    .::. Toggle the verbosity flag		 \n\n"
	   
	   " Available JCC                 .::. +jmp, je, jne, jg, jl, jge, jle		   \n"
	   " Available OP                  .::. +add, sub, mul, div, mod		   \n"
	   " Insert params syntax          .::. +sect  name code|data|unmap [sz] [align]   \n"
	   "                                    -sym   name value           [sz]           \n"
	   "                                    +phdr  type vaddr           [sz]	 \n\n"

	   " Remove params syntax          .::. *[sect|sym] name			 \n\n"
	   
	   " Available modules :							   \n"
	   " modtest                       .::. +Sample test module			   \n"
	   " modremap		       .::. +ET_EXEC remapper (x86)			   \n"
	   " modflow		       .::. +Control flow blocks grapher & hijacker (x86)\n\n"
	   
	   " (+) ondisk & memory compliant (*) ondisk only (-) only meaningfull ondisk  \n"
	   " (~) only meaningfull in memory \n" 
	   ,
	   ELFSH_VERSION, 
#if defined(ELFSH32)
	   "32 bits"
#elif defined(ELFSH64)
	   "64 bits"
#else
	   "Unknown built"
#endif
	   );
  
  vm_output(logbuf);
  return (0);
}

