#include "kedbg.h"
#include "interface.h"


/* /\**************** Handler for vectors ****************\/ */
void            kedbg_resetstep_ia32(void)
{
  e2dbgworld.curthread->step = FALSE;
}


eresi_Addr	*kedbg_getfp_ia32(void)
{
  fprintf(stderr, "Ganja rasta root :) \n");
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
  fprintf(stderr, "Ganja rasta root :) \n");
  fflush(stdout);
  return NULL;
}


void            *kedbg_getret_ia32(void *frame)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  return gdbwrap_readmemory(*(la32*)frame, DWORD_IN_BYTE, loc);
}


int             kedbg_break_ia32(elfshobj_t *f, elfshbp_t *bp)
{
  NOT_USED(f);
  NOT_USED(bp);
  fprintf(stderr, "Ganja rasta root :) \n");
  fflush(stdout);
  return 0;
}


void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr,
				void *buf, u_int size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  NOT_USED(file);

  fprintf(stderr, "RASTA YO YO\n\n");
  fflush(stderr);


  
  memcpy(buf, gdbwrap_readmemory(addr, (uint8_t)size, loc), size);

  fprintf(stderr, "Received in %s:%s\n\n", __PRETTY_FUNCTION__, (char *)buf);
  fflush(stderr);
  
  /* returning a char is ok ? */
  return buf;
}


int             kedbg_writemem(elfshobj_t *file, eresi_Addr addr, void *data,
			       u_int size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  fprintf(stderr, "RASTA YO YO - 2\n\n");
  fflush(stderr);
  
  NOT_USED(file);
  gdbwrap_writememory(addr, data, size, loc);

  return 0;
}


void            *kedbg_readmem(elfshsect_t *sect)
{
  fprintf(stderr, "Can in %s\n", __PRETTY_FUNCTION__);
  fflush(stderr);


  void *ptr = alloca(sect->shdr->sh_size);

/*   int *u = (int *)elfsh_get_raw(sect); */
/*   unsigned a; */
/*   if (u != NULL) */
/*     for (a = 0; a < sect->shdr->sh_size; a++) */
/*       fprintf(stderr, "Value in %s, name: %x\n", __PRETTY_FUNCTION__, u[a]); */
  
  kedbg_readmema(sect->parent, sect->shdr->sh_addr, ptr, sect->shdr->sh_size);
  
  fprintf(stderr, "Can in %s, name: %s\n", __PRETTY_FUNCTION__, sect->name);
  fflush(stderr);

  return ptr;
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
