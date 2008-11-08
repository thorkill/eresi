#include "kedbg.h"
#include "interface.h"


/* /\**************** Handler for vectors ****************\/ */
void            kedbg_resetstep_ia32(void)
{
  e2dbgworld.curthread->step = FALSE;
}


eresi_Addr	*kedbg_getfp_ia32(void)
{
  fprintf(stderr, "%s NOT IMPLEMENTED (YET) \n", __PRETTY_FUNCTION__);
  fflush(stdout);
  
  /*   gdbwrap_t     *loc = gdbwrap_current_get(); */

/*   /\* First update all the reg. *\/ */
/*   gdbwrap_readgenreg(loc); */
/*   return (eresi_Addr *)loc->reg32.ebp; */
  return NULL;
}


void            *kedbg_bt_ia32(void *frame)
{

  NOT_USED(frame);
  fprintf(stderr, "%s NOT IMPLEMENTED (YET) \n", __PRETTY_FUNCTION__);
  fflush(stdout);
  return NULL;
}


void            *kedbg_getret_ia32(void *frame)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  return gdbwrap_readmemory(*(la32*)frame, DWORD_IN_BYTE, loc);
}


/**
 * Set up a breakpoint. We have to change the memory on the server
 * side, thus we need to save the value we change.
 */
int             kedbg_setbp(elfshobj_t *f, elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  
  NOT_USED(f);
  gdbwrap_setbp(bp->addr, bp->savedinstr, loc);
  
  return 0;
}


int             kedbg_delbp(elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  printf("ASDKJASDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
  gdbwrap_delbp(bp->addr, bp->savedinstr, loc);
  return 0;
}


void            kedbg_print_reg(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  
  e2dbg_register_dump("EAX", loc->reg32.eax);
  e2dbg_register_dump("EBX", loc->reg32.ebx);
  e2dbg_register_dump("ECX", loc->reg32.ecx);
  e2dbg_register_dump("EDX", loc->reg32.edx);
  e2dbg_register_dump("ESI", loc->reg32.esi);
  e2dbg_register_dump("EDI", loc->reg32.edi);
  e2dbg_register_dump("ESP", loc->reg32.esp);
  //E2DBG_SETREG(E2DBG_SSP_VAR, loc->reg32.eax);
  e2dbg_register_dump("EBP", loc->reg32.ebp);
  e2dbg_register_dump("EIP", loc->reg32.eip);
}

void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr,
				void *buf, unsigned size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  char          *ret;
  char          *charbuf = buf;
  unsigned      i;
  
  NOT_USED(file);
  ret = gdbwrap_readmemory(addr, size, loc);

  /* gdbserver sends a string, we need to convert it. Note that 2
     characters = 1 real Byte.*/
  for (i = 0; i < size; i++) 
    charbuf[i] = (u_char)gdbwrap_atoh(ret + 2 * i, 2);

  //  printf("Returning: %s\n", charbuf);
  /* returning a char is ok ? */
  return charbuf;
}


void            *kedbg_readmem(elfshsect_t *sect)
{
  void *ptr;

  if(!elfsh_get_section_writableflag(sect->shdr))
    ptr = sect->data;
  else
    {
      ptr = malloc(sect->shdr->sh_size);
      kedbg_readmema(sect->parent, sect->shdr->sh_addr, ptr, sect->shdr->sh_size);
    }

  return ptr;
}


int             kedbg_writemem(elfshobj_t *file, eresi_Addr addr, void *data,
			       unsigned size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  NOT_USED(file);
  gdbwrap_writememory(addr, data, size, loc);

  return 0;
}


/* Get %IP */
eresi_Addr      *kedbg_getpc_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  return (eresi_Addr *)&loc->reg32.eip;
}

  
void            kedbg_setstep_ia32(void)
{
  e2dbgworld.curthread->step = TRUE;
}


void            kedbg_set_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  unsigned      i;
  
  E2DBG_SETREG(E2DBG_EAX_VAR, loc->reg32.eax);
  E2DBG_SETREG(E2DBG_EBX_VAR, loc->reg32.ebx);
  E2DBG_SETREG(E2DBG_ECX_VAR, loc->reg32.ecx);
  E2DBG_SETREG(E2DBG_EDX_VAR, loc->reg32.edx);
  E2DBG_SETREG(E2DBG_ESI_VAR, loc->reg32.esi);
  E2DBG_SETREG(E2DBG_EDI_VAR, loc->reg32.edi);
  E2DBG_SETREG(E2DBG_SP_VAR,  loc->reg32.esp);
  //E2DBG_SETREG(E2DBG_SSP_VAR, loc->reg32.eax);
  E2DBG_SETREG(E2DBG_FP_VAR,  loc->reg32.ebp);
  E2DBG_SETREG(E2DBG_PC_VAR,  loc->reg32.eip);

  /* We send the 9th first register to the server. */
  for (i = 0; i < 9; i++)
    {
      gdbwrap_writereg(i, *(&loc->reg32.eax + i), loc);
    }
}


void            kedbg_get_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  
  gdbwrap_readgenreg(loc);

  E2DBG_GETREG(E2DBG_EAX_VAR, loc->reg32.eax);
  E2DBG_GETREG(E2DBG_EBX_VAR, loc->reg32.ebx);
  E2DBG_GETREG(E2DBG_ECX_VAR, loc->reg32.ecx);
  E2DBG_GETREG(E2DBG_EDX_VAR, loc->reg32.edx);
  E2DBG_GETREG(E2DBG_ESI_VAR, loc->reg32.esi);
  E2DBG_GETREG(E2DBG_EDI_VAR, loc->reg32.edi);
  E2DBG_GETREG(E2DBG_SP_VAR,  loc->reg32.esp);
  //E2DBG_GETREG(E2DBG_SSP_VAR, loc->reg32.eax);
  E2DBG_GETREG(E2DBG_FP_VAR,  loc->reg32.ebp);
  E2DBG_GETREG(E2DBG_PC_VAR,  loc->reg32.eip);
}
