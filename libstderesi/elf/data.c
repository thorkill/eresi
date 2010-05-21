/**
* @file libstderesi/elf/data.c
** @ingroup elf
*/
/**
 *
 * 
 * Started on  Mon Feb 26 04:06:38 2001 jfv
 * Last update Wed Mar 10 12:14:24 2004 jfv
 *
 * $Id$
 *
 */
#include "libstderesi.h"


revmconst_t			asm_instr_type[] =
  {
    {"Undefined instruction"                            , "ASM_TYPE_NONE"      , ASM_TYPE_NONE},
    {"Branch instruction"                               , "ASM_TYPE_BRANCH"    , ASM_TYPE_BRANCH},
    {"Procedure calling instruction"                    , "ASM_TYPE_CALLPROC"  , ASM_TYPE_CALLPROC},
    {"Return instruction"                               , "ASM_TYPE_RETPROC"   , ASM_TYPE_RETPROC},
    {"Arithmetic (or logic) instruction"                , "ASM_TYPE_ARITH"     , ASM_TYPE_ARITH},
    {"Instruction that reads from memory"               , "ASM_TYPE_LOAD"      , ASM_TYPE_LOAD},
    {"Instruction that writes in memory"                , "ASM_TYPE_STORE"     , ASM_TYPE_STORE},
    {"Architecture dependent instruction"               , "ASM_TYPE_ARCH"      , ASM_TYPE_ARCH},
    {"Flag-modifier instruction"                        , "ASM_TYPE_WRITEFLAG" , ASM_TYPE_WRITEFLAG},
    {"Flag-reader instruction"                          , "ASM_TYPE_READFLAG"  , ASM_TYPE_READFLAG},
    {"Interrupt/call-gate instruction"                  , "ASM_TYPE_INT"       , ASM_TYPE_INT},
    {"Assignment instruction"                           , "ASM_TYPE_ASSIGN"    , ASM_TYPE_ASSIGN},
    {"Comparison or test instruction"                   , "ASM_TYPE_COMPARISON", ASM_TYPE_COMPARISON},
    {"Instruction modifies control registers"           , "ASM_TYPE_CONTROL"   , ASM_TYPE_CONTROL},
    {"Instruction that does nothing"                    , "ASM_TYPE_NOP"       , ASM_TYPE_NOP},
    {"Instruction modifies stack pointer"               , "ASM_TYPE_TOUCHSP"   , ASM_TYPE_TOUCHSP},
    {"Bit Testing Instruction"                          , "ASM_TYPE_BITTEST"   , ASM_TYPE_BITTEST},
    {"Bit Setting Instruction"                          , "ASM_TYPE_BITSET"    , ASM_TYPE_BITSET},
    {"Instruction increment or decrement"               , "ASM_TYPE_INCDEC"    , ASM_TYPE_INCDEC},
    {"Instruction creates function prolog"              , "ASM_TYPE_PROLOG"    , ASM_TYPE_PROLOG},
    {"Instruction creates function epilog"              , "ASM_TYPE_EPILOG"    , ASM_TYPE_EPILOG},
    {"Instruction stops the program"                    , "ASM_TYPE_STOP"      , ASM_TYPE_STOP},
    {"Instruction accesses I/O locations (e.g. ports)." , "ASM_TYPE_IO"        , ASM_TYPE_IO},
    {"Instruction executes conditionally"               , "ASM_TYPE_CONDCONTROL", ASM_TYPE_CONDCONTROL},
    {"Instruction changes control indirectly."          , "ASM_TYPE_INDCONTROL", ASM_TYPE_INDCONTROL},
    {"Type that doesn't fit the ones above"             , "ASM_TYPE_OTHER"     , ASM_TYPE_OTHER},
  };


/**
 * Architecture-independant operand types 
 *
 */
revmconst_t			asm_operand_type[] = 
  {
    {"Undefined operand type"    , "ASM_OPTYPE_NONE", ASM_OPTYPE_NONE},
    {"Immediate operand type"    , "ASM_OPTYPE_IMM", ASM_OPTYPE_IMM},
    {"Register operand type"     , "ASM_OPTYPE_REG", ASM_OPTYPE_REG},
    {"Memory access operand type", "ASM_OPTYPE_MEM", ASM_OPTYPE_MEM},
  };


/**
 *  Program header (segment) entry strings 
 */
revmconst_t			elfsh_seg_type[] = 
{
  {"NULL segment"         , "PT_NULL"   , PT_NULL},
  {"Loadable segment"     , "PT_LOAD"   , PT_LOAD},
  {"Dynamic linking info" , "PT_DYNAMIC", PT_DYNAMIC},
  {"Program interpreter"  , "PT_INTERP" , PT_INTERP},
  {"Auxiliary information", "PT_NOTE"   , PT_NOTE},
  {"Reserved"             , "PT_SHLIB"  , PT_SHLIB},
  {"Program header table" , "PT_PHDR"   , PT_PHDR},
};

/** 
 * Extended program header types 
 */
revmconst_t			elfsh_extseg_type[] = 
{
  {"Stack flags"               , "PT_GNU_STACK"   , PT_GNU_STACK},
  {"Read-only after relocation", "PT_GNU_RELRO"   , PT_GNU_RELRO},
  {"New PaX flags"             , "PT_PAX_FLAGS"   , PT_PAX_FLAGS},
  {"GCC .eh_frame_hdr"         , "PT_GNU_EH_FRAME", PT_GNU_EH_FRAME},
  {"MIPS Registers informations"    , "PT_MIPS_REGINFO", PT_MIPS_REGINFO},
};




/** 
 * Encoding type in ELF header 
 */
revmconst_t			elfsh_encoding[] = 
{
  {"Invalid encoding", "ELFDATANONE", ELFDATANONE},
  {"Little endian"   , "ELFDATA2LSB", ELFDATA2LSB},
  {"Big endian"      , "ELFDATA2MSB", ELFDATA2MSB},
};


/** 
 * Section type strings 
 */
revmconst_t			elfsh_sh_type[] = 
{
  {"NULL section"           , "SHT_NULL"    , SHT_NULL},
  {"Program data"           , "SHT_PROGBITS", SHT_PROGBITS},
  {"Symbol table"           , "SHT_SYMTAB"  , SHT_SYMTAB},
  {"String table"           , "SHT_STRTAB"  , SHT_STRTAB},
  {"Reloc. ent. w/ addends" , "SHT_RELA"    , SHT_RELA},
  {"Symbol hash table"      , "SHT_HASH"    , SHT_HASH},
  {"Dynamic linking info"   , "SHT_DYNAMIC" , SHT_DYNAMIC},
  {"Notes"                  , "SHT_NOTES"   , SHT_NOTE},
  {"BSS"                    , "SHT_NOBITS"  , SHT_NOBITS},
  {"Reloc. ent. w/o addends", "SHT_REL"     , SHT_REL},
  {"Reserved"               , "SHT_SHLIB"   , SHT_SHLIB},
  {"Dynamic linker symtab"  , "SHT_DYNSYM"  , SHT_DYNSYM},
  {"MIPS Registers Information", "SHT_MIPS_REGINFO", SHT_MIPS_REGINFO},
  {"Version definition section", "SHT_GNU_verdef", SHT_GNU_verdef},
  {"Version needs section"  , "SHT_GNU_verneed", SHT_GNU_verneed},
  {"Version symbol table"   , "SHT_GNU_versym" , SHT_GNU_versym},
};


/** 
 * ELF object type strings 
 */
revmconst_t			elfsh_obj_type[] = 
{
  {"Unknown"               , "ET_NONE", ET_NONE},
  {"Relocatable object"    , "ET_REL" , ET_REL},
  {"Executable object"     , "ET_EXEC", ET_EXEC},
  {"Shared object"         , "ET_DYN" , ET_DYN},
  {"Core file"             , "ET_CORE", ET_CORE},
};

/** 
 * Symbol Binding strings 
 */
revmconst_t			elfsh_sym_bind[] = 
{
  {"Local"            , "STB_LOCAL"  , STB_LOCAL},
  {"Global"           , "STB_GLOBAL" , STB_GLOBAL},
  {"Weak"             , "STB_WEAK"   , STB_WEAK},
};


