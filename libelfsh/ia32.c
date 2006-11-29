/*
** ia32.c for libelfsh
**
** Started on  Fri Jan 11 03:05:37 2003 mayhem
**
*/
#include "libelfsh.h"
#include "libasm.h"



static asm_processor	proc;


/* Start of hook code for EXTPLT */
int		elfsh_extplt_ia32(elfshsect_t *extplt, elfshsect_t *altgot,
				  elfshsect_t *dynsym, elfshsect_t *relplt)
{
  int		prot;
  int		*reloc;
  elfsh_Addr	gotent;
  elfsh_Rel	r;
  char		*ent;
  u_int		relentsz;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* 6 is the size of the first instruction of a .plt entry on x86 */
  gotent = extplt->shdr->sh_addr + extplt->curend + 6;
  memcpy(elfsh_get_raw(altgot) + altgot->curend, 
	 (char *) &gotent, sizeof(gotent));
  altgot->curend += sizeof(gotent);
 
#if __DEBUG_EXTPLT__
  printf("[DEBUG_EXTPLT] Set ALTGOT entry " XFMT " to value " XFMT "\n", 
	 altgot->shdr->sh_addr + altgot->curend, gotent);
#endif

  /* 7 is the offset for the relocation offset location, encoded in GOT */
  ent = elfsh_get_raw(extplt) + extplt->curend;
  reloc  = (int *) ((char *) ent + 7);
  prot = elfsh_munprotect(extplt->parent, 
			  (elfsh_Addr) reloc, 
			  (char *) reloc - (char *) elfsh_get_raw(extplt));
  *reloc = relplt->curend;
  elfsh_mprotect((elfsh_Addr) reloc, 
		 (char *) reloc - (char *) elfsh_get_raw(extplt), prot);
  extplt->curend += elfsh_get_pltentsz(extplt->parent);

  /* Insert relocation entry pointing on the new PLT entry */
  relentsz = IS_REL(extplt) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
  r = elfsh_create_relent(R_386_JMP_SLOT, dynsym->curend / sizeof(elfsh_Sym), 
			  altgot->shdr->sh_addr + altgot->curend - sizeof(elfsh_Addr));
  memcpy(elfsh_get_raw(relplt) + relplt->curend, &r, relentsz);
  relplt->curend += relentsz;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* On IA32 we need to reencode the PLT so that it uses the .alt.got instead of .got */
/* Should work on both ET_EXEC and ET_DYN (similar encoding offsets even if different jmp) */
int		elfsh_reencode_pltentry_ia32(elfshobj_t   *file, 
					     elfshsect_t  *plt, 
					     uint32_t     diff, 
					     u_int	     off)
{
  char		*pltent;
  uint32_t	*got;
  elfshsect_t	*relplt;
  u_int		resoff;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || plt == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  /* 12 is the size of 3 reserved PLT entries on x86 */
  resoff = (!strcmp(plt->name, ELFSH_SECTION_NAME_EXTPLT) ? 12 : 0);

  /* Add ALTGOT addr difference to the GOT offset encoded in PLT */
  pltent = plt->data + off + 2;
  got = (uint32_t *) pltent;
  *got += diff - resoff;

  /* If thats EXTPLT we are reencoding, shift the pushed reloc offsets as well */
  if (!strcmp(plt->name, ELFSH_SECTION_NAME_EXTPLT))
    {
      relplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELPLT, 0, 0, 0);
      if (relplt == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot retreive .rel.plt", -1);
      
      pltent = plt->data + off + 7;
      got  = (uint32_t *) pltent;
      *got = relplt->shdr->sh_size;
    }
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* On IA32 we need to reencode the PLT so that it uses the .alt.got instead of .got */ 
/* Should work on both ET_EXEC and ET_DYN */
int		elfsh_reencode_first_pltentry_ia32(elfshobj_t  *file, 
						   elfshsect_t *plt, 
						   uint32_t	diff)
{
  char		*pltent;
  uint32_t	*got;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL || plt == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
      
  /* The first GOT address is at offset .plt+2 on IA32 */
  pltent = plt->data + 2;
  got = (uint32_t *) pltent;
  *got += diff;
  
  /* The second GOT address is at offset .plt+8 on IA32 */
  pltent = plt->data + 8;
  got = (uint32_t *) pltent;
  *got += diff;		

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Hook for ENCODEPLT1 on IA32 : Simple wrapper for both calls */
int		elfsh_encodeplt1_ia32(elfshobj_t *file, 
				      elfshsect_t *plt, 
				      elfshsect_t *extplt,
				      elfsh_Addr diff)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (elfsh_reencode_first_pltentry_ia32(file, plt, diff) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Reencoding of PLT+0 failed", -1);
  
  if (elfsh_reencode_first_pltentry_ia32(file, extplt, diff) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Reencoding of EXTPLT+0 failed", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Hook for ENCODEPLT on IA32 */
int		elfsh_encodeplt_ia32(elfshobj_t *file, 
				     elfshsect_t *plt, 
				     elfsh_Addr diff, 
				     u_int      off)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (elfsh_reencode_pltentry_ia32(file, plt, diff, off) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Reencoding of PLT+0 failed", -1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Get the next frame pointer given the current one */
void		*elfsh_bt_ia32(void *frame)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!frame)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     ((void *) *(long*) frame));
}

/* Get the return address giving the current frame pointer */
void		*elfsh_getret_ia32(void *frame)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  
  if (!frame)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (void *) (*((long *) frame + 1)));
}


/* Write a breakpoint 0xCC in memory */
/* One of the 2 breakpoint technique of e2dbg */
int			elfsh_break_ia32(elfshobj_t *f,
					 elfshbp_t  *bp)
{
  int			prot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_BREAKPOINTS__
  printf("[DEBUG_BREAKPOINTS:ia32] bp->addr %08X \n", bp->addr);
#endif

  bp->savedinstr[0] = (*(char *) bp->addr);
  prot = elfsh_munprotect(f, bp->addr, 4);
  if (prot == (-1))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Munprotect failed", (-1));
  
#if __DEBUG_BREAKPOINTS__
  printf("[DEBUG_BREAKPOINTS:ia32] after munprotect\n");
#endif
  (*(char *) bp->addr) = 0xCC;
#if __DEBUG_BREAKPOINTS__
  printf("[DEBUG_BREAKPOINTS:ia32] after write\n");
#endif
  elfsh_mprotect(bp->addr, 4, prot);
#if __DEBUG_BREAKPOINTS__
  printf("[DEBUG_BREAKPOINTS:ia32] after mprotect\n");
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}



/* Static hooking for IA32 */
int			elfsh_cflow_ia32(elfshobj_t	*file,
					 char		*name,
					 elfsh_Sym	*symbol,
					 elfsh_Addr	addr)
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if 	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] Requesting hijack addr = %08X, sym.st_value = %08X, name = %s\n", 
	 addr, symbol->st_value, name);
