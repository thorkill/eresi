#include <netdb.h>
#include "revm.h"
#include "gdbwrapper.h"


extern gdbwrapworld_t gdbwrapworld;
Bool   stepping = FALSE;

/* /\**************** Handler for vectors ****************\/ */
void            kedbg_resetstep_ia32(void)
{
  stepping = FALSE;
}


eresi_Addr	*kedbg_getfp_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  return (eresi_Addr *)loc->reg32.ebp;
}


/* void            *kedbg_bt_ia32(void *frame) */
/* { */
/* } */


void            *kedbg_getret_ia32(void *frame)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  return &loc->reg32.ebp;
}


/* int             kedbg_break_ia32(elfshobj_t *f, elfshbp_t *bp) */
/* { */
/* } */


/* void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr, */
/* 				void *buf, u_int size) */
/* { */
/* } */


/* int             kegbg_writemem(elfshobj_t *, eresi_Addr, void *, u_int) */
/* { */
/* } */


/* void            *kedbg_readmem(elfshsect_t *base) */
/* { */

/* } */


/* Get %IP */
eresi_Addr      *kedbg_getpc_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  return (eresi_Addr *)loc->reg32.eip;
}

  
void            kedbg_setstep_ia32(void)
{
  stepping = TRUE;
}


/* void            kedbg_set_regvars_ia32(void) */
/* { */
/* } */


void            kedbg_get_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_readgenreg(loc);
}

/**************** Command definition. ****************/


/* Dummy function, for compatibility with e2dbg. */
int             cmd_linkmap(void)
{
  return 0;
}


int             e2dbg_linkmap_load(char *name)
{
  name = NULL;
  return 0;
}


/* Debugging purpose. */
void            cmd_com1(void)
{
  kedbg_getpc_ia32();
  //  fprintf(stderr, "YOUPLA: %#x\n", *kedbg_getpc_ia32());
}


/* Continue command. */
void            cmd_kedbgcont(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  if (!stepping)
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