/** 
 * Symbol Type strings 
 */
revmconst_t			elfsh_sym_type[] = 
{
  {"NOTYPE"  , "STT_NOTYPE" , STT_NOTYPE},
  {"VARIABLE", "STT_OBJECT" , STT_OBJECT},
  {"FUNCTION", "STT_FUNC"   , STT_FUNC},
  {"SECTION" , "STT_SECTION", STT_SECTION},
  {"FILENAME", "STT_FILE"   , STT_FILE},
  {"COMMON"  , "STT_COMMON" , STT_COMMON},
  {"TLS"     , "STT_TLS"    , STT_TLS},
  {"BLOCK"   , "STT_BLOCK"  , STT_BLOCK},
};


/** 
 * Version entry type strings 
*/
revmconst_t                  elfsh_verentry_type[] =
{
  {"NONE"          , "ELFSH_VERTYPE_NONE" , ELFSH_VERTYPE_NONE},
  {"UNKNOWN"       , "ELFSH_VERTYPE_UNK"  , ELFSH_VERTYPE_UNK},
  {"NEED"          , "ELFSH_VERTYPE_NEED" , ELFSH_VERTYPE_NEED},
  {"DEF"           , "ELFSH_VERTYPE_DEF"  , ELFSH_VERTYPE_DEF},
};


/** 
 * Dynamic section entry type strings 
 */
revmconst_t			elfsh_dynentry_type[] = 
{
  {"END OF DYNAMIC SECTION"           , "DT_NULL"        , DT_NULL},
  {"Name of needed library"           , "DT_NEEDED"      , DT_NEEDED},
  {"Size in bytes for .rel.plt"       , "DT_PLTRELSZ"     , DT_PLTRELSZ},
  {"Processor defined value"          , "DT_PLTGOT"      , DT_PLTGOT}, 
  {"Address of symbol hash table"     , "DT_HASH"        , DT_HASH},
  {"Address of dynamic string table"  , "DT_STRTAB"       , DT_STRTAB},
  {"Address of dynamic symbol table"  , "DT_SYMTAB"      , DT_SYMTAB},
  {"Address of relocs info w/ add-end", "DT_RELA"        , DT_RELA},
  {"Totsize of relocs info w/ add-end", "DT_RELASZ"      , DT_RELASZ},
  {"Size of RELA entry"               , "DT_RELAENT"     , DT_RELAENT},
  {"Size of string table"             , "DT_STRSZ"       , DT_STRSZ},
  {"Size of symbol table entry"       , "DT_SYMENT"      , DT_SYMENT},
  {"Address of init function"         , "DT_INIT"        , DT_INIT},
  {"Address of fini function"         , "DT_FINI"        , DT_FINI},
  {"Name of shared object"            , "DT_SONAME"      , DT_SONAME},
  {"Library search path"              , "DT_RPATH"       , DT_RPATH},
  {"Start symbol search here"         , "DT_SYMBOLIC"    , DT_SYMBOLIC},
  {"Address of .rel.got section"      , "DT_REL"         , DT_REL},
  {"Total size of .rel section"       , "DT_RELSZ"       , DT_RELSZ},
  {"Size of a REL entry"              , "DT_RELENT"      , DT_RELENT},
  {"Type of reloc in PLT"             , "DT_PLTREL"      , DT_PLTREL},
  {"Debugging entry (unknown)"        , "DT_DEBUG"       , DT_DEBUG},
  {"Reloc might modify .text"         , "DT_TEXTREL"     , DT_TEXTREL},
  {"Address of .rel.plt"              , "DT_JMPREL"      , DT_JMPREL},
  {"Process relocations of object"    , "DT_BIND_NOW"    , DT_BIND_NOW},
  {"Array with addresses of init fct" , "DT_INIT_ARRAY"  , DT_INIT_ARRAY},
  {"Array with addresses of fini fct" , "DT_FINI_ARRAY"  , DT_FINI_ARRAY},
  {"Size in bytes of DT_INIT_ARRAY"   , "DT_INIT_ARRAYSZ", DT_INIT_ARRAYSZ},
  {"Size in bytes of DT_FINI_ARRAY"   , "DT_FINI_ARRAYSZ", DT_FINI_ARRAYSZ},
  {"Library search path"              , "DT_RUNPATH"     , DT_RUNPATH},
  {"Flags for the object being loaded", "DT_FLAGS"       , DT_FLAGS},
  {"Start of encoded range"           , "DT_ENCODING"    , DT_ENCODING},
  {"Pre-init function addresses array", "DT_PREINIT_ARRAY", DT_PREINIT_ARRAY},
  {"Size in bytes of DT_PREINIT_ARRAY", "DT_PREINIT_ARRAYSZ", DT_PREINIT_ARRAYSZ},
  {"Number used"                      , "DT_NUM"         , DT_NUM},
};


/** 
 * Extended dynamic types 
 */
revmconst_t			elfsh_extdyn_type[] = 
{
  {"Feature selection"                , "DT_FEATURE_1" , DT_FEATURE_1},
  {"DT entries flags"                 , "DT_POSFLAG_1" , DT_POSFLAG_1},
  {"Syminfo table size"               , "DT_SYMINSZ"   , DT_SYMINSZ},
  {"Syminfo entry size"               , "DT_SYMINENT"  , DT_SYMINENT},
  {"Syminfo table"                    , "DT_SYMINFO"   , DT_SYMINFO},
  {"GNU version VERSYM"               , "DT_VERSYM"    , DT_VERSYM},
  {"GNU version RELACCOUNT"           , "DT_RELACOUNT" , DT_RELACOUNT},
  {"GNU version RELCOUNT"             , "DT_RELCOUNT"  , DT_RELCOUNT},
  {"State flags"                      , "DT_FLAGS_1"   , DT_FLAGS_1},
  {"SUN version table address"        , "DT_VERDEF"    , DT_VERDEF},
  {"SUN version table number"         , "DT_VERDEFNUM" , DT_VERDEFNUM},
  {"SUN needed version table"         , "DT_VERNEED"   , DT_VERNEED},
  {"SUN needed version number"        , "DT_VERNEEDNUM", DT_VERNEEDNUM},
  {"Shared object to load before self", "DT_AUXILIARY" , DT_AUXILIARY},
  {"Shared object to get values from" , "DT_FILTER"    , DT_FILTER},
  {"[?]"                              , "DT_CHECKSUM"  , DT_CHECKSUM},
  {"[?]"                              , "DT_PLTPADSZ"  , DT_PLTPADSZ},
  {"[?]"                              , "DT_MOVEENT"   , DT_MOVEENT},
  {"[?]"                              , "DT_MOVESZ"    , DT_MOVESZ},
};

/** 
 * MIPS dynamic types 
 */