#endif

  /* Resolve parameters */
  off = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  if (!off) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid address to hijack", -1);

  ret = elfsh_raw_read(file, off, buff, 32);
  if (ret != 32)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Function too small to be hijacked", -1);

  /* If the hook section does not exist, create it */
  hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 
				    0, 0, 0); 

  if (!hooks)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get and inject .hooks", -1);

  hook = elfsh_get_raw(hooks) + hooks->curend;

  /* Determine the minimal aligned length */
  asm_init_i386(&proc);
  for (idx = ret = 0; ret < 5 && idx < 5; idx++)
    ret += asm_read_instr(instrs + idx, buff + ret, 32 - ret, &proc);

  /* Create the hook for this function */
  prot = elfsh_munprotect(file, (elfsh_Addr) hook, 16);
  memset(hook, 0x90, 40);
  memcpy(hook, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hook + 1) = addr - (hooks->shdr->sh_addr + (hook + 1 - (char *) elfsh_get_raw(hooks)) + 4);
  memcpy(hook + 5, buff, ret);
  memcpy(hook + 5 + ret, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hook + 6 + ret) = symbol->st_value - (hooks->shdr->sh_addr + hooks->curend + 10);
  elfsh_mprotect((elfsh_Addr) hook, 16, prot);

  /* We need to grab the parent section to compute the remaining offset */
  source = elfsh_get_parent_section_by_foffset(file, off, NULL);
  if (!source)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find parent section for hooked addr", -1);

  /* Install the hook */
#if	__DEBUG_CFLOW__      
  printf("[DEBUG_CFLOW] Installing hook with addr %08X (curhookent addr = %08X with curend = %u symbol.st_value %08X) \n", 
	 ((hooks->shdr->sh_addr + hooks->curend) - (symbol->st_value + 5)),
	 (hooks->shdr->sh_addr + hooks->curend), hooks->curend, symbol->st_value);
