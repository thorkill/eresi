/*
** elfsh-help.h for elfsh
**
** Started on  Thu Aug 15 07:19:04 2005 mayhem
*/
#ifndef __ELFSH_HELP_H_
 #define __ELFSH_HELP_H_


/* Commands */
#define HLP_DISASM		"Disassemble matching binary objects in current file\n\n"         \
				"disasm parameter\n\n"					          \
				"Parameter format : regx, regx:rva, regx:rva%%size, regx%%size\n" \
	                        " - regx : Regular expression (mandatory)		\n"       \
	                        " - rva  : Byte offset from the beginning (optional)    \n"       \
	                        " - size : Bytes number limit (optional)	      \n\n"       \

#define HLP_HEXA		"Dump matching binary objects in current file\n\n"		  \
				"X parameter\n\n"					          \
				"Parameter format : regx, regx:rva, regx:rva%%size, regx%%size\n" \
	                        " - regx : Regular expression (mandatory)		\n"       \
	                        " - rva  : Byte offset from the beginning (optional)    \n"       \
	                        " - size : Bytes number limit (optional)	      \n\n"       \

#define HLP_INSERT		"Inject new object in current working file \n\n"	   \
				"insert sect  name code|data|unmap [sz] [align] \n"	   \
	                        "       sym   name value           [sz] \n"		   \
	                        "       phdr  type vaddr           [sz] "

#define	HLP_REMOVE		"Remove ELF object from current working file\n\n"	\
				"remove params \n\n"					\
				" Parameter format : sect name \n"			\
				"                    sym  name \n"			\
				"                    phdr index  "			

#define HLP_REL			"Print all relocation tables from current file"
#define HLP_DYNAMIC		"Print the ELF dynamic section"
#define HLP_SYM			"Print symbol table (.symtab)"
#define HLP_DYNSYM		"Print dynamic symbol table (.dynsym)"
#define HLP_SHT			"Print Section Header Table (SHT)"
#define HLP_RSHT		"Print the runtime Section Header Table (RSHT)"
#define HLP_PHT			"Print Program Header Table (PHT)"
#define HLP_RPHT		"Print the runtime Program Header Table (RPHT)"
#define HLP_STAB		"Print raw stab debugging information"
#define HLP_DWARF		"Print raw dwarf debugging information"
#define HLP_ELF			"Print ELF header"
#define	HLP_INTERP		"Print ELF interpreter path"
#define	HLP_NOTES		"Print the NOTES section content"
#define	HLP_GOT			"Print the Global Offset Table (GOT) section"
#define	HLP_CTORS		"Print CTORS section content"
#define	HLP_DTORS		"Print DTORS section content"
#define	HLP_SHTRM		"Mark Section Header Table as removed"
#define	HLP_COMMENT		"Print comment section content"
#define	HLP_SET			"Set parameter object value"
#define	HLP_GET			"Print parameter object value"
#define	HLP_PRINT		"Print list of resolved values of parameters"
#define	HLP_EXEC		"Fork and execute parameter command (with args)"
#define	HLP_ADD			"Add the 2 parameters"
#define	HLP_SUB			"Substract 2nd parameter to the first one"
#define	HLP_MUL			"Multiply the 2 parameters"
#define	HLP_DIV			"Divide first param by second one"
#define	HLP_MOD			"Modulo operation between 2 parameters"
#define	HLP_INFO		"Print ELF objects usage for scripting"
#define	HLP_METACMD		"Execute a UNIX shell"
#define	HLP_WRITE		"Write raw data to ELF section"
#define	HLP_APPEND		"Append raw data to ELF section"
#define	HLP_EXTEND		"Extend ELF section with zeros"
#define	HLP_FIXUP		"Mark BSS section to be included in disk file"
#define	HLP_FINDREL		"Try to find back stripped relocation information"
#define	HLP_MODLOAD		"Load an ELFsh/E2dbg module"
#define	HLP_MODULOAD		"Unload an ELFsh/E2dbg module"
#define HLP_MODHELP		"Print the ELFsh/E2dbg modules help information"
#define	HLP_HELP		"Print the help summary"
#define	HLP_STRIP		"Mark the symbol table (.symtab) as stripped"
#define	HLP_SSTRIP		"Mark the Section Header Table (SHT) and Symbol Table (.symtab) as stripped"
#define	HLP_RELINJCT		"Relocate a new .o file inside the current file or process"
#define	HLP_STOP		"Raise a SIGSTOP to the current process"
#define HLP_TRACE		"Trace functions"

