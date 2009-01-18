/**
 * @file ia32.c
 * @ingroup libelfsh
 * ia32.c for libelfsh
 *
 * Started on  Fri Jan 11 03:05:37 2003 jfv
 *
 *
 * $Id$
 *
 */
#include "libelfsh.h"
#include "libasm.h"

#define ELFSH_IA32_ARG_MAXSIZE 	512
#define ELFSH_IA32_PROLOG_MSTEP 3
static asm_processor	proc;
static u_char		proc_init = 0;
static int      	args[ELFSH_TRACE_MAX_ARGS+1];
static u_int		arg_count;
static u_int		max_arg_offset;

#define ELFSH_IA32_PUSH_REGBASE(_i)			\
(_i.instr == ASM_PUSH 					\
 && _i.op[0].regset == ASM_REGSET_R32			\
 && (_i.op[0].type) == ASM_OP_BASE)

#define ELFSH_IA32_MOV_REGBASE(_i, _reg)		\
(_i.instr == ASM_MOV 					\
 && _i.op[0].baser == _reg 				\
 && _i.op[1].baser == ASM_REG_ESP 			\
 && _i.op[0].regset == ASM_REGSET_R32 			\
 && _i.op[1].regset == ASM_REGSET_R32			\
 && (_i.op[0].type) == ASM_OP_BASE	\
 && (_i.op[1].type) == ASM_OP_BASE)

/**
 * Start of hook code for EXTPLT 
 *
 * @param extplt
 * @param altgot
 * @param dynsym
 * @param relplt
 * @return
 */
