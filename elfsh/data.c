/*
** data.c for elfsh
** 
** Started on  Mon Feb 26 04:06:38 2001 mayhem
** Last update Mon Jun 23 05:47:27 2003 mayhem
*/
#include "elfsh.h"





/* Program header (segment) entry strings */
elfshconst_t			elfsh_seg_type[] = 
{
  {"NULL segment"         , "PT_NULL"   , PT_NULL},
  {"Loadable segment"     , "PT_LOAD"   , PT_LOAD},
  {"Dynamic linking info" , "PT_DYNAMIC", PT_DYNAMIC},
  {"Program interpreter"  , "PT_INTERP" , PT_INTERP},
  {"Auxiliary information", "PT_NOTE"   , PT_NOTE},
  {"Reserved"             , "PT_SHLIB"  , PT_SHLIB},
  {"Program header table" , "PT_PHDR"   , PT_PHDR},
};


/* Encoding type in ELF header */
elfshconst_t			elfsh_encoding[] = 
{
  {"Invalid encoding", "ELFDATANONE", ELFDATANONE},
  {"Little endian"   , "ELFDATA2LSB", ELFDATA2LSB},
  {"Big endian"      , "ELFDATA2MSB", ELFDATA2MSB},
};


/* Section type strings */
elfshconst_t			elfsh_sh_type[] = 
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
};


/* ELF object type strings */
elfshconst_t			elfsh_obj_type[] = 
{
  {"Unknown"               , "ET_NONE", ET_NONE},
  {"Relocatable object"    , "ET_REL" , ET_REL},
  {"Executable object"     , "ET_EXEC", ET_EXEC},
  {"Shared object"         , "ET_DYN" , ET_DYN},
  {"Core file"             , "ET_CORE", ET_CORE},
};

/* Symbol Binding strings */
elfshconst_t			elfsh_sym_bind[] = 
{
  {"Local"            , "STB_LOCAL"  , STB_LOCAL},
  {"Global"           , "STB_GLOBAL" , STB_GLOBAL},
  {"Weak"             , "STB_WEAK"   , STB_WEAK},
};


/* Symbol Type strings */
elfshconst_t			elfsh_sym_type[] = 
{
  {"NOTYPE"  , "STT_NOTYPE" , STT_NOTYPE},
  {"VARIABLE", "STT_OBJECT" , STT_OBJECT},
  {"FUNCTION", "STT_FUNC"   , STT_FUNC},
  {"SECTION" , "STT_SECTION", STT_SECTION},
  {"FILENAME", "STT_FILE"   , STT_FILE},
  {"COMMON"  , "STT_COMMON" , STT_COMMON},
  {"BLOCK"   , "STT_BLOCK"  , STT_BLOCK},
};



/* Dynamic section entry type strings */
elfshconst_t			elfsh_dynentry_type[] = 
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


/* Extended dynamic types */
elfshconst_t			elfsh_extdyn_type[] = 
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



/* The next 4 arrays are special flag based DT entries */
/* Enumeration of '<< 1' increment */
elfshconst_t			elfsh_feature1[] = 
{
  {"Option 1: PARINIT(?)", "DTF_1_PARINIT", DTF_1_PARINIT},
  {"Option 2: CONFEXP(?)", "DTF_1_CONFEXP", DTF_1_CONFEXP},
};

elfshconst_t			elfsh_posflag1[] = 
{
  {"Lazyload following object"              , "DF_P1_LAZYLOAD" , DF_P1_LAZYLOAD},
  {"Next object symbols maybe not available", "DF_P1_GROUPPERM", DF_P1_GROUPPERM},
};

elfshconst_t			elfsh_flags[] = 
{
  {"Object may use DF_ORIGIN"        , "DF_ORIGIN"  , DF_ORIGIN},
  {"Symbol resolutions starts here"  , "DF_SYMBOLIC", DF_SYMBOLIC},	
  {"Object contains text relocations", "DF_TEXTREL" , DF_TEXTREL},
  {"No lazy binding for this object" , "DF_BIND_NOW", DF_BIND_NOW},
};

elfshconst_t			elfsh_flags1[] = 
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



/* Relocation types strings */
elfshconst_t			elfsh_rel_type_i386[] = 
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

/* Relocation types strings for SPARC */
elfshconst_t                    elfsh_rel_type_sparc[] = 
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
};



/* Architecture strings */
char			*elfsh_arch_type[] = 
{
  "No machine"           ,
  "AT&T WE 32100"        ,
  "SUN SPARC"            , 
  "Intel 80386"          ,
  "Motorola m68k family" ,
  "Motorola m88k family" ,
  "Intel 80486"          ,
  "Intel 80860"          ,
  "MIPS R3000 big-endian",
  "Amdahl"               ,
  "MIPS R4000 big-endian",
  "RS6000"               ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "HPPA"                 ,
  "nCUBE"                ,
  "Fujitsu VPP500"       ,
  "Sun's v8plus"         ,
  "Intel 80960"          ,
  "PowerPC"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,   
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "Unknown"              ,
  "NEC V800 series"      ,
  "Fujitsu FR20"         ,
  "TRW RH32"             ,
  "Fujitsu MMA"          ,
  "ARM"                  ,
  "Digital Alpha"        ,
  "Hitachi SH"           ,
  "SPARC v9 64-bit"      ,
  "Siemens Tricore"      ,
  "Argonaut RISC Core"   ,
  "Hitachi H8/300"       ,
  "Hitachi H8/300H"      ,
  "Hitachi H8S"          ,
  "Hitachi H8/500"       ,
  "Intel Merced"         ,
  "Stanford MIPS-X"      ,
  "Motorola Coldfire"    ,
  "Motorola M68HC12"     ,
};


/* Stab entry type strings */
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