#define HLP_COLOR               "Associates data types to colors \n\n\t"				 \
				CMD_COLOR" type [bg,fg] color     \n\n\t"				 \
				CMD_COLOR" type [underline,bold] (1=on,0=off)\n\n\t"			 \
				"Available types  : address, number, string, endstring, warnstring \n\t" \
				"                   typestring, fieldstring, instr, pspecial, psname \n\t" \
				"                   pspecial, pversion, prelease, pedition          \n\t" \
				"Available colors : "							 \
				"\033[00;01;30mblack "							 \
				"\033[00;01;31mred "							 \
				"\033[00;901;32mgreen "							 \
				"\033[00;01;33myellow "							 \
				"\033[00;01;34mblue "							 \
				"\033[00;01;35mmagenta "						 \
				"\033[00;01;36mcyan "							 \
				"\033[00mwhite"			       

#define HLP_NOCOLOR             "Toggle color status"
#define HLP_VERSION             "Print the version table with symbol information"
#define HLP_VERDEF              "Print the defined versions table"
#define HLP_VERNEED             "Print the needed versions table"
#define HLP_HASH                "Print the dynsymbols ELF hash table (.hash)"

/* Configuration commands */
#define	HLP_HIJACK		"Redirect a function to another function \n\n\t" \
				CMD_HIJACK" [hooked funcname|addr] [hooking funcname|addr]"				

#define	HLP_FLUSH		"Flush all injected .o files from current file"
#define	HLP_VLIST		"List current used internal ELFsh/E2dbg variables"
#define	HLP_SOURCE		"Source a script file in current session"
#define HLP_SDIR		"Change the script directory"
#define HLP_LSCRIPTS		"List macro commands"
#define HLP_CAT			"Print the file argument"
#define HLP_PROFILE		"Change the ELFsh profiling behavior"
#define	HLP_LOG			"Enable logging of work session \n\n\t" \
				CMD_LOG" [outputfile|stop]"

#define	HLP_EXPORT		"Export an environment variable \n\n\t" \
				CMD_EXPORT" variable value"

#define	HLP_EDIT		"Edit the file given in parameter"

/* Network commands */
#define	HLP_NETWORK		"Activate networking capabilities"
#define	HLP_NETLIST		"List connected network clients"
#define	HLP_NETKILL		"Kill a network client by IP"
#define	HLP_SHARED		"Shared prefix : allow opened file to be shared between users"
#define	HLP_CONNECT		"Add an DUMP connection to given host"
#define	HLP_DISCON		"Kill the DUMP connection from IP"
#define	HLP_PEERSLIST		"List connected DUMP peers"
#define HLP_RCMD		"Execute a command on a remote machine"

/* Scripting only commands */
#define HLP_CMP			"Compare 2 parameters (modify $_ variable)"
#define	HLP_JMP			"Inconditional jump to script label"
#define HLP_JE			"Jump on label if equal (use last cmp result)"
#define	HLP_JNE			"Jump on label if nont equal (use last cmp result)"
#define	HLP_JL			"Jump on label if lower (use last cmp result)"
#define	HLP_JG			"Jump on label if greater (use last cmp result)"
#define	HLP_JLE			"Jump on label if lower or equal (use last cmp result)"
#define	HLP_JGE			"Jump on label if greater or equal (use last cmp result)"

/* Prefixes */
#define	HLP_SORT		 "Sorting prefix (use a -addr- or s -size-)"
#define	HLP_QUIET		 "Toggle quiet flag (do not print executed commands)"
#define	HLP_VERB		 "Toggle verbose flag (undo the quiet flag)"
#define HLP_ALL			 "All prefix : set global regular expression"
#define	HLP_ALERT		 "Alert prefix : change alert regular expression"

/* Interactive only command */
#define	HLP_LOAD		 "Load a new ELF file"
#define	HLP_UNLOAD		 "Unload file (id or name)"
#define	HLP_SAVE		 "Dump output file at parameter location"
#define	HLP_QUIT		 "Quit current session or script"
#define	HLP_SWITCH		 "Change the current working object"
#define	HLP_LIST		 "List loaded ELF objects"
#define	HLP_WORKSPACE		 "List existing workspaces"

/* Debugger commands */
#define	HLP_MODE		"Print or change to debugger (d) or static (s) mode"
#define	HLP_LINKMAP		"Print Linkmap list for this process"
#define	HLP_BT			"Print backtrace (needs frame pointer)"
#define	HLP_BP			"Put a breakpoint on symbol or address (0xADDR)"
#define	HLP_WATCH		"Put a watchpoint on symbol or address (0xADDR)"
#define HLP_STACK		"Dump N words on debuggee stack"
#define HLP_DBGSTACK		"Dump N words on debugger stack"
#define	HLP_DUMPREGS		"Dump registers of debuggee at breakpoint or step"
#define	HLP_STEP		"Enable stepping of debuggee program"
#define	HLP_START		"Start the debuggee program"
#define	HLP_DELETE		"Delete breakpoint by symbol, addr, or ID"
#define	HLP_CONTINUE		"Continue executing debuggee program after break"
#define	HLP_RUN			"Erase existing process by new execution (no fork)"
#define	HLP_DISPLAY		"List or add displayed commands on breakpoints hits or step"
#define	HLP_UNDISPLAY		"UNIMPLEMENTED : Remove a display command"


#endif
