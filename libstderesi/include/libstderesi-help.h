/*
** libstderesi.help.h for elfsh
**
** Started on Wed Jan 03 19:19:04 2007 jfv
**
** $Id: libstderesi-help.h,v 1.3 2007-12-09 23:00:18 may Exp $
**
*/
#ifndef __REVM_HELP_H_
 #define __REVM_HELP_H_

/* Commands */
#define HLP_DISASM		"Disassemble matching binary objects in current file\n\n"         \
				"disasm parameter\n\n"					          \
				"Parameter format : regx, regx:rva, regx:rva%size, regx%size\n" \
	                        " - regx : Regular expression (mandatory)		\n"       \
	                        " - rva  : Byte offset from the beginning (optional)    \n"       \
	                        " - size : Bytes number limit (optional)	      \n\n"       \

#define HLP_HEXA		"Dump matching binary objects in current file\n\n"		  \
				"X parameter\n\n"					          \
				"Parameter format : regx, regx:rva, regx:rva%size, regx%size\n" \
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
#define HLP_COREINFO		"Print information about the core file"
#define HLP_DYNSYM		"Print dynamic symbol table (.dynsym)"
#define HLP_SHT			"Print Section Header Table (SHT)"
#define HLP_RSHT		"Print the runtime Section Header Table (RSHT)"
#define HLP_PHT			"Print Program Header Table (PHT)"
#define HLP_RPHT		"Print the runtime Program Header Table (RPHT)"
#define HLP_DEBUG		"Print uni format debugging information"
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
#define	HLP_RELINJCT		"Relocate a new .o file inside the current file or process"
#define	HLP_STOP		"Stop executing a script and switch to interactive mode"
#define HLP_TRACES	    	"\t Trace some functions\n\n\t Trace command options: \n\n"  \
	       			" add       funcreg      [optional_traces_name] : add a function to a given trace\n"  \
	       			" rm        funcname     [optional_traces_name] : remove a function from a given trace\n"  \
	       			" exclude   funcreg      <others funcreg>       : add a list of regex to exclude at the end\n"  \
	       			" rmexclude funcreg      <others funcreg>       : delete a regex from the exclude list\n"  \
	       			" enable    funcname|all [optional_traces_name] : enable tracing for a function in a given trace (enabled by default after a add)\n"   \
	       			" disable   funcname|all [optional_traces_name] : disable tracing for a function in a given trace\n"				     \
	       			" create    tracename    <optionals funcnames>  : create a new trace with a pool of traced functions by default\n"		     \
	       			" delete    tracename                           : delete a trace\n"								     \
	       			" flush     tracename                           : remove all functions from a given trace\n"					     \
	       			" [optional_traces_name]                        : list all available traces\n\n"

#define HLP_TRACEADD		"\t Add all submited functions on global traces scope.\n\n"
#define HLP_TRACEEXCLUDE	"\t Add all submited functions to exclude list.\n\n"
#define HLP_TRACERUN		"\t Run a traced binary directly.\n\n"

#define HLP_COLOR               "Associates data types to colors \n\n\t"				 \
				CMD_COLOR" type [bg,fg] color     \n\n\t"				 \
				CMD_COLOR" type [underline,bold] (1=on,0=off)\n\n\t"			 \
				"Available types  : address, number, string, endstring, warnstring \n\t" \
				"                   typestring, fieldstring, instr, pspecial, psname \n\t" \
				"                   pspecial, pversion, prelease, pedition, function \n\t" \
				"                   filename \n\t" \
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

#define	HLP_DEFINE		"Define a given name (param 1) to a constant value (param 2)"
#define	HLP_UNDEF		"Undefine a given name to its value"

/* Configuration commands */
#define	HLP_HIJACK		"Redirect a function to another function \n\n\t" \
				CMD_HIJACK" [hooked funcname|addr] [hooking funcname|addr]"				

