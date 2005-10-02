/*
** sparc32.c for libelfsh
**
** Started on  Fri Jan 11 03:05:37 2003 mayhem
**
*/
#include "libelfsh.h"



/* Static hooking for Sparc */
int			elfsh_cflow_sparc32(elfshobj_t  *file,
					    char	*name,
					    elfsh_Sym	*symbol,
					    elfsh_Addr  addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "CFLOW unimplemented yet for SPARC", -1);
}



/* PLT hijacking on SPARC */
/* Based on a early version by emsi, delay slot added by ELFsh crew, now it's works yes */
/* Slight patch by emsi to support function call when the high bit is set */
int		elfsh_hijack_plt_sparc32(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 elfsh_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[3];
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_machine != EM_SPARC && file->hdr->e_machine != EM_SPARC32PLUS)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
		   "ELFSH_HIJACK_CPU_SPARC while the elf file is not "
		   "SPARC\n", -1);

  /* compute the sparc %hi(), %lo() address */
  addrh = addr & 0xfffffc00;                                                             
  addrl = addr & 0x3ff;

  /* sethi %hi(addrh), %g1      */
  opcode[0] = 0x03000000 | (0x3fffff & (addrh >> 10));
  
  /* jmp %g1 + addrl    ! addr  */
  opcode[1] = 0x81c06000 | addrl;
  
  /* Add a nop for delay slot */
  opcode[2] = 0x01000000;
  
  foffset = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  elfsh_raw_write(file, foffset, opcode, 3 * sizeof(uint32_t));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* PLT hijacking on SPARC */
/* Based on a early version by emsi, delay slot added by ELFsh crew, now it's works yes */
/* Slight patch by emsi to support function call when the high bit is set */
int		elfsh_hijack_plt_sparc32_second(elfshobj_t *file, 
						elfsh_Sym *symbol,
						elfsh_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[3];
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_machine != EM_SPARC && 
      file->hdr->e_machine != EM_SPARC32PLUS)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
		      "ELFSH_HIJACK_CPU_SPARC while the elf file is not "
		      "SPARC\n", -1);

  /* compute the sparc %hi(), %lo() address */
  addrh = addr & 0xfffffc00;                                                             
  addrl = addr & 0x3ff;

  /* sethi %hi(addrh), %g2      */
  opcode[0] = 0x05000000 | (0x3fffff & (addrh >> 10));
  
  /* jmp %g1 + addrl    ! addr  */
  opcode[1] = 0x81c0a000 | addrl;
  
  /* Add a nop for delay slot */
  opcode[2] = 0x01000000;
  
  foffset = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  elfsh_raw_write(file, foffset, opcode, 3 * sizeof(uint32_t));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* 

   Used for hijacking the first PLT entry, when %g1 needs to be kept.
   This 16 instructions lenght code copies the first updated .plt
   entry to the first not-yet-updated .alt.plt entry in runtime. This
   operation needs to recompute the 'call elf_bndr' since its operand is
   relative. 

   This fix is necessary so that we keep a consistent relocation offset
   (because first .alt.plt entry 's %i7 is used for computing the relocation 
   offset on solaris/sparc, I guess this is not a very standard behavior ;).
   
   Updated 20050311 : Alpha does it as well, not respecting the ELF reference
   seems to be a normal behavior. Yeah, direct PLT patching sucks.
 
   If we just try to redirect .alt.plt first entry on .plt first entry, it will
   work the first time, but the hook is removed after the first original (hijacked) 
   function call, and thats not what we want (in case we need to call the original
   function from the hook function many times). That's the main reason why we
   need this cool stub -mm

   Optimized by dvorak for fitting in 96 bytes (12 instrs) danke Jack !

*/
int		elfsh_hijack_altplt_sparc32(elfshobj_t *file, 
					    elfsh_Sym *symbol,
					    elfsh_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[11];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_machine != EM_SPARC && file->hdr->e_machine != EM_SPARC32PLUS)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
		   "ELFSH_HIJACK_CPU_SPARC while the elf file is not "
		   "SPARC\n", -1);

  {
    elfsh_Sym	*sym;

    sym = elfsh_get_symbol_by_value(file, addr, 0, ELFSH_EXACTSYM);
    if (!sym)
ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot find .plt symbol .. ", -1);

    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_hijack_plt_sparc32_second(file, sym, symbol->st_value)));
  }


  //Not needed anymore ?
  addr += 4;

  /* save %sp, -64, %sp */
  //opcode[0] = 0x9de3bfc0;

  /* save %sp, -88, %sp */
  opcode[0] = 0x9de3bfa8;

  /* mloop: call  0x10740 <x> */
  opcode[1] = 0x40000002;

  /* sethi  %hi(0x20800), %l2 */
  addrh = addr & 0xfffffc00;
  opcode[2] = 0x25000000 | addrh >> 10;

  /* x: or  %l2, 0x1ac, %l2 */
  addrl = addr & 0x3ff;
  opcode[3] = 0xA414a000 | addrl;

  /* sub  %o7, %l2, %l3 */
  opcode[4] = 0xa623c012;

  /* sra  %l3, 2, %l3 */
  opcode[5] = 0xa73ce002;

  /* ld  [ %l2 ], %l4 */
  opcode[6] = 0xe804a000;

  /* sub %l4, %l3, %l4 */
  opcode[7] = 0xa8250013;

  /* st  %l4, [ %o7 ] */
  opcode[8] = 0xe823e000;

  /* ld  [ %l2 + 8 ], %l3 */
  opcode[9] = 0xe604a008;

  /* b  0x10738 <mloop> */
  opcode[10] = 0x10bffff7;

  /* st  %l3, [ %o7 + 8 ] */
  opcode[11] = 0xe623e008;

  foffset = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  elfsh_raw_write(file, foffset, opcode, 12 * sizeof(uint32_t));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Perform relocation on entry for SPARC architecture */