revmconst_t                    elfsh_mipsdyn_type[] = 
{
  {"Runtime linker interface version" ,	"DT_MIPS_RLD_VERSION"	, DT_MIPS_RLD_VERSION},
  {"Timestamp"			      ,	"DT_MIPS_TIME_STAMP"	, DT_MIPS_TIME_STAMP},
  {"Checksum"			      , "DT_MIPS_ICHECKSUM"	, DT_MIPS_ICHECKSUM},
  {"Version string (string tbl index)", "DT_MIPS_IVERSION"	, DT_MIPS_IVERSION},
  {"Flags"			      ,	"DT_MIPS_FLAGS"		, DT_MIPS_FLAGS},
  {"Base address"		      ,	"DT_MIPS_BASE_ADDRESS"	, DT_MIPS_BASE_ADDRESS},
  {"[?]"			      , "DT_MIPS_MSYM"		, DT_MIPS_MSYM},
  {"Address of CONFLICT section"      , "DT_MIPS_CONFLICT"	, DT_MIPS_CONFLICT},
  {"Address of LIBLIST section"	      , "DT_MIPS_LIBLIST"	, DT_MIPS_LIBLIST},
  {"Number of local GOT entries"      , "DT_MIPS_LOCAL_GOTNO"	, DT_MIPS_LOCAL_GOTNO},
  {"Number of CONFLICT entries"	      , "DT_MIPS_CONFLICTNO"	, DT_MIPS_CONFLICTNO},
  {"Number of LIBLIST entries"	      , "DT_MIPS_LIBLISTNO"	, DT_MIPS_LIBLISTNO},
  {"Number of DYNSYM entries"	      , "DT_MIPS_SYMTABNO"	, DT_MIPS_SYMTABNO},
  {"First external DYNSYM"	      , "DT_MIPS_UNREFEXTNO"	, DT_MIPS_UNREFEXTNO},
  {"First GOT entry in DYNSYM"	      , "DT_MIPS_GOTSYM"	, DT_MIPS_GOTSYM},
  {"Number of GOT page table entries" , "DT_MIPS_HIPAGENO"	, DT_MIPS_HIPAGENO},
  {"Address of run time loader map."  , "DT_MIPS_RLD_MAP"	, DT_MIPS_RLD_MAP},
  {"Delta C++ class definition"	      , "DT_MIPS_DELTA_CLASS"	, DT_MIPS_DELTA_CLASS},
  {"Number of entries in DT_MIPS_DELTA_CLASS"
				      , "DT_MIPS_DELTA_CLASS_NO", DT_MIPS_DELTA_CLASS_NO},
  {"Delta C++ class instances"	      , "DT_MIPS_DELTA_INSTANCE", DT_MIPS_DELTA_INSTANCE},
  {"Number of entries in DT_MIPS_DELTA_INSTANCE"
				      , "DT_MIPS_DELTA_INSTANCE_NO"
								, DT_MIPS_DELTA_INSTANCE_NO},
  {"Delta relocations", "DT_MIPS_DELTA_RELOC"			, DT_MIPS_DELTA_RELOC},
  {"Number of entries in DT_MIPS_DELTA_RELOC", "DT_MIPS_DELTA_RELOC_NO"
								, DT_MIPS_DELTA_RELOC_NO},
  {"Delta symbols that Delta relocations refer to"
				      , "DT_MIPS_DELTA_SYM"	, DT_MIPS_DELTA_SYM},
  {"Number of entries in DT_MIPS_DELTA_SYM"
				      , "DT_MIPS_DELTA_SYM_NO"	, DT_MIPS_DELTA_SYM_NO},
  {"Delta symbols that hold the class declaration"
				      , "DT_MIPS_DELTA_CLASSSYM", DT_MIPS_DELTA_CLASSSYM},
  {"Number of entries in DT_MIPS_DELTA_CLASSSYM"
				      , "DT_MIPS_DELTA_CLASSSYM_NO"
								, DT_MIPS_DELTA_CLASSSYM_NO},
  {"Flags indicating for C++ flavor"  , "DT_MIPS_CXX_FLAGS"	, DT_MIPS_CXX_FLAGS},
  {"[?]"			      , "DT_MIPS_PIXIE_INIT"	, DT_MIPS_PIXIE_INIT},
  {"[?]"			      , "DT_MIPS_SYMBOL_LIB"	, DT_MIPS_SYMBOL_LIB},
  {"[?]"			      , "DT_MIPS_LOCALPAGE_GOTIDX"
								, DT_MIPS_LOCALPAGE_GOTIDX},
  {"[?]"			      , "DT_MIPS_LOCAL_GOTIDX"	, DT_MIPS_LOCAL_GOTIDX},
  {"[?]"			      , "DT_MIPS_HIDDEN_GOTIDX"	, DT_MIPS_HIDDEN_GOTIDX},
  {"[?]"			      , "DT_MIPS_PROTECTED_GOTIDX"
								, DT_MIPS_PROTECTED_GOTIDX},
  {"Address of .options"	      , "DT_MIPS_OPTIONS"	, DT_MIPS_OPTIONS},
  {"Address of .interface"	      , "DT_MIPS_INTERFACE"	, DT_MIPS_INTERFACE},
  {"[?]"			      , "DT_MIPS_DYNSTR_ALIGN"	, DT_MIPS_DYNSTR_ALIGN},
  {"Size of the .interface section"   , "DT_MIPS_INTERFACE_SIZE", DT_MIPS_INTERFACE_SIZE},
  {"Address of rld_text_rsolve function stored in GOT"
				      , "DT_MIPS_RLD_TEXT_RESOLVE_ADDR"
								, DT_MIPS_RLD_TEXT_RESOLVE_ADDR},
  {"Default suffix of dso to be added by rld on dlopen() calls"
				      , "DT_MIPS_PERF_SUFFIX"	, DT_MIPS_PERF_SUFFIX},
  {"(O32)Size of compact rel section" , "DT_MIPS_COMPACT_SIZE"	, DT_MIPS_COMPACT_SIZE},
  {"GP value for aux GOTs"	      , "DT_MIPS_GP_VALUE"	, DT_MIPS_GP_VALUE},
  {"Address of aux .dynamic"	      , "DT_MIPS_AUX_DYNAMIC"	, DT_MIPS_AUX_DYNAMIC},
};


/** 
 * The next 4 arrays are special flag based DT entries
 */
/**
 * Enumeration of '<< 1' increment 
 */
revmconst_t			elfsh_feature1[] = 
{
  {"Option 1: PARINIT(?)", "DTF_1_PARINIT", DTF_1_PARINIT},
  {"Option 2: CONFEXP(?)", "DTF_1_CONFEXP", DTF_1_CONFEXP},
};


revmconst_t			elfsh_posflag1[] = 
{
  {"Lazyload following object"              , "DF_P1_LAZYLOAD" , DF_P1_LAZYLOAD},
  {"Next object symbols maybe not available", "DF_P1_GROUPPERM", DF_P1_GROUPPERM},
};

revmconst_t			elfsh_flags[] = 
{
  {"Object may use DF_ORIGIN"        , "DF_ORIGIN"  , DF_ORIGIN},
  {"Symbol resolutions starts here"  , "DF_SYMBOLIC", DF_SYMBOLIC},	
  {"Object contains text relocations", "DF_TEXTREL" , DF_TEXTREL},
  {"No lazy binding for this object" , "DF_BIND_NOW", DF_BIND_NOW},
};

revmconst_t			elfsh_flags1[] = 
{
  {"Set RTLD_NOW for this object"      , "DF_1_NOW"      , DF_1_NOW},
  {"Set RTLD_GLOBAL for this object"   , "DF_1_GLOBAL"   , DF_1_GLOBAL},
  {"Set RTLD_GROUP for this object"    , "DF_1_GROUP"    , DF_1_GROUP},
  {"Set RTLD_NODELETE for this object" , "DF_1_NODELETE" , DF_1_NODELETE},
  {"Trigger filtee loading at runtime" , "DF_1_LOADFLTR" , DF_1_LOADFLTR},
  {"Set RTLD_INITFIRST for this object", "DF_1_INITFIRST", DF_1_INITFIRST},
  {"Set RTLD_NOOPEN for this object"   , "DF_1_NOOPEN"   , DF_1_NOOPEN},
  {"$ORIGIN must be handled"           , "DF_1_ORIGIN"   , DF_1_ORIGIN},
  {"Direct binding enabled"            , "DF_1_DIRECT"   , DF_1_DIRECT},
  {"Option 200: TRANS(?)"              , "DF_1_TRANS"    , DF_1_TRANS},
  {"Object is used to interpose"       , "DF_1_INTERPOSE", DF_1_INTERPOSE},
  {"Ignore default lib search path"    , "DF_1_NODEFLIB" , DF_1_NODEFLIB},
  {"Option 1000: NODUMP(?)"            , "DF_1_NODUMP"   , DF_1_NODUMP},
  {"Option 2000: CONFALT(?)"           , "DF_1_CONFALT"	 , DF_1_CONFALT},
  {"Option 4000: ENDFILTEE(?)"         , "DF_1_ENDFILTEE", DF_1_ENDFILTEE},	
};

