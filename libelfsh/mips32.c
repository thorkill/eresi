/*
** mips32.c for libelfsh
**
** Started on  Sat Feb 26 23:25:37 2005 mayhem
**
** Skeleton cut & pasted from the sparc64 backend
**
*/
#include "libelfsh.h"





/* Static hooking for Mips32 */
/*
  Note : a new technique is beeing considered that
  may invalidate this handler as it (at least 
  change the way it should be implemented)

  Make sure to ask anything before deciding to implement it
*/
int			elfsh_cflow_mips32(elfshobj_t *file,
					   char	      *name,
					   elfsh_Sym  *symbol,
					   elfsh_Addr  addr)
{
  elfshsect_t		*hooks;
  elfshsect_t		*source;
  uint32_t		buff[3];
  int			ret, len;
  int			off;
  char			*hookbuf;
  char			*hook;
  elfsh_Sym		sym;
  char			bufname[BUFSIZ];
  void			*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /*
    func+0:	[addu t9, (hook-func)]
    func+4:	[jump hook]
    func+8:	[nop]
    func+c:	[???]

    hook:	...

    old_entry+0:	[addu t9, (func-old_entry)]
    old_entry+4:	[instr1]
    old_entry+8:	[instr2]
    old_entry+c:	[instr3]
    old_entry+10:	[jmp func+8]
    old_entry+14:	[nop]

   */

  /* Resolve parameters */
  off = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  if (!off) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid address to hijack", -1);

  ret = elfsh_raw_read(file, off, (void *) buff, 3*4);
  if (ret != 3*4)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Function too small to be hijacked", -1);

  /* If the hook section does not exist, create it */
  hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 0, 0, 0);
  if (!hooks)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Cannot get .HOOKS", -1);  
  
  hook = (char *) (hooks->shdr->sh_addr + hooks->curend);
  
  if (((uint32_t)  symbol->st_value & 0xf0000000) != (addr & 0xf0000000))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "hook function too far from hijacked function", -1);
  
  if (((uint32_t) hook & 0xf0000000) != ((symbol->st_value + 0x8) & 0xf0000000)) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "hook section too far from hijacked function", -1);
  
  if ((addr - (uint32_t)  symbol->st_value) & (0xffffffff<<16))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "hook function too far from hijacked function", -1);

  if (((uint32_t)  symbol->st_value - (uint32_t) hook) & (0xffffffff<<16))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "hook section too far from hijacked function", -1);

  /* Determine the minimal aligned length */
  /* RISC's powa */
  /* 3 instructions : 1 add t9..., 1 jmp, 1 nop for delay slot */
  ret = 3 * 4; 

  /* Create the hook for this function */
  data = elfsh_get_raw(hooks);
  memset(data + hooks->curend, 0x00, 40); // nop 

  /* addi $t, $s, imm : 0010 00ss ssst tttt iiii iiii iiii iiii */
  *((uint32_t *) ((char *) (data + hooks->curend) + 0x0)) = 0x23390000;
  *((uint32_t *) ((char *) (data + hooks->curend) + 0x0)) |= 
    (((uint32_t) sym.st_value - (uint32_t)hook) & 0x0000ffff);

  /* first three hijacked function's instructions */
  *((uint32_t *) ((char *) (data + hooks->curend) + 0x4)) = buff[0];
  *((uint32_t *) ((char *) (data + hooks->curend) + 0x8)) = buff[1];
  *((uint32_t *) ((char *) (data + hooks->curend) + 0xc)) = buff[2];

  /* non-linked jump to func + 8 (where should be a NOP) */
  /* mips32 jump use the 4 MSB of PC reg and 26 bits from instruction left 
     shited by 2 */
  memcpy(data + hooks->curend + 0x10, "\x08\x00\x00\x00", 4);
  *((uint32_t *) ((char *) (data + hooks->curend) + 0x10)) |= ((symbol->st_value + 0x8) & (~ 0xe0000000 ))>>2;

  /* NOTE : there must be a NOP after this last jump */

  /* Insert the old symbol on the original saved bytes */
  //name = elfsh_get_symbol_name(file, symbo);
  snprintf(bufname, BUFSIZ, "old_%s", name); 
  elfsh_insert_funcsym(file, bufname, (elfsh_Addr) hook,
		       ret + 0x10, hooks->index);

  /*  
      snprintf(bufname, BUFSIZ, "hook_%s", name);
      elfsh_insert_funcsym(file, bufname, addr,
      ret + 8, hooks->index);
  */

  /* We need to grab the parent section to compute the remaining offset */
  source = elfsh_get_parent_section_by_foffset(file, off, NULL);
  if (!source)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find parent section for hooked addr", -1);

  /* Install the hook */
  hookbuf = alloca(ret);

  /* patch t9 reg */
  *((uint32_t *) ((char *) (hookbuf) + 0x0)) = 0x23390000;
  *((uint32_t *) ((char *) (hookbuf) + 0x0)) |= ((addr - symbol->st_value) & 
						 0x0000ffff);  
  /* jump to hook func */
  *((uint32_t *) ((char *) (hookbuf) + 0x4)) = 0x08000000;
  *((uint32_t *) ((char *) (hookbuf) + 0x4)) |= ((uint32_t) (addr ) & 
						 (~0xe0000000))>>2;
  /* delay slot's NOP */
  *((uint32_t *) ((char *) (hookbuf) + 0x8)) = 0x00000000;

  len = elfsh_raw_write(file, off, hookbuf, ret);
  if (len != ret)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Error during hook installation", -1);

  /* Everything OK, ret is always 3*4 on mips32 (RISC strike again) */
  hooks->curend += ret + 6; // (6 = 1 add, 3 instr, 1 jump, 1 nop)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* PLT hijacking on MIPS32, not needed thus unimplemented for now */