#endif

  /* Ret: the minimal instruction aligned length for installing the hook caller-side properly */
  hookbuf = alloca(ret);

  //prot = elfsh_munprotect(hookbuf, 16);
  memcpy(hookbuf, "\xe9\x00\x00\x00\x00", 5);
  *(uint32_t *) ((char *) hookbuf + 1) = (hooks->shdr->sh_addr + hooks->curend) - (symbol->st_value + 5); 
  memset(hookbuf + 5, 0x90, ret - 5);
  len = elfsh_raw_write(file, off, hookbuf, ret);
  if (len != ret)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Error during hook installation", -1);
  //elfsh_mprotect(hookbuf, 16, prot);

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
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* PLT hijacking on i386 for ET_DYN objects */
int		elfsh_hijack_plt_ia32(elfshobj_t *file, 
				      elfsh_Sym *symbol,
				      elfsh_Addr addr)
{
  int		foffset;
  uint8_t	opcode; 
  uint32_t	displacement;
  int		prot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->hdr->e_machine != EM_386)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "File is not IA32", -1);

  /* Compute jmp displacement, 5 is the length of jmp opcode */
  /* Direct jmp with full displacement */
  opcode       = 0xE9;
  displacement = addr - symbol->st_value - 5;
  foffset      = elfsh_get_foffset_from_vaddr(file, symbol->st_value);
  prot         = elfsh_munprotect(file, 
				  symbol->st_value, 
				  elfsh_get_pltentsz(file));
  elfsh_raw_write(file, foffset, &opcode, sizeof(opcode));

  /* Byte order test for cross-endianess ability */
#if __BYTE_ORDER == __BIG_ENDIAN
  displacement = swap32(displacement);
  elfsh_raw_write(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
#else
  elfsh_raw_write(file, foffset + sizeof(opcode), 
		  &displacement, sizeof(displacement));
#endif

  elfsh_mprotect(symbol->st_value, 
		 elfsh_get_pltentsz(file), prot);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Perform relocation on entry for INTEL architecture */
/* XXX: not endianess independant - ym */
int      elfsh_relocate_ia32(elfshsect_t	*new,
			     elfsh_Rel		*cur,
			     elfsh_Addr		*dword,
			     elfsh_Addr		addr,
			     elfshsect_t	*mod)
{
  elfsh_Shdr *section;
  elfsh_Sym  *symbol;	
  char	     *symname;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
   
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
      section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_GOT);
      if (section == NULL)
         ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to find GOT for GOTOFF", -1);
      *dword = cur->r_offset - section->sh_addr;
      break;
    
    case R_386_PLT32:
      if (elfsh_static_file(new->parent))
	{
#if	__DEBUG_RELADD__      
	  printf("[DEBUG_RELADD] R_386_PLT32 on STATIC file : %s \n", new->parent->name);
#endif      
	  symname = elfsh_get_symname_from_reloc(mod->parent, cur); 
#if	__DEBUG_RELADD__      
	  printf("[DEBUG_RELADD] R_386_PLT32 : %s\n", symname); 
#endif	  
	  if (symname == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find symbol name", -1);
	  

	  symbol = elfsh_get_symbol_from_reloc(mod->parent, cur);
	  if (symbol == NULL)
	   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to find symbol", -1);
	  *dword = symbol->st_value + addr - (new->shdr->sh_addr + cur->r_offset + 4);
	}
      else
	{
	  section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_PLT);
	  if (section == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
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
      section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_GOT);
      if (section == NULL)
         ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to find GOT for GOTOFF", -1);
      *dword = addr - section->sh_addr;
      break;

    case R_386_GOTPC:
      section = elfsh_get_sht_entry_by_name(new->parent, ELFSH_SECTION_NAME_GOT);
      if (section == NULL)
         ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                        "Unable to find GOT for GOTPC", -1);

      *dword = section->sh_addr - (new->shdr->sh_addr + cur->r_offset) + 2;

#if	__DEBUG_RELADD__        
      printf("[DEBUG_RELADD] R_386_GOTPC : *dword=%08X, non added *dword was %08X \n", 
	     section->sh_addr - (new->shdr->sh_addr + cur->r_offset) + 2, *dword);
#endif

      break;

    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unsupported relocation type", -1);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

typedef struct s_int
{
  int		value;
  struct s_int  *prec;
  struct s_int	*next;
}		s_sint;

/* Personnal func / define for args_count */
static int    elfsh_is_arg_ebp(asm_operand *op)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (op->base_reg == ASM_REG_EBP && op->imm > 0)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, op->imm);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

static int    elfsh_is_arg_esp(asm_operand *op, int sub)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (op->base_reg == ASM_REG_ESP && op->imm > sub)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, op->imm);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