/* mips DT_MIPSFLAGS flags */
revmconst_t                    elfsh_mipsflags[] =
{
  {"No flags"			       , "RHF_NONE"	 , RHF_NONE},
  {"Use quickstart"		       , "RHF_QUICKSTART", RHF_QUICKSTART},  
  {"Hash size not power of 2"	       , "RHF_NOTPOT"	 , RHF_NOTPOT}, 
  {"Ignore LD_LIBRARY_PATH"	       , "RHF_NO_LIBRARY_REPLACEMENT"
							 , RHF_NO_LIBRARY_REPLACEMENT},
  {"[?]"			       , "RHF_NO_MOVE"	 , RHF_NO_MOVE},
  {"[?]"			       , "RHF_SGI_ONLY"	 , RHF_SGI_ONLY},
  {"[?]"			       , "RHF_GUARANTEE_INIT"
							 , RHF_GUARANTEE_INIT},
  {"[?]"			       , "RHF_DELTA_C_PLUS_PLUS"
							 , RHF_DELTA_C_PLUS_PLUS},
  {"[?]"			       , "RHF_GUARANTEE_START_INIT"
							 , RHF_GUARANTEE_START_INIT},
  {"[?]"			       , "RHF_PIXIE"	 , RHF_PIXIE},
  {"[?]"			       , "RHF_DEFAULT_DELAY_LOAD"
							 , RHF_DEFAULT_DELAY_LOAD},
  {"[?]"			       , "RHF_REQUICKSTART"
							 , RHF_REQUICKSTART},
  {"[?]"			       , "RHF_REQUICKSTARTED"
							 , RHF_REQUICKSTARTED},
  {"[?]"			       , "RHF_CORD"	 , RHF_CORD},
  {"[?]"			       , "RHF_NO_UNRES_UNDEF"
							 , RHF_NO_UNRES_UNDEF},
  {"[?]"			       , "RHF_RLD_ORDER_SAFE"	
							 , RHF_RLD_ORDER_SAFE},
  /*
    // from http://www.iagu.net/docs/dec/AA-PS31D-TET1_html/asm10.html

  {"Object may be quickstarted by loader" , "RHF_QUICKSTART" , RHF_QUICKSTART},
  {"Hash size not a power of two "	  , "RHF_NOTPOT"     , RHF_NOTPOT},
  {"[?]"				  , "RHF_NO_LIBRARY_", RHF_NO_LIBRARY_},
  {"Use default system libraries only"	  , "REPLACEMENT"    , REPLACEMENT},
  {"Do not relocate"			  , "RHF_NO_MOVE"    , RHF_NO_MOVE},
  {"Symbol resolution same as DT_SYMBOLIC", "RHF_RING_SEARCH", RHF_RING_SEARCH},
  {"Depth first symbol resolution "	  , "RHF_DEPTH_FIRST", RHF_DEPTH_FIRST},
  {"[?]"				  , "RHF_USE_31BIT_" , RHF_USE_31BIT_},
  {"TASO (Truncated Address Support Option) objects"
                                          , "ADDRESSES"	     , ADDRESSES},
  */
};

/** 
 * Relocation types strings 
 */
revmconst_t			elfsh_rel_type_i386[] = 
  {
    {"No relocation"                   , "R_386_NONE"    , R_386_NONE},
    {"Direct 32 bit"                   , "R_386_32"      , R_386_32},
    {"Relative 32 bit"                 , "R_386_PC32"    , R_386_PC32},
    {"32 bit GOT entry"                , "R_386_GOT32"   , R_386_GOT32},
    {"32 bit PLT entry"                , "R_386_PLT32"   , R_386_PLT32},
    {"Copy symbol at runtime"          , "R_386_COPY"    , R_386_COPY},
    {"Create GOT entry"                , "R_386_GLOB_DAT", R_386_GLOB_DAT},
    {"Create PLT entry"                , "R_386_JMP_SLOT", R_386_JMP_SLOT},
    {"Adjust by program base address"  , "R_386_RELATIVE", R_386_RELATIVE},
    {"32 bit offset to GOT"            , "R_386_GOTOFF"  , R_386_GOTOFF},
    {"32 bit PC relative offset to GOT", "R_386_GOTPC"   , R_386_GOTPC},
  };

/** 
 * Relocation types for itanium 
 */
