/*
** sparc64.c for libelfsh
**
** Started on  Sat Jan 15 14:25:37 2005 mayhem
** Last update Sat Jan 15 14:26:39 2005 mayhem
**
** Cut & Pasted from the sparc32 backend
** 64 bits backend work-in-progress
**
*/
#include "libelfsh.h"





/*
  Here some temporary info on openssl64

  printf : 0x100255560 (plt + 672)
*/



/* Static hooking for Sparc64 */
int	elfsh_cflow_sparc64(elfshobj_t  *null,
			    char	*snull,
			    elfsh_Sym	*null2,
			    elfsh_Addr	null3)
{
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}



/* PLT hijacking on SPARC64 */
int		elfsh_hijack_plt_sparc64(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 elfsh_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[3];
  
  if (file->hdr->e_machine != EM_SPARCV9)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
		   "ELFSH_HIJACK_CPU_SPARC while the elf file is not "
		   "SPARC\n", -1);

  /* compute the sparc %hi(), %lo() address */
  addrh = addr & 0xfffffc00;
  addrl = addr & 0x3ff;

  /* sethi %hi(addrh), %g1	*/
  opcode[0] = 0x03000000 | addrh >> 10;

  /* jmp %g1 + addrl	! addr	*/
  opcode[1] = 0x81c06000 | addrl;

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
   If we dont do that, the hook is removed after the first original (hijacked) 
   function call, and thats not what we want (in case we need to call the original
   function from the hook function.) -mm

   Optimized by dvorak at synnergy dot net for fitting in 64 bytes, danke Jack!

*/
int		elfsh_hijack_altplt_sparc64(elfshobj_t *file, 
					    elfsh_Sym *symbol,
					    elfsh_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[11];

  if (file->hdr->e_machine != EM_SPARCV9)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
		   "ELFSH_HIJACK_CPU_SPARC while the elf file is not "
		   "SPARC\n", -1);

  addr += 4;

  /* save %sp, -64, %sp */
  opcode[0] = 0x9de3bfc0;

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




/* Perform relocation on entry for SPARC64 architecture */
/* Shared relocation function with augmented SPARC32 */
int       elfsh_relocate_sparc64(elfshsect_t       *new,
				 elfsh_Rela        *cur,
				 elfsh_Addr        *dword,
				 elfsh_Addr        addr,
				 elfshsect_t	   *mod)
{
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_relocate_sparc32(new, cur, dword, addr, mod)));
}