int		elfsh_extplt_ia32(elfshsect_t *extplt, 
				  elfshsect_t *altgot,
				  elfshsect_t *dynsym, 
				  elfshsect_t *relplt)
{
  int		prot;
  int		*reloc;
  eresi_Addr	gotent;
  elfsh_Rel	r;
  char		*ent;
  u_int		relentsz;
  elfshsect_t	*plt;
  elfshsect_t	*got;
  eresi_Addr	diff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  plt = elfsh_get_plt(extplt->parent, NULL);

  /* Do we need to encode extplt entry ? */
  if (plt && extplt->curend >= plt->shdr->sh_size)
    {
      got = elfsh_get_gotsct(extplt->parent);
      if (got)
	{
	  diff = (eresi_Addr) altgot->shdr->sh_addr - got->shdr->sh_addr;
	  elfsh_reencode_pltentry_ia32(extplt->parent, extplt, diff, extplt->curend);
	}
    }

  /* 6 is the size of the first instruction of a .plt entry on x86 */
  gotent = extplt->shdr->sh_addr + extplt->curend + 6;

  memcpy(elfsh_readmem(altgot) + altgot->curend, (char *) &gotent, sizeof(gotent));
  //elfsh_writemem(altgot->parent, altgot->shdr->sh_addr + altgot->curend, 
  //	 (char *) &gotent, sizeof(gotent));

  altgot->curend += sizeof(gotent);
 
#if __DEBUG_EXTPLT__
  printf("[DEBUG_EXTPLT] Set ALTGOT entry " XFMT " to value " XFMT "\n", 
	 altgot->shdr->sh_addr + altgot->curend, gotent);
#endif

  /* 7 is the offset for the relocation offset location, encoded in GOT */
  ent = elfsh_readmem(extplt) + extplt->curend;
  reloc  = (int *) ((char *) ent + 7);
  prot = elfsh_munprotect(extplt->parent, 
			  (eresi_Addr) reloc, 
			  (char *) reloc - (char *) elfsh_readmem(extplt));
  *reloc = relplt->curend;
  elfsh_mprotect(extplt->parent,
		 (eresi_Addr) reloc, 
		 (char *) reloc - (char *) elfsh_readmem(extplt), prot);
  extplt->curend += elfsh_get_pltentsz(extplt->parent);

  /* Insert relocation entry pointing on the new PLT entry */
  relentsz = IS_REL(extplt) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
  r = elfsh_create_relent(R_386_JMP_SLOT, dynsym->curend / sizeof(elfsh_Sym), 
			  altgot->shdr->sh_addr + altgot->curend - sizeof(eresi_Addr));

  memcpy(elfsh_readmem(relplt) + relplt->curend, &r, relentsz);
  //elfsh_writemem(relplt->parent, relplt->shdr->sh_addr + relplt->curend, &r, relentsz);

  relplt->curend += relentsz;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * On IA32 we need to reencode the PLT so that it uses the .alt.got instead of .got
 * Should work on both ET_EXEC and ET_DYN (similar encoding offsets even if different jmp) 
 *
 * @param file
 * @param plt
 * @param diff
 * @param off
 * @return
 */
int		elfsh_reencode_pltentry_ia32(elfshobj_t   *file, 
					     elfshsect_t  *plt, 
					     uint32_t     diff, 
					     u_int	     off)
{
  char		*pltent;
  uint32_t	*got;
  elfshsect_t	*relplt;
  elfshsect_t	*realplt;
  u_int		resoff;
  int		entsz;
  u_int32_t	*align;
  u_int		is_extplt;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || plt == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* 12 is the size of 3 reserved PLT entries on x86 */
  is_extplt = !strcmp(plt->name, ELFSH_SECTION_NAME_EXTPLT);

  resoff = (is_extplt ? 12 : 0);
  
  if (is_extplt)
    {
      realplt = elfsh_get_plt(file, NULL);

      /* Do we need to copy previous element ? */
      if (off >= realplt->shdr->sh_size)
	{
	  entsz = elfsh_get_pltentsz(file);

	  /* We need at least one element */
	  if (off < entsz + elfsh_get_first_pltentsz(file))
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Cannot find a previous extplt element", -1);
	  
	  memcpy(plt->data + off, plt->data + off - entsz, entsz);
	  
	  pltent = plt->data + off + 12;
	  align = (uint32_t *) pltent;
	  *align -= 16;

	  diff = resoff + 4;
	}
    }

  /* Add ALTGOT addr difference to the GOT offset encoded in PLT */
  pltent = plt->data + off + 2;
  got = (uint32_t *) pltent;
  *got += diff - resoff;

  /* If thats EXTPLT we are reencoding, shift the pushed reloc offsets as well */
  if (is_extplt)
    {
      relplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELPLT, 0, 0, 0);
      if (relplt == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot retreive .rel.plt", -1);
      
      pltent = plt->data + off + 7;
      got  = (uint32_t *) pltent;
      *got = relplt->shdr->sh_size;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * On IA32 we need to reencode the PLT so that it uses the .alt.got instead of .got 
 * Should work on both ET_EXEC and ET_DYN 
 *
 * @param file
 * @param plt
 * @param diff
 * @return
 */
int		elfsh_reencode_first_pltentry_ia32(elfshobj_t  *file, 
						   elfshsect_t *plt, 
						   uint32_t	diff)
{
  char		*pltent;
  uint32_t	*got;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || plt == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
      
  /* The first GOT address is at offset .plt+2 on IA32 */
  pltent = plt->data + 2;
  got = (uint32_t *) pltent;
  *got += diff;
  
  /* The second GOT address is at offset .plt+8 on IA32 */
  pltent = plt->data + 8;
  got = (uint32_t *) pltent;
  *got += diff;		

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Hook for ENCODEPLT1 on IA32 : Simple wrapper for both calls 
 *
 * @param file
 * @param plt
 * @param extplt
 * @param diff
 * @return
 */
int		elfsh_encodeplt1_ia32(elfshobj_t *file, 
				      elfshsect_t *plt, 
				      elfshsect_t *extplt,
				      eresi_Addr diff)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (elfsh_reencode_first_pltentry_ia32(file, plt, diff) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Reencoding of PLT+0 failed", -1);
  
  if (elfsh_reencode_first_pltentry_ia32(file, extplt, diff) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Reencoding of EXTPLT+0 failed", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Hook for ENCODEPLT on IA32 
 *
 * @param file
 * @param plt
 * @param diff
 * @param off
 * @return
 */
int		elfsh_encodeplt_ia32(elfshobj_t *file, 
				     elfshsect_t *plt, 
				     eresi_Addr diff, 
				     u_int      off)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (elfsh_reencode_pltentry_ia32(file, plt, diff, off) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Reencoding of PLT+0 failed", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Static hooking for IA32 
 * @param file
 * @param name
 * @param symbol
 * @param addr
 * @return
 */
int			elfsh_cflow_ia32(elfshobj_t	*file,
					 char		*name,
					 elfsh_Sym	*symbol,
					 eresi_Addr	addr)
{
  elfshsect_t		*hooks;
  elfshsect_t		*source;
  asm_instr		instrs[5];
  u_char		buff[32];
  u_int			ret, len;
  int			off;
  int			idx;
  char			*hookbuf;
  char			*hook;
  char			bufname[BUFSIZ];
  int			prot;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if 	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] Requesting hijack addr = %08X, sym.st_value = %08X, name = %s\n", 
	 addr, symbol->st_value, name);
#endif

  /* Resolve parameters */
  off = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  if (!off) 
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid address to hijack", -1);

  ret = elfsh_readmemf(file, off, buff, 32);
  if (ret != 32)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Function too small to be hijacked", -1);

  /* If the hook section does not exist, create it */
  hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 
				    0, 0, 0); 

  if (!hooks)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get and inject .hooks", -1);

  hook = elfsh_readmem(hooks) + hooks->curend;

  /* Determine the minimal aligned length */
  if (!proc_init)
    {
      asm_init_arch(&proc, ASM_PROC_IA32);
      proc_init = 1;
    }
  for (idx = ret = 0; ret < 5 && idx < 5; idx++)
    ret += asm_read_instr(instrs + idx, buff + ret, 32 - ret, &proc);

  /* Create the hook for this function */
  prot = elfsh_munprotect(file, (eresi_Addr) hook, 16);
  memset(hook, 0x90, 40);
  memcpy(hook, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hook + 1) = addr - (hooks->shdr->sh_addr + (hook + 1 - (char *) elfsh_readmem(hooks)) + 4);
  memcpy(hook + 5, buff, ret);
  memcpy(hook + 5 + ret, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hook + 6 + ret) = symbol->st_value - (hooks->shdr->sh_addr + hooks->curend + 10);
  elfsh_mprotect(file, (eresi_Addr) hook, 16, prot);

  /* We need to grab the parent section to compute the remaining offset */
  source = elfsh_get_parent_section_by_foffset(file, off, NULL);
  if (!source)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find parent section for hooked addr", -1);

  /* Install the hook */
#if	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] Installing hook with addr %08X (curhookent addr = %08X with curend = %u symbol.st_value %08X) \n", 
	 ((hooks->shdr->sh_addr + hooks->curend) - (symbol->st_value + 5)),
	 (hooks->shdr->sh_addr + hooks->curend), hooks->curend, symbol->st_value);
#endif

  /* Ret: the minimal instruction aligned length for installing the hook caller-side properly */
  hookbuf = alloca(ret);
  memcpy(hookbuf, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hookbuf + 1) = (hooks->shdr->sh_addr + hooks->curend) - (symbol->st_value + 5); 
  memset(hookbuf + 5, 0x90, ret - 5);
  len = elfsh_writememf(file, off, hookbuf, ret);
  if (len != ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Error during hook installation", -1);

  /* Insert the old symbol on the original saved bytes (do the insertion AFTER all use of symbol !!) */
  snprintf(bufname, BUFSIZ, "old_%s", name);
  elfsh_insert_funcsym(file, bufname, hooks->shdr->sh_addr + hooks->curend + 5,
		       ret + 5, hooks->index);
  snprintf(bufname, BUFSIZ, "hook_%s", name);
  elfsh_insert_funcsym(file, bufname, hooks->shdr->sh_addr + hooks->curend,
		       ret + 10, hooks->index);

#if	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] hook_legit_func = %08X, old_legit_func = %08X \n", 
	 hooks->shdr->sh_addr + hooks->curend, 
	 hooks->shdr->sh_addr + hooks->curend + 5);
#endif

  /* Everything OK, ret usually is 6 on x86 */
  hooks->curend += ret + 10;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/**
 * PLT hijacking on i386 for ET_DYN objects 
 * @param file
 * @param symbol
 * @param addr
 * @return
 */
int		elfsh_hijack_plt_ia32(elfshobj_t *file, 
				      elfsh_Sym *symbol,
				      eresi_Addr addr)
{
  int		foffset;
  uint8_t	opcode; 
  uint32_t	displacement;
  int		prot;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->hdr->e_machine != EM_386)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "File is not IA32", -1);

  /* Compute jmp displacement, 5 is the length of jmp opcode */
  /* Direct jmp with full displacement */
  opcode       = 0xE9;
  displacement = addr - symbol->st_value - 5;
  foffset      = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  prot         = elfsh_munprotect(file, 
				  symbol->st_value, 
				  elfsh_get_pltentsz(file));
  elfsh_writememf(file, foffset, &opcode, sizeof(opcode));

  /* Byte order test for cross-endianess ability */
#if __BYTE_ORDER == __BIG_ENDIAN
  displacement = swap32(displacement);
  elfsh_writememf(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
#else
  elfsh_writememf(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
#endif

  elfsh_mprotect(file, symbol->st_value, 
		 elfsh_get_pltentsz(file), prot);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Perform relocation on entry for INTEL architecture.
 * XXX: not endianess independant - ym 
 * @param new
 * @param cur
 * @param dword
 * @param addr
 * @param mod
 * @return
 */
int      elfsh_relocate_ia32(elfshsect_t	*new,
			     elfsh_Rel		*cur,
			     eresi_Addr		*dword,
			     eresi_Addr		addr,
			     elfshsect_t	*mod)
{
  elfsh_Shdr *section;
  elfsh_Sym  *symbol;	
  elfsh_Rel  *rel_entry;
  uint32_t   rel_offset;
  char	     *symname;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_reltype(cur))
    {

      /* Absolute reference */
    case R_386_32:
      *dword = addr;
      break;

      /* Relative reference */
    case R_386_PC32:
      *dword = addr - (new->shdr->sh_addr + cur->r_offset + 4);
      break;

    case R_386_GOT32:   
      section = elfsh_get_sht_entry_by_name(new->parent, 
					    ELFSH_SECTION_NAME_GOTPLT);
      if (section == NULL)
	{
	  section = elfsh_get_sht_entry_by_name(new->parent, 
						ELFSH_SECTION_NAME_GOT);
	  if (!section)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to find GOT for GOTPC", -1);
	}
      
      //*dword = cur->r_offset - section->sh_addr;
      symname = elfsh_get_symname_from_reloc(mod->parent, cur);
      if (symname == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find symbol in host", -1);
      rel_entry = elfsh_get_relent_by_name(new->parent, symname);
      if (rel_entry == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to find relocation in host", -1);
      *dword = elfsh_get_reloffset(rel_entry) - section->sh_addr;


      break;
    
    case R_386_PLT32:
      if (elfsh_static_file(new->parent))
	{
#if	__DEBUG_RELADD__      
	  printf("[DEBUG_RELADD] R_386_PLT32 on STATIC file : %s \n", 
		 new->parent->name);
#endif      
	  symname = elfsh_get_symname_from_reloc(mod->parent, cur); 
#if	__DEBUG_RELADD__      
	  printf("[DEBUG_RELADD] R_386_PLT32 : %s\n", symname); 
#endif	  
	  if (symname == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find symbol name", -1);
	  

	  symbol = elfsh_get_symbol_from_reloc(mod->parent, cur);
	  if (symbol == NULL)
	   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find symbol", -1);
	  *dword = symbol->st_value + addr - (new->shdr->sh_addr + cur->r_offset + 4);
	}
      else
	{
	  section = elfsh_get_sht_entry_by_name(new->parent, 
						ELFSH_SECTION_NAME_PLT);
	  if (section == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find PLT fot PLT32", -1);
	
#if	__DEBUG_RELADD__        
	  printf("[DEBUG_RELADD] R_386_PLT32: *dword value was %08X \n", *dword);
#endif

	  /* Basicaly the same */
	  *dword = addr - (new->shdr->sh_addr + cur->r_offset + 4);
	  
#if	__DEBUG_RELADD__        
	  printf("[DEBUG_RELADD] R_386_PLT32 on DYNAMIC file : set dword[*%08X] = %08X (reach PLT entry %08X) \n", 
		 new->shdr->sh_addr + cur->r_offset, *dword, addr);
#endif

	}
      break;

    case R_386_GOTOFF:
      section = elfsh_get_sht_entry_by_name(new->parent, 
					    ELFSH_SECTION_NAME_GOTPLT);
      if (section == NULL)
	{
	  section = elfsh_get_sht_entry_by_name(new->parent, 
						ELFSH_SECTION_NAME_GOT);
	  if (!section)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to find GOT for GOTPC", -1);
	}
      *dword = addr - section->sh_addr;
      break;

    case R_386_GOTPC:
      section = elfsh_get_sht_entry_by_name(new->parent, 
					    ELFSH_SECTION_NAME_GOTPLT);
      if (section == NULL)
	{
	  section = elfsh_get_sht_entry_by_name(new->parent, 
						ELFSH_SECTION_NAME_GOT);
	  if (!section)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to find GOT for GOTPC", -1);
	}

      rel_offset = (elfsh_get_ostype(new->parent) == ELFSH_OS_BEOS ? 3 : 2);
      *dword = section->sh_addr - (new->shdr->sh_addr + cur->r_offset) + rel_offset;
      
#if	__DEBUG_RELADD__        
      printf("[DEBUG_RELADD] R_386_GOTPC : *dword=%08X, non added *dword was %08X \n", 
	     section->sh_addr - (new->shdr->sh_addr + cur->r_offset) + rel_offset, *dword);
#endif

      break;

    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unsupported relocation type", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Personnal func / define for args_count 
 *
 * @param op
 * @param regbased
 * @return
 */
static int    elfsh_ac_is_arg_ebp(asm_operand *op, int regbased)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* imm value must be superior to 4 because it is return addr 
     malloc() use to look at the return address which make arg count
     think that malloc should take 2 arguments.
   */
  if (op->baser == regbased && op->imm > 4)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, op->imm);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

static int    elfsh_ac_is_arg_esp(asm_operand *op, int sub)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (op->baser == ASM_REG_ESP && op->imm > sub)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, op->imm - sub);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 *
 * @param add
 * @return
 */
static int    elfsh_ac_largs_add(int add)
{
  u_int                 index;
#if __DEBUG_ARG_COUNT__
  u_int			z;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (add == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid argument", -1);

  /* Empty list or next value is > to current max_arg_offset */
  if ((arg_count == 0 && args[arg_count] == 0)
      || max_arg_offset < add)
    {
#if __DEBUG_ARG_COUNT__
      printf("[DEBUG_ARG_COUNT] MAX OR FIRST = %d\n", add);
#endif

      max_arg_offset = add;
      args[arg_count++] = add;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Already got it */
  if (max_arg_offset == add)
    {
#if __DEBUG_ARG_COUNT__
      printf("[DEBUG_ARG_COUNT] ALREADY = %d\n", add);
#endif

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  for (index = 0; index < ELFSH_TRACE_MAX_ARGS; index++)
    {
      /* Already add ? */
      if (args[index] == add)
	{
#if __DEBUG_ARG_COUNT__
	  printf("[DEBUG_ARG_COUNT] ALREADY = %d\n", add);
#endif

	  break;
	}

      /* We sort result */
      if (args[index] > add)
	{
	  if (arg_count == ELFSH_TRACE_MAX_ARGS)
	    {
#if __DEBUG_ARG_COUNT__
	      printf("[DEBUG_ARG_COUNT] MAX ARGUMENT\n");
#endif

	      arg_count--;
	    }

#if __DEBUG_ARG_COUNT__
	  printf("[DEBUG_ARG_COUNT] Next values (b):\n");

	  for (z = index; z < arg_count; z++)
	    printf("[DEBUG_ARG_COUNT] :%d -> %d\n", z, args[z]);
#endif

	  /* Move arguments */
	  memmove(&args[index + 1], &args[index], (arg_count - index)*sizeof(int));

#if __DEBUG_ARG_COUNT__
	  printf("[DEBUG_ARG_COUNT] Next values (a):\n");

	  for (z = index; z < arg_count + 1; z++)
	    printf("[DEBUG_ARG_COUNT] :%d -> %d\n", z, args[z]);
#endif

	  args[index] = add;
	  arg_count++;
	  break;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/*
Used with Forward analysis
TODO: Keep it ?
static eresi_Addr elfsh_ac_foundcallto(elfshobj_t *file, eresi_Addr vaddr, eresi_Addr *before)
{
  u_int 	index;
  asm_instr   	i;
  int		ret;
  elfsh_SAddr	foffset;
  elfsh_Word	len;
  elfshsect_t	*text;
  char		*data;
  eresi_Addr	base_vaddr, last_vaddr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  text = elfsh_get_parent_section(file, elfsh_get_entrypoint(file->hdr), &foffset);
  if (!text)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find parent section from entry point", 0);
  
  if (!elfsh_get_anonymous_section(file, text))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to get an anonymous section", 0);
  
  data = elfsh_readmem(text);
  len = text->shdr->sh_size;

  base_vaddr = (elfsh_is_runtime_mode() && !elfsh_section_is_runtime(text) ?
		file->rhdr.base + elfsh_get_section_addr(text->shdr) :
		elfsh_get_section_addr(text->shdr));

  last_vaddr = base_vaddr;

  for (index = 0; index < len; index += ret)
    {
    // Read an instruction 
      if ((ret = asm_read_instr(&i, (u_char *) (data + index), len -  index, &proc)))
	{
	// Search a call to our address (near call) 
	  if (i.instr == ASM_CALL)
	    {
	      if (base_vaddr + index + i.op[0].imm + i.len == vaddr)
		{
		  if (before)
		    *before = last_vaddr;

		  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, base_vaddr + index);
		}
	      last_vaddr = base_vaddr + index;
	    }
	  else if (i.instr == ASM_RET)
	    {
	      last_vaddr = base_vaddr + index;
	    }
	}

      if (ret <= 0)
	ret = 1;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
*/

/**
 *
 * @param file
 * @param vaddr
 * @return
 */
static char	*elfsh_ac_get_sect_ptr(elfshobj_t *file, eresi_Addr vaddr)
{
  elfshsect_t	*sect;
  elfsh_SAddr	foffset;
  char		*buf = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sect = elfsh_get_parent_section(file, vaddr, &foffset);

  if (sect)
    {
      buf = elfsh_readmem(sect);
      buf += vaddr - (sect->parent->rhdr.base + sect->shdr->sh_addr);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}


/**
 * TODO: implement forward / backward 
 *
 * @param file
 * @param foffset
 * @param vaddr
 * @return
 */
int           	*elfsh_args_count_ia32(elfshobj_t *file, u_int foffset, eresi_Addr vaddr)
{
  int         	index;
  int         	ret;
  int	      	reserv = 0;
  int	      	ffp = 1;
  const int    	asm_len = 1024;
  //int         	len = 0;
  int	      	*final_args;
  //eresi_Addr  	f_vaddr, up_vaddr;
  asm_instr   	i;
  char      	*data;
  int		op;
  u_char	prolog_setup = 0;
  u_int		icount, regicount;
  int		regbased = -1;
  int		next_regbased = -1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!proc_init)
    {
      asm_init_arch(&proc, ASM_PROC_IA32);
      proc_init = 1;
    }
  max_arg_offset = 0;

  /* Find the real function if this vaddr point to plt section */
  elfsh_resolv_remote_function(file, vaddr, &file, &vaddr);

#if __DEBUG_ARG_COUNT__
  printf("[DEBUG_ARG_COUNT] Arg count into file: %s with addr: 0x%x \n", file->name, vaddr);
#endif

  data = elfsh_ac_get_sect_ptr(file, vaddr);

  if (!data)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

  /* Prepare args */
  memset(args, 0x00, (ELFSH_TRACE_MAX_ARGS+1)*sizeof(int));
  arg_count = 0;

  reserv = 0;
  regicount = 0;

  /* Enumerate all arguments */
  for (icount = index = 0; index < asm_len && data; index += ret, icount++)
    {
      /* Read an instruction */
      if ((ret = asm_read_instr(&i, (u_char *) (data + index), asm_len -  index, &proc)) > 0)
	{
	  /* After 10 instruction, 
	     no prolog anlysis should be done */
	  if (!prolog_setup && icount == 10)
	    prolog_setup = 1;

	  /* We don't want to read another function */
	  if (i.instr == ASM_RET)
	    {
#if __DEBUG_ARG_COUNT__
	      printf("[DEBUG_ARG_COUNT] RET @ +%d (%x)\n", index, index);
#endif
	      break;
	    }

	  /* Seek if we are near a ebp like prolog */
	  if (!prolog_setup)
	    {
	      if (ELFSH_IA32_PUSH_REGBASE(i))
		{
		  /* We register pushed register and go to see if 
		     next instruction will be a mov $reg, $esp (look after) 

		     This detection algorithm has been update for getopt()
		     where I found a dispositin like this:

		     push $ebp
		     xor $eax, $eax
		     mov $ebp, $esp
		  */
		  regbased = i.op[0].baser;
		  regicount = 0;
		}
	      else if (regbased != -1)
		{
		  /* mov %reg, %esp */
		  if (ELFSH_IA32_MOV_REGBASE(i, regbased))
		    {
#if __DEBUG_ARG_COUNT__
		      printf("[DEBUG_ARG_COUNT] EBP like based found @ +%d (%x) / reg = %d\n", 
			     index, index, regbased);
#endif

		      /* We have a ebp like register, then prolog is found
		         and now we will see for argument based on it */
		      prolog_setup = 1;
		      ffp = 0;
		      continue;
		    }

		  /* We use a counter to control how many instruction can be behind
		     the push & the mov */
		  if (regicount < ELFSH_IA32_PROLOG_MSTEP)
		    regicount++;
		  else
		    regbased = -1; /* Reset based register */
		}
	    }
	  else
	    {
	      /* TODO: Find a better way to find function end */
	      if (ELFSH_IA32_PUSH_REGBASE(i))
		{
		  /* This time we try to see if we reach another function
		     Some function does not rely on ret 
		     This fix has been made for my exit() function which did not
		     has a ret, then we start parsing on_exit() */
		  next_regbased = i.op[0].baser;
		  regicount = 0;
		}
	      else
		{
		  /* If its true, we reached another function */
		  if (ELFSH_IA32_MOV_REGBASE(i, next_regbased))
		    break; 

		  if (regicount < ELFSH_IA32_PROLOG_MSTEP)
		    regicount++;
		  else
		    next_regbased = -1; /* Reset based register */
		}
	      
	    }

	  if (ffp == 0)
	    {
	      /* EBP based argument */
	      op = elfsh_ac_is_arg_ebp(&i.op[0], regbased);
		  
	      if (op > 0)
		{
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] EBP (1) @ +%d (%x) - %d\n", index, index, op);
#endif
		  elfsh_ac_largs_add(op);
		}
		  
	      op = elfsh_ac_is_arg_ebp(&i.op[1], regbased);

	      if (op > 0)
		{
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] EBP (2) @ +%d (%x) - %d\n", index, index, op);
#endif
		  elfsh_ac_largs_add(op);
		}

#if __DEBUG_ARG_COUNT__
	      printf("[DEBUG_ARG_COUNT] @ +%d (%x) i.instr = %d\n", index, index, i.instr);
#endif
	    }
	  else
	    {
	      /* sub $esp, x */
	      if (i.instr == ASM_SUB && i.op[0].baser == ASM_REG_ESP)
		{
		  reserv += i.op[1].imm;

#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] reserv @ +%d (%x) += %d / %d\n", index, index,
			 i.op[1].imm, reserv);
#endif
		  continue;
		}
	      /* add $esp, x */
	      else if (i.instr == ASM_ADD && i.op[0].baser == ASM_REG_ESP)
		{
		  reserv -= i.op[1].imm;

#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] reserv @ +%d (%x) -= %d / %d\n", index, index,
			 i.op[1].imm, reserv);
#endif
		  continue;
		}

	      /* ESP based argument */
	      op = elfsh_ac_is_arg_esp(&i.op[0], reserv);

	      if (op > 0)
		{
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] ESP (1) @ +%d (%x) - %d\n", index, index, op);
#endif
		  elfsh_ac_largs_add(op);
		}

	      op = elfsh_ac_is_arg_esp(&i.op[1], reserv);

	      if (op > 0)
		{
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] ESP (2) @ +%d (%x) - %d\n", index, index, op);
#endif
		  elfsh_ac_largs_add(op);
		}

	      /* On esp based functions, sometimes compiler optimisation is to not
	         clear argument after a call (just at the end). Then we need to update
	         stack state. Problem found in setupterm() */
	      if (i.instr == ASM_PUSH) 
		{
		  reserv += sizeof(eresi_Addr);
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] reserv @ +%d (%x) += %d / %d\n", index, index,
			 sizeof(eresi_Addr), reserv);
#endif
		}
	      else if (i.instr == ASM_POP)
		{
		  reserv -= sizeof(eresi_Addr);
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] reserv @ +%d (%x) -= %d / %d\n", index, index,
			 sizeof(eresi_Addr), reserv);
#endif
		}
	      else
		{
#if __DEBUG_ARG_COUNT__
		  printf("[DEBUG_ARG_COUNT] @ +%d (%x) i.instr = %d\n", index, index, i.instr);
#endif
		}
	    }
	}
      else
	break;

      ret = asm_instr_len(&i);
      if (!ret)
	ret++;
    }

  /* Do we have something ? */
  /*
  if (arg_count > 0)
    goto setargs;
  */

  /**
   * Backward anlysis
   * We analyse a call to a specific function
   * TODO: Keep this part ? at least optimise it. Seeking a call everytime is too long

  f_vaddr = elfsh_ac_foundcallto(file, vaddr, &up_vaddr);
      
  if (f_vaddr > 0)
    {
      data = elfsh_ac_get_sect_ptr(file, up_vaddr);
      len = f_vaddr - up_vaddr;

      for (index = 0; index < len && data ; index += ret)
	{
	  // Read an instruction 
	  if ((ret = asm_read_instr(&i, (u_char *) (data + index), len -  index, &proc)))
	    {
	      // We don't want to read another function 
	      if (i.instr == ASM_MOV && i.op[0].baser == ASM_REG_ESP)
		elfsh_ac_largs_add(i.op[0].imm + 16);
	    }

	  ret = asm_instr_len(&i);
	  if (!ret)
	    ret++;
	}
    }


  setargs:
  */

  /* No arguments */
  if (arg_count == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't find arguments", NULL);

  /* Add first offset 
     Sometimes first argument is not use (__xstat64() case)
     which creates a bug into arguments number 
     Offset 8 should always be first argument */
  elfsh_ac_largs_add(0x8);

  for (index = 0; index < ELFSH_TRACE_MAX_ARGS && index < arg_count; index++)
    {
      /* Last entry */
      if (index + 1 == arg_count)
	{
	  /* Can't know last argument size, presume eresi_Addr
	     size. This issue seems impossible to resolve without
	     an advanced analyse engine */
	  args[index] = (int) sizeof(eresi_Addr);
	}
      else
	{
	  /* Argument size is next_offset - my_offset */
	  args[index] = args[index + 1] - args[index];
	}

      /* Aligned on address size (at least) */
      if (args[index] < (int) sizeof(eresi_Addr)
	  || args[index] > ELFSH_IA32_ARG_MAXSIZE)
	args[index] = (int) sizeof(eresi_Addr);
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__, final_args, 
	 (arg_count + 1) * sizeof(int), NULL);

  /* Copy current arguments */
  memcpy(final_args, args, (arg_count + 1) * sizeof(int));

#if __DEBUG_ARG_COUNT__
  printf("[DEBUG_ARG_COUNT] arg_count = %d\n[DEBUG_ARG_COUNT] RETURN LIST:\n", arg_count);

  for (index = 0; index < arg_count + 1; index++)
    printf("[DEBUG_ARG_COUNT] [%02d] %d\n", index, final_args[index]);
#endif
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, final_args);
}