#define	HLP_FLUSH		"Flush all injected .o files from current file"
#define	HLP_VLIST		"List current used internal ELFsh/E2dbg variables"
#define	HLP_SOURCE		"Source a script file in current session"
#define HLP_SDIR		"Change the script directory"
#define HLP_LSCRIPTS		"List macro commands"
#define HLP_CAT			"Print the file argument"
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
#define HLP_RCMD		"Execute a command on a remote machine\n\n\t" \
                                CMD_RCMD" <command> <host>" 


/* Flow analysis commands */
#define HLP_GRAPH	        "Generate GVZ files:\n\n\t" \
				CMD_GRAPH " - will plot a function dependency graph\n\t" \
				CMD_GRAPH " func <vaddr|symbol> - will plot dependency graph of given function\n\t" \
				CMD_GRAPH " bloc <vaddr|symbol> - will plot control flow graph of given function\n"

#define	HLP_SETGVL              "Set graph verbosity level \n\n\t"CMD_SETGVL" <0-5> \n"
#define	HLP_INSPECT	        "Disassemble block at symbol/address\n"
#define HLP_FLOWJACK	        "Hijack basic block reference from sym1 to sym2\n"
#define HLP_ADDGOTO	        "\t"CMD_ADDGOTO" <vaddr> <func_addr>\n\n\t" \
				"Inform elfsh about the location and value of a function pointer\n"
#define	HLP_FLOWTEST		"Test command for graph analysis features"
#define	HLP_ANALYSE		"Perform libmjollnir analysis"
#define	HLP_CONTROL		"Print stored control flow information"
#define HLP_UNSTRIP		"Unstrip the current working binary"
#define HLP_RENAME		"Symbol rename: rename <old_symbol> <new_symol>"

/* Scripting only commands */
#define HLP_CMP			"Compare 2 parameters (modify $_ variable)"
#define HLP_TEST		"Test enabled bits in first parameter (modify $_ variable)"
#define	HLP_JMP			"Inconditional jump to script label"
#define HLP_JE			"Jump on label if equal (use last cmp result)"
#define	HLP_JNE			"Jump on label if nont equal (use last cmp result)"
#define	HLP_JL			"Jump on label if lower (use last cmp result)"
#define	HLP_JG			"Jump on label if greater (use last cmp result)"
#define	HLP_JLE			"Jump on label if lower or equal (use last cmp result)"
#define	HLP_JGE			"Jump on label if greater or equal (use last cmp result)"

#define	HLP_FOREACH		"Iterate on hash tables or arrays      \n\n\t" \
				"foreach idxvar of minint until maxint \n\t"   \
				"(...)                                 \n\t"   \
				"forend                                \n\n\t" \
				"foreach elmvar of hashobj [as regx]   \n\t"   \
				"(...)                                 \n\t"   \
				"forend                                \n\n"
#define	HLP_FOREND		HLP_FOREACH

#define	HLP_MATCH		"Conditional program transformation using rewriting \n\n\t"					  \
				"rewrite $variable into \n\n\t"								          \
				" case CandidateType1(fieldname:value, ...) -> ProducedType1(...)::ProducedType2(...)::... \n\t"  \
				" case CandidateType2(fieldname:value, ...) -> ProducedType2(...)::ProducedType2(...)::... \\n\t" \
				" .... \n\t" \
				" default print No match; other eresi commands \n\n\t" \
				"endmatch				\n"
#define	HLP_MATCHEND		HLP_MATCH
#define	HLP_CASE		HLP_MATCH
#define	HLP_DEFAULT		HLP_MATCH
#define	HLP_PRE			"Execute side-effects ERESI commands before transformation (see rewrite)"
#define	HLP_POST		"Execute side-effects ERESI commands after transformation (see rewrite)"
#define	HLP_INTO		"Transform a matching expression (see rewrite command)"

#define	HLP_REFLECT		"Reflect instruction list for a given basic block \n\n\t" \
				"reflect <$block|0xaddr> \n\n\t"			  \
				"Instruction list is then accessible from fileid.instrlist[0xaddr] \n"