revmconst_t			elfsh_rel_type_ia64[] =
  {
    {"none"				, "R_IA64_NONE"		, R_IA64_NONE},
    {"symbol + addend, add imm14"	, "R_IA64_IMM14"	, R_IA64_IMM14}, 
    {"symbol + addend, add imm22"	, "R_IA64_IMM22"	, R_IA64_IMM22}, 
    {"symbol + addend, mov imm64"	, "R_IA64_IMM64"	, R_IA64_IMM64}, 
    {"symbol + addend, data4 MSB"	, "R_IA64_DIR32MSB"	, R_IA64_DIR32MSB}, 
    {"symbol + addend, data4 LSB"	, "R_IA64_DIR32LSB"	, R_IA64_DIR32LSB}, 
    {"symbol + addend, data8 MSB"	, "R_IA64_DIR64MSB"	, R_IA64_DIR64MSB}, 
    {"symbol + addend, data8 LSB"	, "R_IA64_DIR64LSB"	, R_IA64_DIR64LSB}, 
    {"@gprel(sym + add), add imm22"	, "R_IA64_GPREL22"	, R_IA64_GPREL22}, 
    {"@gprel(sym + add), mov imm64"	, "R_IA64_GPREL64I"	, R_IA64_GPREL64I}, 
    {"@gprel(sym + add), data4 MSB"	, "R_IA64_GPREL32MSB"	, R_IA64_GPREL32MSB}, 
    {"@gprel(sym + add), data4 LSB"	, "R_IA64_GPREL32LSB"	, R_IA64_GPREL32LSB}, 
    {"@gprel(sym + add), data8 MSB"	, "R_IA64_GPREL64MSB"	, R_IA64_GPREL64MSB}, 
    {"@gprel(sym + add), data8 LSB"	, "R_IA64_GPREL64LSB"	, R_IA64_GPREL64LSB},
    {"@ltoff(sym + add), add imm22"	, "R_IA64_LTOFF22"	, R_IA64_LTOFF22}, 
    {"@ltoff(sym + add), mov imm64"	, "R_IA64_LTOFF64I"	, R_IA64_LTOFF64I}, 
    {"@pltoff(sym + add), add imm22"	, "R_IA64_PLTOFF22"	, R_IA64_PLTOFF22}, 
    {"@pltoff(sym + add), mov imm64"	, "R_IA64_PLTOFF64I"	, R_IA64_PLTOFF64I}, 
    {"@pltoff(sym + add), data8 MSB"	, "R_IA64_PLTOFF64MSB"	, R_IA64_PLTOFF64MSB}, 
    {"@pltoff(sym + add), data8 LSB"	, "R_IA64_PLTOFF64LSB"	, R_IA64_PLTOFF64LSB}, 
    {"@fptr(sym + add), mov imm64"	, "R_IA64_FPTR64I"	, R_IA64_FPTR64I}, 
    {"@fptr(sym + add), data4 MSB"	, "R_IA64_FPTR32MSB"	, R_IA64_FPTR32MSB}, 
    {"@fptr(sym + add), data4 LSB"	, "R_IA64_FPTR32LSB"	, R_IA64_FPTR32LSB}, 
    {"@fptr(sym + add), data8 MSB"	, "R_IA64_FPTR64MSB"	, R_IA64_FPTR64MSB}, 
    {"@fptr(sym + add), data8 LSB"	, "R_IA64_FPTR64LSB"	, R_IA64_FPTR64LSB}, 
    {"@pcrel(sym + add), brl"		, "R_IA64_PCREL60B"	, R_IA64_PCREL60B}, 
    {"@pcrel(sym + add), ptb, call"	, "R_IA64_PCREL21B"	, R_IA64_PCREL21B}, 
    {"@pcrel(sym + add), chk.s"		, "R_IA64_PCREL21M"	, R_IA64_PCREL21M}, 
    {"@pcrel(sym + add), fchkf"		, "R_IA64_PCREL21F"	, R_IA64_PCREL21F}, 
    {"@pcrel(sym + add), data4 MSB"	, "R_IA64_PCREL32MSB"	, R_IA64_PCREL32MSB}, 
    {"@pcrel(sym + add), data4 LSB"	, "R_IA64_PCREL32LSB"	, R_IA64_PCREL32LSB}, 
    {"@pcrel(sym + add), data8 MSB"	, "R_IA64_PCREL64MSB"	, R_IA64_PCREL64MSB}, 
    {"@pcrel(sym + add), data8 LSB"	, "R_IA64_PCREL64LSB"	, R_IA64_PCREL64LSB}, 
    {"@ltoff(@fptr(s+a)), imm22"	, "R_IA64_LTOFF_FPTR22"	, R_IA64_LTOFF_FPTR22}, 
    {"@ltoff(@fptr(s+a)), imm64"	, "R_IA64_LTOFF_FPTR64I", R_IA64_LTOFF_FPTR64I}, 
    {"@ltoff(@fptr(s+a)), data4 MSB"	, "R_IA64_LTOFF_FPTR32MSB", R_IA64_LTOFF_FPTR32MSB}, 
    {"@ltoff(@fptr(s+a)), data4 LSB"	, "R_IA64_LTOFF_FPTR32LSB", R_IA64_LTOFF_FPTR32LSB}, 
    {"@ltoff(@fptr(s+a)), data8 MSB"	, "R_IA64_LTOFF_FPTR64MSB", R_IA64_LTOFF_FPTR64MSB}, 
    {"@ltoff(@fptr(s+a)), data8 LSB"	, "R_IA64_LTOFF_FPTR64LSB", R_IA64_LTOFF_FPTR64LSB}, 
    {"@segrel(sym + add), data4 MSB"	, "R_IA64_SEGREL32MSB"	, R_IA64_SEGREL32MSB}, 
    {"@segrel(sym + add), data4 LSB"	, "R_IA64_SEGREL32LSB"	, R_IA64_SEGREL32LSB}, 
    {"@segrel(sym + add), data8 MSB"	, "R_IA64_SEGREL64MSB"	, R_IA64_SEGREL64MSB}, 
    {"@segrel(sym + add), data8 LSB"	, "R_IA64_SEGREL64LSB"	, R_IA64_SEGREL64LSB}, 
    {"@secrel(sym + add), data4 MSB"	, "R_IA64_SECREL32MSB"	, R_IA64_SECREL32MSB}, 
    {"@secrel(sym + add), data4 LSB"	, "R_IA64_SECREL32LSB"	, R_IA64_SECREL32LSB}, 
    {"@secrel(sym + add), data8 MSB"	, "R_IA64_SECREL64MSB"	, R_IA64_SECREL64MSB}, 
    {"@secrel(sym + add), data8 LSB"	, "R_IA64_SECREL64LSB"	, R_IA64_SECREL64LSB}, 
    {"data 4 + REL"			, "R_IA64_REL32MSB"	, R_IA64_REL32MSB}, 
    {"data 4 + REL"			, "R_IA64_REL32LSB"	, R_IA64_REL32LSB}, 
    {"data 8 + REL"			, "R_IA64_REL64MSB"	, R_IA64_REL64MSB}, 
    {"data 8 + REL"			, "R_IA64_REL64LSB"	, R_IA64_REL64LSB}, 
    {"symbol + addend, data4 MSB"	, "R_IA64_LTV32MSB"	, R_IA64_LTV32MSB}, 
    {"symbol + addend, data4 LSB"	, "R_IA64_LTV32LSB"	, R_IA64_LTV32LSB}, 
    {"symbol + addend, data8 MSB"	, "R_IA64_LTV64MSB"	, R_IA64_LTV64MSB}, 
    {"symbol + addend, data8 LSB"	, "R_IA64_LTV64LSB"	, R_IA64_LTV64LSB}, 
    {"@pcrel(sym + add), 21bit inst"	, "R_IA64_PCREL21BI"	, R_IA64_PCREL21BI}, 
    {"@pcrel(sym + add), 22bit inst"	, "R_IA64_PCREL22"	, R_IA64_PCREL22}, 
    {"@pcrel(sym + add), 64bit inst"	, "R_IA64_PCREL64I"	, R_IA64_PCREL64I}, 
    {"dynamic reloc, imported PLT, MSB"	, "R_IA64_IPLTMSB"	, R_IA64_IPLTMSB}, 
    {"dynamic reloc, imported PLT, LSB"	, "R_IA64_IPLTLSB"	, R_IA64_IPLTLSB}, 
    {"copy relocation"			, "R_IA64_COPY"		, R_IA64_COPY}, 
    {"Addend and symbol difference"	, "R_IA64_SUB"		, R_IA64_SUB}, 
    {"LTOFF22, relaxable"		, "R_IA64_LTOFF22X"	, R_IA64_LTOFF22X}, 
    {"Use of LTOFF22X"			, "R_IA64_LDXMOV"	, R_IA64_LDXMOV}, 
    {"@tprel(sym + add), imm14"		, "R_IA64_TPREL14"	, R_IA64_TPREL14}, 
    {"@tprel(sym + add), imm22"		, "R_IA64_TPREL22"	, R_IA64_TPREL22}, 
    {"@tprel(sym + add), imm64"		, "R_IA64_TPREL64I"	, R_IA64_TPREL64I}, 
    {"@tprel(sym + add), data8 MSB"	, "R_IA64_TPREL64MSB"	, R_IA64_TPREL64MSB}, 
    {"@tprel(sym + add), data8 LSB"	, "R_IA64_TPREL64LSB"	, R_IA64_TPREL64LSB}, 
    {"@ltoff(@tprel(s+a)), imm2"	, "R_IA64_LTOFF_TPREL22", R_IA64_LTOFF_TPREL22}, 
    {"@dtpmod(sym + add), data8 MSB"	, "R_IA64_DTPMOD64MSB"	, R_IA64_DTPMOD64MSB}, 
    {"@dtpmod(sym + add), data8 LSB"	, "R_IA64_DTPMOD64LSB"	, R_IA64_DTPMOD64LSB}, 
    {"@ltoff(@dtpmod(sym + add)), imm22", "R_IA64_LTOFF_DTPMOD22", R_IA64_LTOFF_DTPMOD22}, 
    {"@dtprel(sym + add), imm14"	, "R_IA64_DTPREL14"	, R_IA64_DTPREL14},
    {"@dtprel(sym + add), imm22"	, "R_IA64_DTPREL22"	, R_IA64_DTPREL22}, 
    {"@dtprel(sym + add), imm64"	, "R_IA64_DTPREL64I"	, R_IA64_DTPREL64I}, 
    {"@dtprel(sym + add), data4 MSB"	, "R_IA64_DTPREL32MSB"	, R_IA64_DTPREL32MSB}, 
    {"@dtprel(sym + add), data4 LSB"	, "R_IA64_DTPREL32LSB"	, R_IA64_DTPREL32LSB}, 
    {"@dtprel(sym + add), data8 MSB"	, "R_IA64_DTPREL64MSB"	, R_IA64_DTPREL64MSB}, 
    {"@dtprel(sym + add), data8 LSB"	, "R_IA64_DTPREL64LSB"	, R_IA64_DTPREL64LSB}, 
    {"@ltoff(@dtprel(s+a)), imm22"	, "R_IA64_LTOFF_DTPREL22", R_IA64_LTOFF_DTPREL22}, 
  };


/** 
 * MIPS relocs 
 */