static int    elfsh_largs_add(s_sint *args, int add)
{
  s_sint	      *p, *n;
  int                 i;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (add == 0 || args == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  if (args->value == 0)
    {
      args->value = add;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* First entry */
  while (args->prec)
    args = args->prec;

  for (i = 0, p = args; i < ELFSH_TRACE_MAX_ARGS 
	 && p != NULL; i++, p = p->next)
    {
      /* Already add ? */
      if (p->value == add)
	break;

      /* We sort result */
      if (p->value > add)
	{
	  XALLOC(n, sizeof(s_sint), -1);
	  n->value = add;
	  
	  n->prec = p->prec;
	  p->prec = n;

	  n->next = p;
	  break;
	}

      /* Add at the tail */
      if (p->next == NULL)
	{
	  XALLOC(n, sizeof(s_sint), -1);
	  n->value = add;
	  
	  p->next = n;
	  n->prec = p;
	  break;
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* TODO: implement forward / backward */
int           *elfsh_args_count_ia32(elfshobj_t *file, u_int foffset, elfsh_Addr vaddr)
{
  int         index;
  int         ret;
  int	      reserv = 0;
  int	      ffp = 0;
  int         len = 1024;
  s_sint      *args = NULL, *p = NULL;
  int	      *final_args;
  asm_instr   i;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Reset args */
  XALLOC(final_args, ELFSH_TRACE_MAX_ARGS * sizeof(int), NULL);

  asm_init_i386(&proc);

  /* Enumerate all arguments */
  for (index = 0; index < len; index += ret)
    {
      /* Read an instruction */
      if ((ret = asm_read_instr(&i, (u_char *) (foffset + index), len -  index, &proc)))
	{
	  /* We don't want to read another function */
	  if (i.instr == ASM_RET)
	    break;

	  /* Check init form of the function */
	  if (index == 0)
	    {
	      /* %esp based (-fomit-frame-pointer) */
	      if (i.instr == ASM_SUB && i.op1.base_reg == ASM_REG_ESP)
		{
		  reserv = i.op2.imm;
		  ffp = 1;
		}

	      XALLOC(args, sizeof(s_sint), NULL);
	      args->value = 0;
	    }
	  else
	    {
	      if (ffp == 0)
		{
		  /* EBP based argument */
		  elfsh_largs_add(args, elfsh_is_arg_ebp(&i.op1));
		  elfsh_largs_add(args, elfsh_is_arg_ebp(&i.op2));
		}
	      else
		{
		  /* ESP based argument */
		  elfsh_largs_add(args, elfsh_is_arg_esp(&i.op1, reserv));
		  elfsh_largs_add(args, elfsh_is_arg_esp(&i.op2, reserv));
		}
	    }
	}
      else
	break;
    }

  /* Go at the end */
  while (args->next)
    args = args->next;

  final_args[0] = NULL;

  /* Fill structure to return */
  for (index = 0, p = args; index < ELFSH_TRACE_MAX_ARGS
	 && p != NULL && p->value > 0; index++, p = p->prec)
    {
      /* Last entry */
      if (p->next == NULL)
	{
	  if (ffp)
	    final_args[index] = reserv - p->value + 20;
	  else
	    {
	      final_args[index] = p->value - 16;

	      /* XXX: wrong readed argument */
	      if (final_args[index] < 0)
		final_args[index] = 4;
	    }

	  continue;
	}

      /* Stock argument size which depend of the next entrie */
      final_args[index] = p->next->value - p->value;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, final_args);
}

/* Find arguments from a call */
int 		elfsh_args_count_forward(elfshobj_t *obj, u_int func, u_int call)
{
  u_int		current;
  u_int		ret;
  u_int		addr = 0;
  short		safeCheck = 0;
  short		foundBreak = 0;
  asm_instr	i;
  u_int		diff = func - call;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if we found the call just after saved registers */
  for (current = func; current < diff; current += ret)
    {
      ret = asm_read_instr(&i, (u_char *) current, diff - current, &proc);
      
      /* Wrong disassemble */
      if (!ret)
	break; // Return an error ? We should be not for the moment
      
      /* Those instruction make our system much more easier ! */
      if (i.type == ASM_TYPE_IMPBRANCH ||
	  i.type == ASM_TYPE_CONDBRANCH ||
	  i.type == ASM_TYPE_CALLPROC)
	{
	  foundBreak = 1;
	  
	  /* Save last separate addr */
	  addr = current;
	}
    }
  
  /* We "directly" call our function without any difference from register save */
  if (!foundBreak)
    safeCheck = 1;
  
  /* saveCheck found last save register push */
  if (safeCheck)
    {
      /* TODO: check */
    }

  /* TODO: add a check on addr if needed ! */

  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