int		elfsh_hijack_plt_mips32(elfshobj_t *file, 
					elfsh_Sym *symbol,
					elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS\n", 0);
}






/* ALTPLT hijacking on MIPS32 */
/* 
   On MIPS we need to restore 2 words from .alt.got to .got 
   and callback __libc_start_main. 

   Update: we also need to copy .got values for extern
   variables. We put ld/st pairs in .pad.got so that we
   fill those only one time, before calling __libc_start_main
   
   The code works and is functional -mm
*/
int		elfsh_hijack_altplt_mips32(elfshobj_t *file, 
					   elfsh_Sym *symbol,
					   elfsh_Addr addr)
{
  elfshsect_t   *altgotprolog, *altgot, *padgot, *got, *start;
  elfshsect_t	*dynsym, *dynamic;
  elfsh_Sym	*sym;
  elfsh_Dyn	*dynent, *dynent2;
  uint32_t	gotno, gotsym;
  u_int		varnbr, gotnbr, symnbr;
  u_int		opcodendx, gotindex, index, varindex;
  uint32_t      *opcode;
  uint16_t	diff;
  uint16_t	gotdiff;
  uint32_t	*originstr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Regular checks */
  if (!FILE_IS_MIPS(file))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "File is not MIPS", -1);

  altgotprolog = file->secthash[ELFSH_SECTION_ALTGOTPROLOG];
  altgot       = file->secthash[ELFSH_SECTION_ALTGOT];
  padgot       = file->secthash[ELFSH_SECTION_PADGOT];
  got	       = file->secthash[ELFSH_SECTION_GOT];
  start        = file->secthash[ELFSH_SECTION_MIPSTART];
  dynsym       = file->secthash[ELFSH_SECTION_DYNSYM];
  dynamic      = file->secthash[ELFSH_SECTION_DYNAMIC];

  if (!altgotprolog || !altgot || !got || !padgot || !dynsym || !dynamic)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find GOT, ALTGOTPROLOG, ALTGOT"
		      " PADGOT, DYSYM, DYNAMIC or MIPSTART section", -1);

  /* Signed 16bits displacement from %gp to last word of .pad.got */
  diff = 0x800c;
  originstr = elfsh_get_raw(padgot->data) + padgot->shdr->sh_size - 4;
  *originstr = altgotprolog->shdr->sh_addr; 
  gotdiff = (uint16_t) got->shdr->sh_addr - altgot->shdr->sh_addr;

  /* Valid _start signature on Linux, may you FIXME for other OS */
  /* lw      t9, __libc_start_main_off(gp) */
  originstr = ((uint32_t *) elfsh_get_raw(start) + 19);	

  /* Grab infos on .got using .dynamic */
  dynent  = elfsh_get_dynamic_entry_by_type(file, DT_MIPS_GOTSYM);
  gotsym = (dynent  ? elfsh_get_dynentry_val(dynent)  : 0);
  symnbr = dynamic->shdr->sh_size / sizeof(elfsh_Sym);
  if (symnbr < gotsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "DYNSYM smaller than DT_MIPS_GOTSYM", -1);
  
  dynent2 = elfsh_get_dynamic_entry_by_type(file, DT_MIPS_LOCAL_GOTNO); 
  gotno  = (dynent2 ? elfsh_get_dynentry_val(dynent2) : 0);
  gotnbr = got->shdr->sh_size / sizeof(elfsh_Addr);
  if (gotnbr < gotno)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GOT smaller than DT_MIPS_GOTSYM", -1);

  sym   = elfsh_get_raw(dynsym);
  sym  += gotsym;

  /* Find all .dynsym entries matching external variables
     (Assuming .got and .rel entries are in the same order)
     Copy their .alt.got entries value in .got 
  */
  for (varnbr = 0, gotindex = gotno, index = gotsym; 
       index < symnbr && gotno < gotnbr; index++, gotindex++)
    if (elfsh_get_symbol_type(sym + index) == STT_OBJECT && !sym[index].st_value)
      varnbr++;
  
  printf("[DEBUG_GOTPLT:mips] Found %u extern variables\n", varnbr);

  XALLOC(opcode, (9 + (varnbr * 2)) * sizeof(uint32_t), -1);

  /*
    __asm__("addi $t0, $gp, 0x8010;");
    __asm__("addi $t1, $t0, gotdiff;"); 
    __asm__("lw   $t2, 0($t0);");     
    __asm__("lw   $t3, 4($t0);");     
    __asm__("sw   $t2, 0($t1);");     
    __asm__("sw   $t3, 4($t1);");     
    __asm__("lw   $t9, __libc_start_main_off($gp)");     <===
    __asm__("jr   $t9;");            
    __asm__("nop;");                 
  */
  opcode[0] = 0x23888010;		/* compute .alt.got addr */
  opcode[1] = 0x21090000 | gotdiff;	/* compute .got addr */
  opcode[2] = 0x8d0a0000;		/* load first   .alt.got word */
  opcode[3] = 0x8d0b0004;		/* load second  .alt.got word */
  opcode[4] = 0xad2a0000;		/* write first  .alt.got word into .got */
  opcode[5] = 0xad2b0004;		/* write second .alt.got word into .got */
  opcode[6] = *originstr;		/* reload __libc_start_main addr into $t9 */ 

  /* Compute the static __libc_start_main hijack in .start */
  *originstr = (*originstr & 0xFFFF0000) | diff;

  /* Now generate the ld/st pairs a la mano residing in .pad.got */
  printf("Using GOTNO = %u and GOTSYM = %u\n", gotno, gotsym);

  opcodendx = 7;
  for (index = varindex = 0, gotindex = gotno; varindex < varnbr; index++, gotindex++)
    if (elfsh_get_symbol_type(sym + index) == STT_OBJECT && !sym[index].st_value)
      {
	
	printf("Using GOT index %u \n", gotindex);
	
	opcode[opcodendx++] = 0x8d0a0000 | (uint16_t) gotindex * sizeof(elfsh_Addr);
	opcode[opcodendx++] = 0xad2a0000 | (uint16_t) gotindex * sizeof(elfsh_Addr);
	varindex++;
      }

  opcode[opcodendx++] = 0x0320f809;	 /* call restored $t9 */
  opcode[opcodendx++] = 0x00000000;	 /* nop delay slot    */

  if (padgot->shdr->sh_size < sizeof(uint32_t) * varnbr * 2)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".pad.got section too small", -1);

  elfsh_raw_write(file, altgotprolog->shdr->sh_offset, opcode, sizeof(uint32_t) * 7);
  elfsh_raw_write(file, padgot->shdr->sh_offset, opcode + 7, sizeof(uint32_t) * (2 + (varnbr * 2)));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






















