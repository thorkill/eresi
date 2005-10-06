/*
** Alpha64.c for libelfsh
**
** Started on  Sat Feb 26 23:25:37 2005 mayhem
**
** Skeleton cut & pasted from the sparc64 backend
**
*/
#include "libelfsh.h"



/* Static hooking for Alpha64 */
int	elfsh_cflow_alpha64(elfshobj_t  *sect,
			    char	*name,
			    elfsh_Sym	*sym,
			    elfsh_Addr	addr)
{
  // uint32_t	cur;
  

  // opcodes to hijack
  // 0xc3 (br)
  // 0xe0 / 0xe1 (blbc)
  // 0xe4 / 0xe5 / 0xe6 / 0xe7 (beq)	(21bits of offset)
  // 0xf4 / 0xf5 / 0xf7 (bne)
  // 0x6a / 0x6b (jsr/jmp)
  // 0xe8 (blt)
  // 0xec / 0xed (ble)

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);		 

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}






/* ALTPLT hijacking on ALPHA64 */
/*
  On ALPHA we dont have any relative operands while those
  bytes are copied.

  However since we only have 32 bytes free in the first
  PLT entry, we cannot insert this 12 instrs length code 
  updating the first entry of .alt.plt by the first entry
  of .plt at runtime (since this last is only filled at 
  runtime).

  Because alpha does not provide an instruction that only
  load the 2 low bytes part of a register, we use different
  register values depending on the addresses we need to
  read/write. The only predictable register value is %gp
  whoose value is always .got+0x8000

  If we dont do this ALTPLT hijack, we cannot call the original 
  function from the hook function that was setup in PLT.
  
*/
int		elfsh_hijack_altplt_alpha64(elfshobj_t *file, 
					    elfsh_Sym *symbol,
					    elfsh_Addr addr)
{
  elfsh_Addr	result, result2;
  uint32_t      opcode[12];
  uint32_t      off;
  elfshsect_t   *altplt;
  elfshsect_t	*altpltprolog;
  elfshsect_t	*plt;
  elfshsect_t	*got;
  uint16_t	hi, low, hi2, low2;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Regular checks */
  if (!FILE_IS_ALPHA64(file))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
		   "ELFSH_HIJACK_CPU_ALPHA while the elf file is not "
		   "ALPHA\n", -1);
  
  got	       = file->secthash[ELFSH_SECTION_GOT];
  plt          = file->secthash[ELFSH_SECTION_PLT];
  altplt       = file->secthash[ELFSH_SECTION_ALTPLT];
  altpltprolog = file->secthash[ELFSH_SECTION_ALTPLTPROLOG];
  if (!plt || !altplt || !got || !altpltprolog)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find one of the PLT sections", 
		      -1);

  /* Compute offsets */
  result = plt->shdr->sh_addr - (got->shdr->sh_addr + 0x8000);
  off    = (uint32_t) result;
  hi     = (off & 0xFFFF0000) >> 16;
  low    = (off & 0xFFFF);
  result2 = altplt->shdr->sh_addr - (got->shdr->sh_addr + 0x8000);
  off    = (uint32_t) result2;
  hi2    = (off & 0xFFFF0000) >> 16;
  low2   = (off & 0xFFFF);
  
  /* 00 00 3d 24     ldah     t0,0(gp) */
  opcode[0] = 0x243d0000 | (uint16_t) hi;

  /* 00 00 3d 20     lda    t0,0(t0|gp) */
  opcode[1] = (((short) low > 0)  ? 0x20210000 : 0x203D0000) | (uint16_t) low;

  /* 00 00 7d 24     ldah     t2,0(gp) */
  opcode[2] = 0x247d0000 | (uint16_t) hi2;

  /* 00 00 7d 20     lda    t2,0(t2|gp) */
  opcode[3] = (((short) low2 > 0) ? 0x20630000 : 0x207d0000) | (uint16_t) low2;

  /* 02 14 64 40     addq    t2,0x20,t1 */
  opcode[4] = 0x40641402;

  /* 00 00 81 a4     ldq     t3,0(t0) */
  opcode[5] = 0xa4810000;

  /* 00 00 83 b4     stq     t3,0(t2) */
  opcode[6] = 0xb4830000;

  /* 03 14 61 40     addq    t2,0x8,t2 */
  opcode[7] = 0x40611403;

  /* 01 14 21 40     addq    t0,0x8,t0 */
  opcode[8] = 0x40211401;

  /* a0 0d 43 40     cmple   t1,t2,v0 */
  opcode[9] = 0x40430da0;

  /* fa ff 1f e4     beq     v0,120000ac4 <loop> */
  opcode[10] = 0xe41ffffa;

  /* 00 00 60 d0     bsr     t2,120000ae0 <loop+0x1c> */
  opcode[11] = 0xd0600000;

  elfsh_raw_write(file, altpltprolog->shdr->sh_offset, 
		  opcode, sizeof(uint32_t) * 12);


  /* Last but not least : redirect .alt.plt + 0 on .alt.plt.prolog */
  off = ((-52) >> 2);
  off = (off & (~(0xffffffff << 20)));
  opcode[0] = 0xc0900000 | (uint32_t) off;
  elfsh_raw_write(file, altplt->shdr->sh_offset, 
		  opcode, sizeof(uint32_t));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}