revmconst_t			elfsh_rel_type_mips[] =
  {

    {"No reloc"			,  "R_MIPS_NONE"	, R_MIPS_NONE},
    {"Direct 16 bit"		,  "R_MIPS_16"		, R_MIPS_16},
    {"Direct 32 bit"		,  "R_MIPS_32"		, R_MIPS_32},
    {"PC relative 32 bit"	,  "R_MIPS_REL32"	, R_MIPS_REL32},
    {"Direct 26 bit shifted"	,  "R_MIPS_26"		,  R_MIPS_26},
    {"High 16 bit"		,  "R_MIPS_HI16"	, R_MIPS_HI16},
    {"Low 16 bit"		,  "R_MIPS_LO16"	, R_MIPS_LO16},
    {"GP relative 16 bit"	,  "R_MIPS_GPREL16"	, R_MIPS_GPREL16},
    {"16 bit literal entry"	,  "R_MIPS_LITERAL"	, R_MIPS_LITERAL},
    {"16 bit GOT entry"		,  "R_MIPS_GOT16"	, R_MIPS_GOT16},
    {"PC relative 16 bit"	,  "R_MIPS_PC16"	,  R_MIPS_PC16},
    {"16 bit GOT entry for function",  "R_MIPS_CALL16"	, R_MIPS_CALL16},
    {"GP relative 32 bit"	,  "R_MIPS_GPREL32"	, R_MIPS_GPREL32}, 
    {""				, "R_MIPS_SHIFT5"	, R_MIPS_SHIFT5},
    {""				, "R_MIPS_SHIFT6"	, R_MIPS_SHIFT6},
    {""				, "R_MIPS_64"		, R_MIPS_64},
    {""				, "R_MIPS_GOT_DISP"	, R_MIPS_GOT_DISP},
    {""				, "R_MIPS_GOT_PAGE"	, R_MIPS_GOT_PAGE},
    {""				, "R_MIPS_GOT_OFST"	, R_MIPS_GOT_OFST},
    {""				, "R_MIPS_GOT_HI16"	, R_MIPS_GOT_HI16},
    {""				, "R_MIPS_GOT_LO16"	, R_MIPS_GOT_LO16},
    {""				, "R_MIPS_SUB"		, R_MIPS_SUB},
    {""				, "R_MIPS_INSERT_A"	, R_MIPS_INSERT_A},
    {""				, "R_MIPS_INSERT_B"	, R_MIPS_INSERT_B},
    {""				, "R_MIPS_DELETE"	, R_MIPS_DELETE},
    {""				, "R_MIPS_HIGHER"	, R_MIPS_HIGHER}, 
    {""				, "R_MIPS_HIGHEST"	, R_MIPS_HIGHEST}, 
    {""				, "R_MIPS_CALL_HI16"	, R_MIPS_CALL_HI16},
    {""				, "R_MIPS_CALL_LO16"	, R_MIPS_CALL_LO16},
    {""				, "R_MIPS_SCN_DISP"	, R_MIPS_SCN_DISP}, 
    {""				, "R_MIPS_REL16"	, R_MIPS_REL16},  
    {""				, "R_MIPS_ADD_IMMEDIATE", R_MIPS_ADD_IMMEDIATE},
    {""				, "R_MIPS_PJUMP"	, R_MIPS_PJUMP},
    {""				, "R_MIPS_RELGOT"	, R_MIPS_RELGOT},
    {""				, "R_MIPS_JALR"		, R_MIPS_JALR},
    
  };


/** 
 * Alpha relocs.  
 */
revmconst_t                    elfsh_rel_type_alpha[] =
  {
    {"No reloc"				, "R_ALPHA_NONE"	, R_ALPHA_NONE},           
    {"Direct 32 bit"			, "R_ALPHA_REFLONG"	, R_ALPHA_REFLONG},
    {"Direct 64 bit"			, "R_ALPHA_REFQUAD"	, R_ALPHA_REFQUAD},
    {"GP relative 32 bit"		, "R_ALPHA_GPREL32"	, R_ALPHA_GPREL32},
    {"GP relative 16 bit w/optimization", "R_ALPHA_LITERAL"	, R_ALPHA_LITERAL},
    {"Optimization hint for LITERAL"	, "R_ALPHA_LITUSE"	, R_ALPHA_LITUSE},
    {"Add displacement to GP"		, "R_ALPHA_GPDISP"	, R_ALPHA_GPDISP},          
    {"PC+4 relative 23 bit shifted"	, "R_ALPHA_BRADDR"	, R_ALPHA_BRADDR},
    {"PC+4 relative 16 bit shifted"	, "R_ALPHA_HINT"	, R_ALPHA_HINT},
    {"PC relative 16 bit"		, "R_ALPHA_SREL16"	, R_ALPHA_SREL16},
    {"PC relative 32 bit"		, "R_ALPHA_SREL32"	, R_ALPHA_SREL32},
    {"PC relative 64 bit"		, "R_ALPHA_SREL64"	, R_ALPHA_SREL64},
    {"OP stack push"			, "R_ALPHA_OP_PUSH"	, R_ALPHA_OP_PUSH},
    {"OP stack pop and store"		, "R_ALPHA_OP_STORE"	, R_ALPHA_OP_STORE},
    {"OP stack subtract"		, "R_ALPHA_OP_PSUB"	, R_ALPHA_OP_PSUB},
    {"OP stack right shift"		, "R_ALPHA_OP_PRSHIFT"	, R_ALPHA_OP_PRSHIFT},
    {"Unknown"				, "R_ALPHA_GPVALUE"	, R_ALPHA_GPVALUE},
    {"GP relative 32 bit, high 16 bits" , "R_ALPHA_GPRELHIGH"	, R_ALPHA_GPRELHIGH},
    {"GP relative 32 bit, low 16 bits"  , "R_ALPHA_GPRELLOW"	, R_ALPHA_GPRELLOW},
    {"GP relative 16 bit"		, "R_ALPHA_GPREL16"	, R_ALPHA_GPREL16},
    {"Unknown"				, "R_ALPHA_IMMED_GP_16"	  , R_ALPHA_IMMED_GP_16},
    {"Unknown"				, "R_ALPHA_IMMED_GP_HI32" , R_ALPHA_IMMED_GP_HI32},   
    {"Unknown"				, "R_ALPHA_IMMED_SCN_HI32", R_ALPHA_IMMED_SCN_HI32},  
    {"Unknown"				, "R_ALPHA_IMMED_BR_HI32" , R_ALPHA_IMMED_BR_HI32},   
    {"Unknown"				, "R_ALPHA_IMMED_LO32"	  , R_ALPHA_IMMED_LO32}, 
    {"Copy symbol at runtime"		, "R_ALPHA_COPY"	, R_ALPHA_COPY},
    {"Create GOT entry"			, "R_ALPHA_GLOB_DAT"	, R_ALPHA_GLOB_DAT},
    {"Create PLT entry"			, "R_ALPHA_JMP_SLOT"	, R_ALPHA_JMP_SLOT},
    {"Adjust by program base"		, "R_ALPHA_RELATIVE"	, R_ALPHA_RELATIVE},
    {"Unknown"				, "R_ALPHA_TLS_GD_HI"	, R_ALPHA_TLS_GD_HI},
    {"Unknown"				, "R_ALPHA_TLSGD"	, R_ALPHA_TLSGD},
    {"Unknown"				, "R_ALPHA_TLS_LDM"	, R_ALPHA_TLS_LDM},
    {"Unknown"				, "R_ALPHA_DTPMOD64"	, R_ALPHA_DTPMOD64},
    {"Unknown"				, "R_ALPHA_GOTDTPREL"	, R_ALPHA_GOTDTPREL},
    {"Unknown"				, "R_ALPHA_DTPREL64"	, R_ALPHA_DTPREL64},
    {"Unknown"				, "R_ALPHA_DTPRELHI"	, R_ALPHA_DTPRELHI},
    {"Unknown"				, "R_ALPHA_DTPRELLO"	, R_ALPHA_DTPRELLO},
    {"Unknown"				, "R_ALPHA_DTPREL16"	, R_ALPHA_DTPREL16},
    {"Unknown"				, "R_ALPHA_GOTTPREL"	, R_ALPHA_GOTTPREL},
    {"Unknown"				, "R_ALPHA_TPREL64"	, R_ALPHA_TPREL64},
    {"Unknown"				, "R_ALPHA_TPRELHI"	, R_ALPHA_TPRELHI},
    {"Unknown"				, "R_ALPHA_TPRELLO"	, R_ALPHA_TPRELLO},
    {"Unknown"				, "R_ALPHA_TPREL16"	, R_ALPHA_TPREL16},
  };
    
   

/** 
 * Relocation types strings for SPARC 
 */