/* Perform relocation on entry for MIPS32 architecture 
   XXX Not really endianess respectful - ym
*/
int			elfsh_relocate_mips32(elfshsect_t       *new,
					      elfsh_Rel         *cur,
					      elfsh_Addr        *dword,
					      elfsh_Addr        addr,
					      elfshsect_t	*mod)
{
  int			retval;
  static u_int		HI16_todo = 0;
  static elfsh_Rel	*HI16_cur = NULL; 
  static elfsh_Addr	*HI16_dword = NULL;
  static elfsh_Addr	HI16_addr = NULL;

  elfsh_Addr		old_HI16_dword = NULL;


#define VERIFY16(x)     (((x) & (0xFFFFFFFF << 16)) ? 1 : 0)
  
#define TRUNCATE16(x)   ((x) & ~(0xFFFFFFFF << 16))
#define TRUNCATE32(x)   (x)				/* XXX just assume its 32bits */ 

#define ISLOCAL(x)	(x->r_info & STB_LOCAL)
#define ISEXTERNAL(x)   (!(x->r_info & STB_LOCAL))
#define ISGPDISP(x)     (elfsh_get_symbol_by_name(new->parent, "_gp_disp")->st_value == \
			 elfsh_get_symbol_from_reloc(new->parent, x)->st_value) 

 