/* PLT hijacking on ALPHA64 */
int		elfsh_hijack_plt_alpha64(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 elfsh_Addr addr)
{
  int           foffset;
  uint32_t      opcode[3];
  uint32_t      off;
  uint16_t	hi, low;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_machine != EM_ALPHA && file->hdr->e_machine != EM_ALPHA_EXP)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "requested "
                     "ELFSH_HIJACK_CPU_ALPHA while the elf file is not "
                     "ALPHA\n", -1);

  if (addr > symbol->st_value)
    {
      opcode[2] = 0xc3800000;	/* forward br */
      off = ((addr - 4) - symbol->st_value - 8) >> 2;
    }
  else
    {
      opcode[2] = 0xc3900000;	/* backward br */
      off = ((addr + 4) - symbol->st_value - 8) >> 2;
    }

  hi  = ((off << 2) & 0xFFFF0000) >> 16;
  low = ((off << 2) + 12) & 0xFFFF;
  off = (off & (~(0xffffffff << 20)));

  /* Set t12 to the address of the called function */
  opcode[0] = 0x277b0000 | (uint16_t) hi;
  opcode[1] = 0x237b0000 | (uint16_t) low;
  opcode[2] |= (uint32_t) off;

#if __DEBUG_REDIR__
  printf("[DEBUG_REDIR] hookaddr = " XFMT " mem2patch = " XFMT 
	 "computed offset = %08X (real = %08X), final opcode = %08X \n",
         addr, symbol->st_value, off, (off << 2), opcode);
#endif
  
  foffset = elfsh_get_foffset_from_vaddr(file, symbol->st_value);

#if __BYTE_ORDER == __BIG_ENDIAN
  opcode[0] = swap32(opcode[0]);
  opcode[1] = swap32(opcode[1]);
  opcode[2] = swap32(opcode[2]);  
  elfsh_raw_write(file, foffset, opcode, sizeof(uint32_t) * 3);
#else
  elfsh_raw_write(file, foffset, opcode, sizeof(uint32_t) * 3);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Find the address of modgot entry for this value */