revmconst_t                    elfsh_rel_type_sparc[] = 
{
  {"No relocation"                  , "R_SPARC_NONE"      , R_SPARC_NONE},
  {"Direct 8 bit"                   , "R_SPARC_8"	  , R_SPARC_8},	
  {"Direct 16 bit"                  , "R_SPARC_16"        , R_SPARC_16},
  {"Direct 32 bit"                  , "R_SPARC_32"        , R_SPARC_32},
  {"PC relative 8 bit"              , "R_SPARC_DISP8"     , R_SPARC_DISP8},
  {"PC relative 16 bit"             , "R_SPARC_DISP16"    , R_SPARC_DISP16},
  {"PC relative 32 bit"             , "R_SPARC_DISP32"    , R_SPARC_DISP32},
  {"PC relative 30 bit shifted"     , "R_SPARC_WDISP30"   , R_SPARC_WDISP30},
  {"PC relative 22 bit shifted"     , "R_SPARC_WDISP22"   , R_SPARC_WDISP22},
  {"High 22 bit"                    , "R_SPARC_HI22"      , R_SPARC_HI22},
  {"Direct 22 bit"                  , "R_SPARC_22"        , R_SPARC_22},
  {"Direct 13 bit"                  , "R_SPARC_13"        , R_SPARC_13},
  {"Truncated 10 bit"               , "R_SPARC_LO10"      , R_SPARC_LO10},
  {"Truncated 10 bit GOT entry"     , "R_SPARC_GOT10"     , R_SPARC_GOT10},
  {"13 bit GOT entry"               , "R_SPARC_GOT13"     , R_SPARC_GOT13},
  {"22 bit GOT entry shifted"       , "R_SPARC_GOT22"     , R_SPARC_GOT22},
  {"PC relative 10 bit truncated"   , "R_SPARC_PC10"      , R_SPARC_PC10},
  {"PC relative 22 bit shifted"     , "R_SPARC_PC22"      , R_SPARC_PC22},
  {"30 bit PC relative PLT address" , "R_SPARC_WPLT30"    , R_SPARC_WPLT30},
  {"Copy symbol at runtime"         , "R_SPARC_COPY"      , R_SPARC_COPY},
  {"Create GOT entry"               , "R_SPARC_GLOB_DAT"  , R_SPARC_GLOB_DAT},
  {"Create PLT entry"               , "R_SPARC_JMP_SLOT"  , R_SPARC_JMP_SLOT},
  {"Adjust by program base"         , "R_SPARC_RELATIVE"  , R_SPARC_RELATIVE}, 
  {"Direct 32 bit unaligned"        , "R_SPARC_UA32"      , R_SPARC_UA32},
  {"Direct 32 bits ref to PLT entry"    , "R_SPARC_PLT32"   , R_SPARC_PLT32},
  {"High 22 bits PLT entry"             , "R_SPARC_HIPLT22" , R_SPARC_HIPLT22},
  {"Truncated 10 bits PLT entry"        , "R_SPARC_LOPLT10" , R_SPARC_LOPLT10},
  {"PC rel 32 bits ref to PLT entry"    , "R_SPARC_PCPLT32" , R_SPARC_PCPLT32},
  {"PC rel high 22 bits PLT entry"      , "R_SPARC_PCPLT22" , R_SPARC_PCPLT22},
  {"PC rel trunc 10 bits PLT entry"     , "R_SPARC_PCPLT10" , R_SPARC_PCPLT10},
  {"Direct 10 bits"                     , "R_SPARC_10"      , R_SPARC_10},
  {"Direct 11 bits"                     , "R_SPARC_11"      , R_SPARC_11},
  {"Direct 64 bits"                     , "R_SPARC_64"      , R_SPARC_64},
  {"10bit with secondary 13 bits addend", "R_SPARC_OLO10"   , R_SPARC_OLO10},
  {"Top 22 bits of direct 64 bits"      , "R_SPARC_HH22"    , R_SPARC_HH22},
  {"High middle 10 bits"                , "R_SPARC_HM10"    , R_SPARC_HM10},
  {"Low middle 22 bits"                 , "R_SPARC_LM22"    , R_SPARC_LM22},
  {"Top 22 bits of pc rel 64 bits"      , "R_SPARC_PC_HH22" , R_SPARC_PC_HH22},
  {"High middle 10 bits"                , "R_SPARC_PC_HM10" , R_SPARC_PC_HM10},
  {"Low miggle 22 bits"                 , "R_SPARC_PC_LM22" , R_SPARC_PC_LM22},
  {"PC relative 16 bits shifted"        , "R_SPARC_WDISP16" , R_SPARC_WDISP16},
  {"PC relative 19 bits shifted"        , "R_SPARC_WDISP19" , R_SPARC_WDISP19},
  {"Direct 7 bits"                      , "R_SPARC_7"       , R_SPARC_7},
  {"Direct 5 bits"                      , "R_SPARC_5"       , R_SPARC_5},
  {"Direct 6 bits"                      , "R_SPARC_6"       , R_SPARC_6},
  {"PC relative 64 bits"                , "R_SPARC_DISP64"  , R_SPARC_DISP64},
  {"Direct 64 bits ref to PLT entry"    , "R_SPARC_PLT64"   , R_SPARC_PLT64},
  {"High 22 bits complemented"          , "R_SPARC_HIX22"   , R_SPARC_HIX22},
  {"Truncated 11 bits complemented"     , "R_SPARC_LOX10"   , R_SPARC_LOX10},
  {"Direct high 12 of 44 bits"          , "R_SPARC_H44"     , R_SPARC_H44},
  {"Direct mid 22 of 44 bits"           , "R_SPARC_M44"     , R_SPARC_M44},
  {"Direct low 10 of 44 bits"           , "R_SPARC_L44"     , R_SPARC_L44},
  {"Global register usage"              , "R_SPARC_REGISTER", R_SPARC_REGISTER},
  {"Direct 64 bits unaligned"           , "R_SPARC_UA64"    , R_SPARC_UA64},
  {"Direct 16 bits unaligned"           , "R_SPARC_UA16"    , R_SPARC_UA16},
};




/** 
 * Architecture strings 
 */