/* First version by thegrugq adapted by elfsh crew */
int       elfsh_relocate_sparc32(elfshsect_t       *new,
				 elfsh_Rela        *cur,
				 elfsh_Addr        *dword,
				 elfsh_Addr         addr,
				 elfshsect_t	   *mod)		
{
  int		retval;
  elfsh_Addr	result;
  uint32_t	*word;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#define ADD     (cur->r_addend)
#define BAS     (elfsh_get_object_baseaddr(new->parent))
#define GOT     0 /* GOT isn't available ... */
#define PLT     0 /* PLT isn't available ... */
#define PLA     (new->shdr->sh_addr + cur->r_offset)
#define SYM     (addr)

#define VERIFY8(x)      (((x) & (0xFFFFFFFF <<  8)) ? 1 : 0)
#define VERIFY13(x)     (((x) & (0xFFFFFFFF << 13)) ? 1 : 0)
#define VERIFY16(x)     (((x) & (0xFFFFFFFF << 16)) ? 1 : 0)
#define VERIFY22(x)     (((x) & (0xFFFFFFFF << 22)) ? 1 : 0)
#define VERIFY30(x)     (((x) & (0xFFFFFFFF << 30)) ? 1 : 0)
#define VERIFY32(x)     (((x) & (0xFFFFFFFF <<  0)) ? 1 : 0)

#define TRUNCATE8(x)    ((x) & ~(0xFFFFFFFF <<  8))
#define TRUNCATE13(x)   ((x) & ~(0xFFFFFFFF << 13))
#define TRUNCATE16(x)   ((x) & ~(0xFFFFFFFF << 16))
#define TRUNCATE22(x)   ((x) & ~(0xFFFFFFFF << 22))
#define TRUNCATE30(x)   ((x) & ~(0xFFFFFFFF << 30))
#define TRUNCATE32(x)   (x) /* XXX just assume its 32bit */

  retval = 0;
  result = 0;
  word = (uint32_t *) dword;

  switch (elfsh_get_reltype((elfsh_Rel *)cur))
    {
    case R_SPARC_NONE:
      break;
    case R_SPARC_8:
      result = SYM + ADD;
      if (VERIFY8(result))
	; // XXX error condition.
      break;
    case R_SPARC_16:
      result = SYM + ADD;
      if (VERIFY16(result))
	; // XXX error condition
      break;
    case R_SPARC_32:
      result = SYM + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_DISP8:
      result = SYM + ADD - PLA;
      if (VERIFY8(result))
	; // XXX error condition
      break;
    case R_SPARC_DISP16:
      result = SYM + ADD - PLA;
      if (VERIFY16(result))
	; // XXX error condition
      break;
    case R_SPARC_DISP32:
      result = SYM + ADD - PLA;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_WDISP30:
      result = (SYM + ADD - PLA) >> 2;
      if (VERIFY30(result))
	; // XXX error condition
      break;
    case R_SPARC_WDISP22:
      result = (SYM + ADD - PLA) >> 10;
      if (VERIFY22(result))
	; // XXX error condition
      break;
    case R_SPARC_HI22:
      result = TRUNCATE22(((SYM + ADD) >> 10));
      break;
    case R_SPARC_22:
      result = SYM + ADD;
      if (VERIFY22(result))
	; // XXX error condition
      break;
    case R_SPARC_13:
      result = SYM + ADD;
      if (VERIFY13(result))
	; // XXX error condition
      break;
    case R_SPARC_LO10:
      result = TRUNCATE13(((SYM + ADD) & 0x3FF));
      break;
    case R_SPARC_GOT10:
    case R_SPARC_GOT13:
    case R_SPARC_GOT22:
      // unsupported
      retval = -1;
      break;
    case R_SPARC_PC10:
      result = TRUNCATE13(((SYM + ADD - PLA) & 0x3FF));
      break;
    case R_SPARC_PC22:
      result = (SYM + ADD - PLA) >> 10;
      if (VERIFY22(result))
	; // XXX error condition
      break;
    case R_SPARC_WPLT30:
      // unsupported
      retval = -1;
      break;
    case R_SPARC_COPY:
      // unsupported
      retval = -1;
      break;
    case R_SPARC_GLOB_DAT:
      result = SYM + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_JMP_SLOT:
      //unsupported
      retval = -1;
      break;
    case R_SPARC_RELATIVE:
      result = BAS + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    case R_SPARC_UA32:
      result = SYM + ADD;
      if (VERIFY32(result))
	; // XXX error condition
      break;
    default:
ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unsupported reloc type",
		     -1);
      break;
    }

#undef ADD
#undef BAS
#undef GOT
#undef PLT
#undef PLA
#undef SYM
  *word += (uint32_t) result;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
}