elfsh_Addr	elfsh_modgot_find(elfshsect_t *modgot, elfsh_Addr addr)
{
  unsigned int	idx;
  elfsh_Addr	*cur;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  cur = elfsh_get_raw(modgot);
  for (idx = 0; idx < modgot->shdr->sh_size / sizeof(elfsh_Addr); idx++)
    {

      //printf("[modgot_find] Compare entries %016lX - %016lX \n", 
      //     cur[idx], addr);

      if (cur[idx] == addr)
	{
	  printf("returned success in modgot_find \n");
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (modgot->shdr->sh_addr + idx * sizeof(elfsh_Addr)));
	}
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle the mod.o.got section */
elfshsect_t	*elfsh_modgot_alpha64(elfshsect_t *infile, elfshsect_t *modrel)
{
  elfshsect_t	*modgot;
  elfshsect_t	*sctcur;
  elfshsect_t	*sect;
  elfsh_Rel	*relcur;
  char		*buff;
  unsigned int	idx;
  unsigned int	gotidx;
  unsigned int	size;
  unsigned int	needed;
  char		*data;
  elfsh_Shdr	hdr;
  elfsh_Addr	*got;
  elfsh_Sym	*sym;
  char		*name;
  char		tmpname[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Create or return existing mod.o.got section */
  XALLOC(buff, strlen(modrel->parent->name) + 4 + 1, NULL);
  sprintf(buff, "%s.got", modrel->parent->name);
  modgot = elfsh_get_section_by_name(infile->parent, buff, 0, 0, 0);
  if (modgot)
    {
      XFREE(buff);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (modgot));
    }

  needed = 0;
  modgot = elfsh_create_section(buff);
  size = (IS_REL(modrel) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela));

  /* Allocate mod.o.got from the number of LITERAL relocations */
  for (sctcur = modrel->parent->sectlist; sctcur; sctcur = sctcur->next)
    if (sctcur->shdr->sh_type == SHT_REL || sctcur->shdr->sh_type == SHT_RELA)
      for (idx = 0; idx < sctcur->shdr->sh_size / size; idx++)
	{
	  relcur = (IS_REL(modrel) ?
		    (void *) ((elfsh_Rel  *) elfsh_get_raw(sctcur) + idx) : 
		    (void *) ((elfsh_Rela *) elfsh_get_raw(sctcur) + idx));
	  if (elfsh_get_reltype(relcur) == R_ALPHA_LITERAL)
	    needed++;
	}


  printf("Size for modgot = %u entries \n", needed);

  /* 
  ** Inject section. Here we are sure that it was loaded in last 
  ** for this module and all ET_REL symbols have been injected.  
  */
  XALLOC(data, needed * sizeof(elfsh_Addr), NULL);
  hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_WRITE | SHF_ALLOC,
			  0, 0, needed * sizeof(elfsh_Addr), 0, 0, 0, 0);

  if (elfsh_insert_mapped_section(infile->parent, modgot, hdr,
				  data, ELFSH_DATA_INJECTION, 0) < 0)
    goto bad;

  modgot = elfsh_get_section_by_name(infile->parent, buff, NULL, NULL, NULL);
  if (modgot == NULL)
    goto bad;

  got = elfsh_get_raw(modgot);

  /* 
  **   Fill the mod.o.got with pointers to the real data
  **   A Real pain .. need to be modularized with the elfsh_relocate_etrel_section() code
  **   -may
  */
  gotidx = 0;
  for (sctcur = modrel->parent->sectlist; sctcur; sctcur = sctcur->next)
    if (sctcur->shdr->sh_type == SHT_REL || sctcur->shdr->sh_type == SHT_RELA)
      for (idx = 0; idx < sctcur->shdr->sh_size / size; idx++)
	{
	  relcur = (IS_REL(modrel) ?
		    (void *) ((elfsh_Rel  *) elfsh_get_raw(sctcur) + idx) :
		    (void *) ((elfsh_Rela *) elfsh_get_raw(sctcur) + idx));
	  
	  /* Now there are many possible computations for the address */
	  if (elfsh_get_reltype(relcur) == R_ALPHA_LITERAL)
	    {

	      sym = elfsh_get_symbol_from_reloc(sctcur->parent, relcur);
	      name = elfsh_get_symname_from_reloc(sctcur->parent, relcur);
	      if (!sym || !name)
ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Symbol or Name not found for LITERAL", 
			       NULL);
	      
	      if (elfsh_get_symbol_bind(sym) != STB_LOCAL   && /* patch BeoS */
		  (elfsh_get_symbol_type(sym) == STT_NOTYPE ||
		   elfsh_get_symbol_link(sym) == SHN_COMMON))
		{
		  
		  sym = elfsh_get_metasym_by_name(infile->parent, name);
		  if (sym == NULL)
		    {
		      printf("missing sym = %s \n", name);
		      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
				     " Cant find requested symbol in ET_EXEC\n", NULL);
		      }
		  got[gotidx] = sym->st_value;		
		}
	      
	      else
		{
		  
		  /* Find target section in ET_REL */
		  sect = elfsh_get_section_by_index(sctcur->parent, sym->st_shndx,
						    NULL, NULL);
		  if (sect == NULL)
		    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
				   "Cant find extracted section in ET_REL\n", NULL);
		  
		  /* Find corresponding inserted section in ET_EXEC */
		  snprintf(tmpname, sizeof(tmpname), "%s%s", sctcur->parent->name, sect->name);
		  sect = elfsh_get_section_by_name(infile->parent, tmpname, 0, 0, 0);
		  if (sect == NULL)
		    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
				   "Cant find inserted section in ET_EXEC\n", NULL);
		  
		  /* Compute pointer value */
		  got[gotidx] = sect->shdr->sh_addr + sym->st_value;
		  
		}
	      
	      printf("MODGOT : found a literal and filling modgot with " XFMT "\n",
		     got[gotidx]);
	      gotidx++;
	    }
	}

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (modgot));
 bad:
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Bad return for modgot", NULL);
}




