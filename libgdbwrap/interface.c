#include <netdb.h>
#include "revm.h"
#include "gdbwrapper.h"
#include "libe2dbg.h"

extern gdbwrapworld_t gdbwrapworld;


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
}


void            *kedbg_bt_ia32(void *frame)
{
  
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
 
}


void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr,
				void *buf, u_int size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  NOT_USED(file);
  strncpy((char *)buf, gdbwrap_readmemory(addr, (uint8_t)size, loc), size);

  /* returning a char is ok ? */
  return buf;
}


int             kegbg_writemem(elfshobj_t *file, eresi_Addr addr, void *data,
			       u_int size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  
  NOT_USED(file);
  gdbwrap_writememory(addr, data, size, loc);

  return 0;
}


void            *kedbg_readmem(elfshsect_t *base)
{
  NOT_USED(base);

  /* Muh ? */
  return base;
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

/**************** Command definition. ****************/


/* Dummy function, for compatibility with e2dbg. */
int             cmd_linkmap(void)
{
  return 0;
}


int             e2dbg_linkmap_load(char *name)
{
  NOT_USED(name);
  return 0;
}


/* Debugging purpose. */
void            cmd_com1(void)
{
  fprintf(stderr, "Value: %d\n", world.curjob->curfile->hostype);
  kedbg_getpc_ia32();
  //  fprintf(stderr, "YOUPLA: %#x\n", *kedbg_getpc_ia32());
}


/* Continue command. */
void            cmd_kedbgcont(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  if (!e2dbgworld.curthread->step)
    gdbwrap_continue(loc);
  else
    gdbwrap_stepi(loc);
}


int             gdbwrap_simpleconnect(char *host, int port)
{
  int           rval;
  int           sd;
  struct        sockaddr_in   socketaddr;
  struct        hostent       *hostaddr;
  struct        protoent      *protocol;
  extern        int           h_errno;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  hostaddr = gethostbyname(host);
  protocol = getprotobyname( "tcp" );

  if (!hostaddr || h_errno == HOST_NOT_FOUND)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "invalid gethostbyname", -1);

  if (!port)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "invalid port", -1);
  
   if (!protocol)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "invalid getprotobyname()",
		 -1);
  
  sd = socket(PF_INET, SOCK_STREAM, protocol->p_proto);
  
  if (sd == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "invalid socket", -1);
  
  memset(&socketaddr, 0, sizeof(socketaddr));
  socketaddr.sin_family = AF_INET;
  socketaddr.sin_port = htons(port);

  memcpy(&socketaddr.sin_addr, hostaddr->h_addr, hostaddr->h_length);
  rval = connect(sd, (struct sockaddr *)&socketaddr, sizeof(socketaddr));

  if (rval == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Problem when connecting",
		 -1);

/*   XALLOC(__FILE__, __FUNCTION__, __LINE__,job, sizeof (revmjob_t), -1); */

 /*  job->ws.io.type     = REVM_IO_GDB; */
/*   job->ws.io.input    = &revm_netgdb_input; */
/*   job->ws.io.output   = &revm_netgdb_output; */
/*   job->ws.createtime  = time(&job->ws.createtime); */
/*   job->ws.io.input_fd = sd; */

/*   hash_add(&world.jobs, "netgdb", job); */
/*   world.curjob = job; */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sd);
}