revmconst_t		   elfsh_arch_type[] = 
{
  {"No machine"           , "EM_NONE", EM_NONE},
  {"AT&T WE 32100"        , "EM_M32", EM_M32},
  {"SUN SPARC"            , "EM_SPARC", EM_SPARC},
  {"Intel 80386"          , "EM_386", EM_386},
  {"Motorola m68k family" , "EM_68K", EM_68K},
  {"Motorola m88k family" , "EM_88K", EM_88K},
  {"Intel 80486"          , "EM_486", EM_486},
  {"Intel 80860"          , "EM_860", EM_860},
  {"MIPS R3000 BE"        , "EM_MIPS", EM_MIPS},
  {"IBM system 370"       , "EM_S370", EM_S370},
  {"MIPS R3000 LE"        , "EM_MIPS_R3000", EM_MIPS_RS3_LE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"HPPA"                 , "EM_PARISC", EM_PARISC},
  {"nCUBE"                , "EM_NCUBE", EM_NCUBE},
  {"Fujitsu VPP500"       , "EM_VPP500", EM_VPP500},
  {"Sun's v8plus"         , "EM_SPARC32PLUS", EM_SPARC32PLUS},
  {"Intel 80960"          , "EM_960", EM_960},
  {"PowerPC"              , "EM_PPC", EM_PPC},
  {"PowerPC 64 bits"      , "EM_PPC64", EM_PPC64},
  {"IBM S390"             , "EM_S390", EM_S390},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"Unknown"              , "EM_NONE", EM_NONE},
  {"NEC V800 series"      , "EM_V800", EM_V800},
  {"Fujitsu FR20"         , "EM_FR20", EM_FR20},
  {"TRW RH32"             , "EM_RH32", EM_RH32},
  {"Motorola RCE"         , "EM_RCE", EM_RCE},
  {"ARM"                  , "EM_ARM", EM_ARM},
  {"Digital Alpha"        , "EM_FAKE_ALPHA", EM_FAKE_ALPHA},
  {"Hitachi SH"           , "EM_SH", EM_SH},
  {"SPARC v9 64-bit"      , "EM_SPARCV9", EM_SPARCV9},
  {"Siemens Tricore"      , "EM_TRICORE", EM_TRICORE},
  {"Argonaut RISC Core"   , "EM_ARC", EM_ARC},
  {"Hitachi H8/300"       , "EM_H8_300", EM_H8_300},
  {"Hitachi H8/300H"      , "EM_H8_300H", EM_H8_300H},
  {"Hitachi H8S"          , "EM_H8S", EM_H8S},
  {"Hitachi H8/500"       , "EM_H8_500", EM_H8_500},
  {"Intel Merced"         , "EM_IA_64", EM_IA_64},
  {"Stanford MIPS-X"      , "EM_MIPS_X", EM_MIPS_X},
  {"Motorola Coldfire"    , "EM_COLDFIRE", EM_COLDFIRE},
  {"Motorola M68HC12"     , "EM_68HC12", EM_68HC12},
  {"Fujitsu MMA Accelerator", "EM_MMA", EM_MMA},
  {"Siemens PCP ", "EM_PCP", EM_PCP},
  {"Sony nCPU embeeded RISC ", "EM_NCPU", EM_NCPU},
  {"Denso NDR1 microprocessor ", "EM_NDR1", EM_NDR1},
  {"Motorola Start*Core processor ", "EM_STARCORE", EM_STARCORE},
  {"Toyota ME16 processor ", "EM_ME16", EM_ME16},
  {"STMicroelectronic ST100 processor ", "EM_ST100", EM_ST100},
  {"Advanced Logic Corp. Tinyj emb.fam", "EM_TINYJ", EM_TINYJ},
  {"AMD x86-64 architecture ", "EM_X86_64", EM_X86_64},
  {"Sony DSP Processor ", "EM_PDSP", EM_PDSP},
  {"Unknown"          , "EM_NONE", EM_NONE},
  {"Unknown"          , "EM_NONE", EM_NONE},
  {"Siemens FX66 MCU ", "EM_FX66", EM_FX66},
  {"STMicroelectronics ST9+ 8/16 mc ", "EM_ST9PLUS", EM_ST9PLUS},
  {"STmicroelectronics ST7 8 bit mc ", "EM_ST7", EM_ST7},
  {"Motorola MC68HC16 MCU ", "EM_68HC16", EM_68HC16},
  {"Motorola MC68HC11 MCU ", "EM_68HC11", EM_68HC11},
  {"Motorola MC68HC08 MCU ", "EM_68HC08", EM_68HC08},
  {"Motorola MC68HC05 MCU ", "EM_68HC05", EM_68HC05},
  {"Silicon Graphics SVx ", "EM_SVX", EM_SVX},
  {"STMicroelectronics ST19 8 bit mc ", "EM_ST19", EM_ST19},
  {"Digital VAX ", "EM_VAX", EM_VAX},
  {"Axis Communications 32-bit embedded processor ", "EM_CRIS", EM_CRIS},
  {"Infineon Technologies 32-bit embedded processor ", "EM_JAVELIN", EM_JAVELIN},
  {"Element 14 64-bit DSP Processor ", "EM_FIREPATH", EM_FIREPATH},
  {"LSI Logic 16-bit DSP Processor ", "EM_ZSP", EM_ZSP},
  {"Donald Knuth's educational 64-bit processor ", "EM_MMIX", EM_MMIX},
  {"Harvard University machine-independent object files ", "EM_HUANY", EM_HUANY},
  {"SiTera Prism ", "EM_PRISM", EM_PRISM},
  {"Atmel AVR 8-bit MCU ", "EM_AVR", EM_AVR},
  {"Fujitsu FR30 ", "EM_FR30", EM_FR30},
  {"Mitsubishi D10V ", "EM_D10V", EM_D10V},
  {"Mitsubishi D30V ", "EM_D30V", EM_D30V},
  {"NEC v850 ", "EM_V850", EM_V850},
  {"Mitsubishi M32R ", "EM_M32R", EM_M32R},
  {"Matsushita MN10300 ", "EM_MN10300", EM_MN10300},
  {"Matsushita MN10200 ", "EM_MN10200", EM_MN10200},
  {"picoJava ", "EM_PJ", EM_PJ},
  {"OpenRISC 32-bit embedded processor ", "EM_OPENRISC", EM_OPENRISC},
  {"ARC Cores Tangent-A5 ", "EM_ARC_A5", EM_ARC_A5},
  {"Tensilica Xtensa Architecture ", "EM_XTENSA", EM_XTENSA},
};
       

/** 
 * Stab entry type strings 
 */
char		*elfsh_stab_type[] = 
{
  "Undefined symbol", 
  "Unknown",
  "File scope absolute symbol",
  "External absolute symbol", 
  "File scope text symbol", 
  "External text symbol", 
  "File scope data symbol", 
  "External data symbol", 
  "File scope BSS symbol", 
  "External BSS symbol", 
  "Symbol is indirected to another symbol",
  "Unknown",
  "Same as N_FN, for Sequent compilers",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Common--visible after shared library dynamic link",
  "Unknown",
  "Unknown",
  "Absolute set element", 
  "Unknown",
  "Text segment set element",
  "Unknown",
  "Data segment set element",
  "Unknown",
  "BSS segment set element",
  "Unknown",
  "Pointer to set vector",
  "Print a warning message during linking",
  "File name of a module",
  "Global symbol",
  "Unknown",
  "Function name (for BSD Fortran)",
  "Unknown",
  "Function name (see section 2.5 Procedures) or text segment variable",
  "Unknown",
  "Data segment file-scope variable",
  "Unknown",
  "BSS segment file-scope variable",
  "Unknown",
  "Name of main routine",
  "Unknown",
  "Variable in .rodata section",
  "Unknown",
  "Unknown",
  "Unknown",
  "Global symbol (for Pascal)",
  "Unknown",
  "Number of symbols (according to Ultrix V4.0)",
  "Unknown",
  "No DST map",
  "Unknown",
  "Unknown",
  "Unknown",
  "Object file (Solaris2)", 
  "Unknown",
  "Unknown",
  "Unknown",
  "Debugger options (Solaris2)", 
  "Unknown",
  "Unknown",
  "Unknown",
  "Register variable",
  "Unknown",
  "Modula-2 compilation unit",
  "Unknown",
  "Line number in text segment",
  "Unknown",
  "Line number in data segment",
  "Unknown",
  "Line number in bss segment",
  "Unknown",
  "GNU Modula2 definition module dependency",
  "Unknown",
  "Function start/body/end line numbers (Solaris2)",
  "Unknown",
  "Unknown",
  "Unknown",
  "GNU C++ exception variable",
  "Unknown",
  "Unknown",
  "Unknown",
  "GNU C++ catch clause",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Structure of union element",
  "Unknown",
  "Last stab for module (Solaris2)",
  "Unknown",
  "Path and name of source file",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Stack variable",
  "Unknown",
  "Beginning of an include file (Sun only)",
  "Unknown",
  "Name of include file",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Parameter variable",
  "Unknown",
  "End of an include file",
  "Unknown",
  "Alternate entry point",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Beginning of a lexical block",
  "Unknown",
  "Place holder for a deleted include file",
  "Unknown",
  "Modula2 scope information (Sun linker)",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",  
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "End of a lexical block",
  "Unknown",
  "Begin named common block",
  "Unknown",
  "End named common block",
  "Unknown",
  "Unknown",
  "Unknown",
  "Member of a common block",
  "Unknown",
  "Pascal with statement: type,,0,0,offset (Solaris2)",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Gould non-base registers",
  "Unknown",
  "Gould non-base registers",
  "Unknown",
  "Gould non-base registers",
  "Unknown",
  "Gould non-base registers",
  "Unknown",
  "Gould non-base registers",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
  "Unknown",
};




/**
 *  Control flow analysis link types
 */
revmconst_t			elfsh_link_type[] = 
{
  {"", "FUNC_CALL"    , MJR_LINK_FUNC_CALL},
  {"", "FUNC_RET"     , MJR_LINK_FUNC_RET},
  {"", "BLOC_TRUE"    , MJR_LINK_BLOCK_COND_TRUE},
  {"", "BLOC_FALSE"   , MJR_LINK_BLOCK_COND_FALSE},
  {"", "BLOC_DELAY"   , MJR_LINK_TYPE_DELAY},
  {"", "BLOC_ALWAYS"  , MJR_LINK_BLOCK_COND_ALWAYS},
  {"", "LINK_UNKNOW"  , MJR_LINK_UNKNOWN},
};


/**
 *  Control flow analysis link scopes
 */
revmconst_t			elfsh_link_scope[] = 
{
  {"", "SCOPE_UNKNOW"   , MJR_LINK_SCOPE_UNKNOWN},
  {"", "SCOPE_LOCAL"    , MJR_LINK_SCOPE_LOCAL},
  {"", "SCOPE_GLOBAL"   , MJR_LINK_SCOPE_GLOBAL},
};