/* 
   Perform relocation on entry for ALPHA64 architecture.

   Only the types that are needed for relocating ET_REL objects 
   are filled, others might come on the need.
*/
int       elfsh_relocate_alpha64(elfshsect_t       *new,
				 elfsh_Rela        *cur,
				 elfsh_Addr        *dword,
				 elfsh_Addr        addr,
				 elfshsect_t	   *mod)
{
  elfshsect_t	*modgot;
  elfsh_Addr     result;
  int	         retval;
  uint32_t	*dw;
  uint32_t	off;
  uint16_t	hi, low;
  elfsh_Addr	val;


  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("relocata_alpha64 Input addr = " XFMT "\n", addr);


#define ADD       (cur->r_addend)
#define BAS       (elfsh_get_object_baseaddr(new->parent))
#define PLA       (new->shdr->sh_addr + cur->r_offset) 
#define SYM       (addr)
#define	SETSYM(a) (addr = a, printf("SETSYM addr set to " XFMT "\n", a))
#define GP        (elfsh_get_sht_entry_by_name(new->parent, ".got")->sh_addr+0x8000)

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
#define TRUNCATE32(x)   (x)  /* XXX just assume its 32bit */ 

#define TRUNCATE16HI(x)   ((x) & ~(0xFFFFFFFF >> 16))

  /* We compute a 32bit dword offset for using with <= 32bit relocations */
  dw = (uint32_t *) dword; 
  retval = result = 0;
  
  switch (elfsh_get_reltype((elfsh_Rel *) cur))
    {
      /* cases ... */
    case R_ALPHA_NONE:
      printf ("R_ALPHA_NONE\n"); 
      break;
    case R_ALPHA_REFLONG:
      printf ("R_ALPHA_REFLONG\n"); 
      result = TRUNCATE32(addr);
      break;
    case R_ALPHA_REFQUAD:
      printf ("R_ALPHA_REFQUAD\n"); 
      result = addr;
      break;
    case R_ALPHA_GPREL32:
      printf ("R_ALPHA_GPREL32\n"); 
      // XXX
      break;
      
      
      /* 
	 On alpha, each static object in the relocatable file that have
	 an address filled by R_ALPHA_LITERAL has an entry in .got
	 which is filled at compil time and loaded in a register
	 at runtime using this relocation. The .got entry is supposed
	 to contains the absolute address of the object. This is alpha 
	 specific and we have to reconstruct this behavior for the injected
	 module with a new section : mod.o.got
	                                            -mm
      */
    case R_ALPHA_LITERAL:

      printf ("R_ALPHA_LITERAL : GP displacement for symbol \n");
      
      modgot = elfsh_modgot_alpha64(new, mod);
      if (modgot == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,"modgot failed", -1);
      val = elfsh_modgot_find(modgot, addr);
      SETSYM(val);
      
      //elfsh_print_sectlist(new->parent, "MODGOT");
      
      if (SYM > GP)
	{
	  result = SYM - GP;
	  low = (uint16_t) result;
	}
      else
	{
	  result = GP - SYM;
	  low = (uint16_t) result;
	  low = -low;
	}
      
      printf ("SYM    : " XFMT " \n", SYM);
      printf ("GP     : " XFMT " \n" , GP);
      printf ("low    : %hd      \n", low);
      
      // seems to be badly encoded
      *dw += low;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
    case R_ALPHA_LITUSE:      
      printf ("R_ALPHA_LITUSE\n"); 
      // XXX
      break;
      
    case R_ALPHA_GPDISP:
      printf ("R_ALPHA_GPDISP : GP displacement for current addr \n"); 
      
      if (PLA < (GP))
	result = (GP) - PLA;
      else
	result = PLA  - (GP); 
      
      // The particular case of elfsh makes the high offset always 0
      // since the .got stay the same on function hijack and the injected
      // code is always mapped after the .got in a 16bits shift interval
      off = (uint32_t) result;
      hi  = 0;
      low = TRUNCATE16(off);
      
      if (PLA > (GP))
	low = -low;
      
      if (off > 0xFFFF)
	printf("[debug_alpha_reloc] warning : difference does not make in 16bits %08X \n", off);
      else
	{
	  printf("[debug_alpha_reloc] offset = %08X low = %04X high = %04X \n", off, low, hi);
	}
      
      *(dw + (ADD / 4)) += low;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
      
    case R_ALPHA_BRADDR:
      printf ("R_ALPHA_BRADDR\n"); 
      // XXX
      break;
    case R_ALPHA_HINT:
      printf ("R_ALPHA_HINT\n"); 
      // XXX
      
      break;
    case R_ALPHA_SREL16:
      printf ("R_ALPHA_SREL16\n"); 
      // XXX
      break;
    case R_ALPHA_SREL32:
      printf ("R_ALPHA_SREL32\n"); 
      // XXX
      break;
    case R_ALPHA_SREL64:
      printf ("R_ALPHA_SREL64\n"); 
      // XXX
      break;
    case R_ALPHA_OP_PUSH:
      printf ("R_ALPHA_OP_PUSH\n"); 
      // XXX
      break;
    case R_ALPHA_OP_STORE:
      printf ("R_ALPHA_OP_STORE\n"); 
      // XXX
      break;
    case R_ALPHA_OP_PSUB: 
      printf ("R_ALPHA_OP_PSUB\n"); 
      // XXX
      break;
    case R_ALPHA_OP_PRSHIFT:
      printf ("R_ALPHA_PRSHIFT\n"); 
      // XXX
      break;
    case R_ALPHA_GPVALUE:    
      printf ("R_ALPHA_GPVALUE\n"); 
      // XXX
      break;
    case R_ALPHA_GPRELHIGH:  
      printf ("R_ALPHA_GPRELHIGH\n"); 
      // XXX
      break;
    case R_ALPHA_GPRELLOW:    
      printf ("R_ALPHA_GPRELLOW\n"); 
      // XXX
      break;
    case R_ALPHA_IMMED_GP_16: 
      printf ("R_ALPHA_IMMED_GP_16\n"); 
      // XXX
      break;
    case R_ALPHA_IMMED_GP_HI32:
      printf ("R_ALPHA_IMMED_GP_HI32\n"); 
      // XXX
      break;
    case R_ALPHA_IMMED_SCN_HI32: 
      printf ("R_ALPHA_IMMED_SCN_HI32\n"); 
      // XXX
      break;
    case R_ALPHA_IMMED_BR_HI32:  
      printf ("R_ALPHA_IMMED_BR_HI32\n"); 
      // XXX
      break;
    case R_ALPHA_IMMED_LO32:  
      printf ("R_ALPHA_IMMED_LO32\n"); 
      // XXX
      break;
    case R_ALPHA_COPY:        
      printf ("R_ALPHA_COPY\n"); 
      // XXX
      break;
    case R_ALPHA_GLOB_DAT:    
      printf ("R_ALPHA_GLOB_DAT\n"); 
      // XXX
      break;
    case R_ALPHA_JMP_SLOT:    
      printf ("R_ALPHA_JMP_SLOT\n"); 
      // XXX
      break;
    case R_ALPHA_RELATIVE:    
      printf ("R_ALPHA_RELATIVE\n"); 
      // XXX
      break;

    default:
ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unsupported relocation type",
		     -1);
      break;
    }
  
  *dword += result;

#undef ADD
#undef BAS
#undef PLA
#undef SYM
#undef SETSYM
#undef GP
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
}