/* Prefixes */
#define	HLP_SORT		 "Sorting prefix (use a -addr- or s -size-)"
#define	HLP_QUIET		 "Toggle quiet flag (do not print executed commands)"
#define	HLP_VERB		 "Toggle verbose flag (undo the quiet flag)"
#define HLP_ALL			 "All prefix : set global regular expression"
#define	HLP_ALERT		 "Alert prefix : change alert regular expression"
#define	HLP_FORCE		 "Force sanity checks to be disabled for the command"

/* Type commands */
#define	HLP_INFORM		"Inform a type about a variable \n\n\t"				             \
				"inform type                \t List all variables of a given type \n\t"	     \
				"inform type name [address] \t Inform type about variable           \n" 

#define	HLP_UNINFORM		"Uninform a type about a variable \n\n\t"				        \
				"uninform type              \t Remove all variables associated to type \n\t"    \
				"uninform type name         \t Remove a given variable from type information\n"

#define	HLP_TYPE		"Create or list ERESI types\n\n\t"		                              \
				"type                                      \t List all existing types \n\t"   \
				"type regx                                 \t Print matching types \n\t"      \
				"type name = field1:type1 ... fieldN:typeN \t Create a new type     \n\n\t"   \
				"Fields and types can be named using scripting variables            \n"

#define	HLP_TYPEDEF		"Copy and rename a type \n\n\t"			                            \
				"typedef name newname"

#define HLP_PROFILE		"Change the ELFsh profiling behavior \n\n\t" \
				CMD_PROFILE" enable|disable warn|trace|alloc|debug|all"

#define	HLP_DECLARE		"Declare a new typed named variable \n\n\t" \
				"<typename> <varname> = field1val ... fieldNval\n"


/* Interactive only command */
#define	HLP_LOAD		 "Load a new ELF file"
#define	HLP_UNLOAD		 "Unload file (id or name)"
#define	HLP_SAVE		 "Dump output file at parameter location"
#define	HLP_QUIT		 "Quit current session or script"
#define	HLP_RETURN		 "Return from current ERESI script (put retval in $_)"
#define	HLP_SWITCH		 "Change the current working object"
#define	HLP_LIST		 "List loaded ELF objects"
#define	HLP_WORKSPACE		 "Syntax: workspace [name] : List or switch workspaces"
#define	HLP_EMPTY		 "Empty hash tables given as parameters"
#define	HLP_TABLES		 "Print hash tables list and content \n\n\t"      \
				 CMD_TABLES"      : Print all tables \n\t"        \
				 CMD_TABLES" regx : Print matching tables \n\n\t" \
				 "If a single hash matches, its content is printed"
#define	HLP_LISTS		 "Print linked lists and content \n\n\t"      \
				 CMD_LISTS"      : Print all lists \n\t"        \
				 CMD_LISTS" regx : Print matching lists \n\n\t" \
				 "If a single list matches, its content is printed"

#define	HLP_VECTORS		 "Act on reflective objects \n\n\t"				       \
				 "vectors                                \t List existing vectors\n\t" \
				 "vectors name                           \t List vector content\n\t"   \
				 "vectors name:idx1:...:idxN             \t Print vector entry\n\t"    \
                                 "vectors name:idx1:...:idxN symbol|addr \t Modify vector entry\n\n\t" \
				 "Vector objects indexes can be given by scripting variables \n"
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
#define	HLP_ITRACE		"Enable tracing of debuggee program until next event"
#define	HLP_START		"Start the debuggee program"
#define	HLP_DELETE		"Delete breakpoint by symbol, addr, or ID"
#define	HLP_CONTINUE		"Continue executing debuggee program after break"
#define	HLP_DISPLAY		"List or add displayed commands on breakpoints hits or step"
#define	HLP_UNDISPLAY		"UNIMPLEMENTED : Remove a display command"
#define HLP_THREADS		"Print existing threads list or switch to a given thread view"

#define HLP_CONFIGURE		"Configure internal elfsh behavior."

#endif