#define S	(addr)					/* XXX Assume we only face local symbols */
#define A	(*dword)
#define P	(new->shdr->sh_addr + cur->r_offset)
#define G	(addr - elfsh_get_sht_entry_by_name(new->parent, ".got")->sh_addr) 
							/* XXX what if there is no .got ? */
#define GP	(elfsh_get_gpvalue_addr(new->parent)?(*(elfsh_get_gpvalue_addr(new->parent))):NULL)					

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  retval = 0;
  switch (elfsh_get_reltype(cur))
    {
      /* cases ... */
    case  R_MIPS_NONE:
      break;
    case  R_MIPS_16:
      break;
    case  R_MIPS_32:
      /* External symbol case */
      /*
	S : Represents the value of the symbol whose index resides in the 
	    relocation entry,  
	    unless the the symbol is STB_LOCAL and is of type
	    STT_SECTION in which case S represents the original sh_addr 
	    minus the final sh_addr.	
	A : addend
	 
	TRUNCATE32(S + A)
      */
      
      /* Local symbol case */
      /*
	TRUNCATE32(S + A)
      */

      printf("[DEBUG] elfsh_relocate_mips32 : R_MIPS_32 \n");
      
      *dword += TRUNCATE32(S + A);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));

    case  R_MIPS_REL32:
      break;
    case  R_MIPS_26:
      break;
    case  R_MIPS_HI16:
      /* _gp_disp case */
      /*
	AHI : addend of HI16 reloc (this reloc)
	ALO : addend of LO16 reloc (next to this LO16 reloc)
	AHL = (AHI << 16) + (short) ALO.

	P   : Represents the place (section offset or address) of the 
	      storage unit being relocated (computed using r_offset).
	GP  : Represents the final gp value to be used for the relocatable, 
	      executable, or shared object file being produced.

	VERIFY16(((AHL + GP - P) - (short) (AHL + GP - P))>>16) 
      */

      /* Local symbol case */
      /*
	AHL, S : see above

	TRUNCATE16(((AHL + S) - (short) (AHL + S))>>16)
      */

      /* External symbol case */
      /*
	AHL, S : see above
	
	NOTE : Does it exist ? 
      
	TRUNCATE16(((AHL + S) - (short) (AHL + S))>>16)
      */


      /* remember last R_MIPS_HI16 */
      HI16_todo = 1;
      HI16_cur = cur; 
      HI16_dword = dword;
      HI16_addr = addr;
      
      /* as all HI16 must be followed by a LO16 we wont do the relocation 
	 right now 
      */
      printf("[DEBUG] elfsh_relocate_mips32 : R_MIPS_HI16 \n");

      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));

    case  R_MIPS_LO16:
      /* _gp_disp case */
      /*
	AHI : addend of HI16 reloc (previously encoutred)
	ALO : addend of LO16 reloc (this reloc)

	AHL = (AHI << 16) + (short) ALO.
	
	P, GP : see above

	VERIFY16(AHL + GP + P + 4)
      */

      /* Local symbol case */
      /*
	AHL : see _gp_disp case	
	S   : see above 
	
	TRUNCATE16(AHL + S)
      */

      /* External symbol case */
      /*
	AHL, S   : see above 

	Does it exists ? 

	TRUNCATE16(AHL + S);
      */

      printf("[DEBUG] elfsh_relocate_mips32 : R_MIPS_LO16 \n");

      if (HI16_cur == NULL || HI16_dword == NULL)
	{
	  printf("[DEBUG] elfsh_relocate_mips32: You loose\n");
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "Invalid HI16 relocation context", -1);
	}

      /* 
       (*HI16_dword) will be modified during HI16 relocation 
       and we need it to perform LO16 reloc 
      */
      old_HI16_dword = *HI16_dword;

      if (HI16_todo == 1)
	{
	  /* gpdisp case */
	  if (ISGPDISP(HI16_cur))
	    {
	      /* VERIFY16(((AHL + GP - P) - (short) (AHL + GP - P))>>16) : 
		 P = new->shdr->sh_addr + HI16_cur->r_offset
		 AHL = ((old_HI16_dword << 16) + (short) A)
		 GP = GP
	      */
	      *HI16_dword += 
		VERIFY16((((old_HI16_dword << 16) + (short) A) +		/* AHL */
			  GP -							/* GP */
			  (new->shdr->sh_addr + HI16_cur->r_offset) -		/* P */
			  (short) (((old_HI16_dword << 16) + (short) A) +	/* AHL */
				   GP -						/* GP */
				   (new->shdr->sh_addr + HI16_cur->r_offset))	/* P */
			  )>>16);
	    }
	  /* local symbol */
	  else if (ISLOCAL(HI16_cur) || ISEXTERNAL(HI16_cur))
	    {
	      /* TRUNCATE16(((AHL + S) - (short) (AHL + S))>>16) 
		 AHL = (see above)
		 S = HI16_addr
	      */
	      HI16_dword += TRUNCATE16((((((old_HI16_dword << 16) + (short) A))/*AHL*/ 
					 + HI16_addr/*S*/) 
					- (short) (((old_HI16_dword << 16) 
						    + (short) A)/*AHL*/ 
						   + HI16_addr/*S*/))>>16);
		
		
		
		}
	  
	  HI16_todo = 0;
 	}
      
      if (ISGPDISP(cur))
	{
	  *dword = VERIFY16(((old_HI16_dword << 16) + (short) A) + GP + P + 4);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
	}
      

      if (ISLOCAL(cur))
	{
	  *dword = TRUNCATE16(((old_HI16_dword << 16) + (short) A) + S);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
	}
      
      if (ISEXTERNAL(cur))
	{
	  *dword = TRUNCATE16(((old_HI16_dword << 16) + (short) A) + S);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
	}

      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Unknown relocation type", -1);      
      
    case  R_MIPS_GPREL16:
      break;
    case  R_MIPS_LITERAL:
      break;
    case  R_MIPS_GOT16:
      /* External symbol case */
      /*
	G : Represents the offset into the global offset table at which the 
	    address of the relocation entry symbol resides during execution.
      
	VERIFIY16(G)
      */
      
      /* Local Symbol case */
      /*
	In the local case, the R_MIPS_GOT16 must be followed immediately 
	with a R_MIPS_LO16 relocation. The AHL addend is extracted and 
	the section in which the referenced data item resides is determined
	(requiring that all sections in an object module have unique 
	addresses and not overlap). From this address the final address of 
	the data item is calculated. If necessary, a global offset table 
	entry is created to hold the high 16 bits of this address (an 
	existing entry is used when possible). The rel16 field is replaced
	by the offset of this entry in the global offset table. The lo16 
	field in the following R_MIPS_LO16 relocation is replaced by the 
	low 16 bits of the actual destination address. This is meant for 
	local data references in position-independent code so that only one
	global offset table entry is necessary for every 64 KBytes of local 
	data.

	...
      */
      printf("[DEBUG] elfsh_relocate_mips32 : R_MIPS_GOT16 \n");

      if (ISLOCAL(cur))
	{
	  *dword += VERIFY16(G);
	  
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
	}
      
      if (ISEXTERNAL(cur))
	{
	  printf("External symbol MIMPS_GOT16 reloc not yet implemented\n");
	  
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
	}

      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Unknown MIPS relocation type", -1);

    case  R_MIPS_PC16:
      break;
    case  R_MIPS_CALL16:
      /* only one case */
      /*
	G : see above
	
	VERIFY16(G)
      */
            printf("[DEBUG] elfsh_relocate_mips32 : R_MIPS_CALL16 \n");
      *dword += VERIFY16(G);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));

    case  R_MIPS_GPREL32:
    case  R_MIPS_SHIFT5:
    case  R_MIPS_SHIFT6:
    case  R_MIPS_64:
    case  R_MIPS_GOT_DISP:
    case  R_MIPS_GOT_PAGE:
    case  R_MIPS_GOT_OFST:
    case  R_MIPS_GOT_HI16:
    case  R_MIPS_GOT_LO16:
    case  R_MIPS_SUB:
    case  R_MIPS_INSERT_A:
    case  R_MIPS_INSERT_B:
    case  R_MIPS_DELETE:
    case  R_MIPS_HIGHER:
    case  R_MIPS_HIGHEST:
    case  R_MIPS_CALL_HI16:
    case  R_MIPS_CALL_LO16:
    case  R_MIPS_SCN_DISP:
    case  R_MIPS_REL16:
    case  R_MIPS_ADD_IMMEDIATE:
    case  R_MIPS_PJUMP:
    case  R_MIPS_RELGOT:
    case  R_MIPS_JALR:
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported relocation type", -1);
      break;
    }

#undef S
#undef A
#undef P
#undef G
#undef GP

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
}


