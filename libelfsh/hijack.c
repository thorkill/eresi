/*
** hijack.c in elfsh
** 
** Started on  Tue Feb  4 14:41:34 2003 emsi
** Last update Thu Jun 26 09:05:20 2003 mayhem
*/
#include "libelfsh.h"


/* PLT hijacking on SPARC */
int		elfsh_hijack_plt_sparc(elfshobj_t *file, 
				       Elf32_Sym *symbol,
				       Elf32_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[3];
  
  if (file->hdr->e_machine != ELFSH_HIJACK_CPU_SPARC)
    ELFSH_SETERROR("[libelfsh:hijack_plt_sparc] requested "
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
  return (0);
}

/* PLT hijacking on SPARC using %g2 instead of %g1 */
/* Used for hijacking of first PLT entry, when %g1 needs to be kept */
int		elfsh_hijack_plt_sparc_g2(elfshobj_t *file, 
					  Elf32_Sym *symbol,
					  Elf32_Addr addr)
{
  int		foffset;
  uint32_t	addrh, addrl;
  uint32_t	opcode[3];
  
  if (file->hdr->e_machine != ELFSH_HIJACK_CPU_SPARC)
    ELFSH_SETERROR("[libelfsh:hijack_plt_sparc_g2] requested "
		   "ELFSH_HIJACK_CPU_SPARC while the elf file is not "
		   "SPARC\n", -1);

  /* compute the sparc %hi(), %lo() address */
  addrh = addr & 0xfffffc00;
  addrl = addr & 0x3ff;

  /* sethi %hi(addrh), %g2	*/
  opcode[0] = 0x05000000 | addrh >> 10;

  /* jmp %g2 + addrl	! addr	*/
  opcode[1] = 0x81c0a000 | addrl;

  /* Add a nop for delay slot */
  opcode[2] = 0x01000000;
  
  foffset = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  elfsh_raw_write(file, foffset, opcode, 3 * sizeof(uint32_t));
  return (0);
}



/* PLT hijacking on i386 */
int		elfsh_hijack_plt_i86(elfshobj_t *file, 
				     Elf32_Sym *symbol,
				     Elf32_Addr addr)
{
  int		foffset;
  uint8_t	opcode = 0xe9; /* direct jmp with full displacement */
  uint32_t	displacement;
  
  if (file->hdr->e_machine != ELFSH_HIJACK_CPU_i86)
    ELFSH_SETERROR("libelfsh: requested "
		   "ELFSH_HIJACK_CPU_i86 while the elf file is not i86.\n",-1);

  /* compute jmp displacement, 5 is the length of jmp opcode */
  displacement = addr-symbol->st_value - 5;
  foffset = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  elfsh_raw_write(file, foffset, &opcode, sizeof(opcode));
  elfsh_raw_write(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
  return (0);
}


/* Perform custom function hijack, return 0 on success -1 on error */
int		elfsh_hijack_function_by_name(elfshobj_t *file, 
					      uint32_t	 type,
					      char	 *name, 
					      Elf32_Addr addr)
{
  Elf32_Sym	*symbol;
  int		ret;
  
  /* Sanity checks */
  if (file == NULL || name == NULL || addr == 0) 
    ELFSH_SETERROR("[libelfsh:hijack_by_name] Invalid NULL parameter\n", -1);
  if (elfsh_copy_plt(file) < 0)
    return (-1);

  /* Guess the hijack mode */
  switch (type & ELFSH_HIJACK_TYPE_MASK) 
    {
      
      /* fake hijack, huh? ;) */
    case ELFSH_HIJACK_TYPE_NONE:
      return (0);
      
      /* XXX: GOT implemented for i86 only... */
    case ELFSH_HIJACK_TYPE_GOT:
      if (file->hdr->e_machine != ELFSH_HIJACK_CPU_i86) 
	ELFSH_SETERROR("[libelfsh:hijack_function_by_name] GOT hijacking "
		       "unsupported on this architecture\n", -1);
      ret = elfsh_set_got_entry_by_name(file, name, addr);
      if (ret < 0) 
	return (-1);
      return (0);
      
      /* PLT hijack */
    case ELFSH_HIJACK_TYPE_PLT:
      symbol = elfsh_get_dynsymbol_by_name(file, name);
      if (NULL == symbol)
	ELFSH_SETERROR("[libelfsh:hijack_function_by_name] Unknown dynamic "
		       "symbol\n", -1);
      if (!elfsh_is_pltentry(file, symbol))
	ELFSH_SETERROR("[libelfsh:hijack_function_by_name] Symbol is not a "
		       "Procedure Linkage Table entry\n",-1);
      
      /* if ELFSH_HIJACK_CPU_ANY the elf file type decides */
      switch ((type & ELFSH_HIJACK_CPU_MASK) == ELFSH_HIJACK_CPU_ANY ?
	      file->hdr->e_machine                                   :
	      (type & ELFSH_HIJACK_CPU_MASK)) 
	{
	  
	  /* SPARC LINUX and SOLARIS links objects the same way 
	     I guess all SPARC OSes would do the same ;) */
	case ELFSH_HIJACK_CPU_SPARC:
	  
	  switch (type & ELFSH_HIJACK_OS_MASK) 
	    {
	    case ELFSH_HIJACK_OS_SOLARIS:
	    case ELFSH_HIJACK_OS_LINUX:
	    case ELFSH_HIJACK_OS_ANY:
	      return elfsh_hijack_plt_sparc(file, symbol, addr);
	    default:
	      ELFSH_SETERROR("[libelfsh:hijack_function_by_name] SPARC PLT "
			     "hijacking unimplemented for this OS\n", -1);
	    }
	  
	  /* i386/Linux */
	case ELFSH_HIJACK_CPU_i86:

	  switch (type & ELFSH_HIJACK_OS_MASK) 
	    {
	    case ELFSH_HIJACK_OS_LINUX:
	    case ELFSH_HIJACK_OS_ANY:
	      return elfsh_hijack_plt_i86(file, symbol, addr);
	    default:
	      ELFSH_SETERROR("[libelfsh:hijack_function_by_name] i386 PLT "
			     "hijacking unimplemented for this OS\n", -1);
	    }

	}
      
      ELFSH_SETERROR("[libelfsh:hijack_function_by_name] PLT hijacking"
		     "not unimplemented on this architecture\n", -1);
    }
  return (-1);
}
