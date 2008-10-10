#include <netdb.h>
#include "revm.h"

/**
 * Network & memory initialization for the gdb wrapper.
 */

/* int             cmd_network_gdbsupport(void) */
/* { */
/*   unsigned      i = 0; */
/*   revmjob_t     *job; */
/*   int           fd; */

/*   PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); */

/*   for (i = 0; revm_get_cur_job_parameter(i) != NULL; i++) */
/*     fprintf(stderr, "Value of current: %s - %d\n", revm_get_cur_job_parameter(i), */
/* 	    atoi(revm_get_cur_job_parameter(i))); */
/*   fprintf(stderr, "FINISHED\n"); */
/*   fflush(stderr); */
  
  
/*   if (revm_get_argc() != 2) */
/*     { */
/*       revm_output("Usage: netgdb <server> <port>\n"); */
/*       PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, */
/* 		   "Bad number of parameters.\n" */
/* 		   "Format: netgdb <server> <port>", -1); */
/*     } else */
/*     { */
/*       fd = gdbwrap_simpleconnect(revm_get_cur_job_parameter(0), */
/* 				 atoi(revm_get_cur_job_parameter(1))); */
/*       fprintf(stderr, "The value returned is: %d\n", fd); */
/*       if (fd == -1) */
/* 	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Error found. Exit", -1); */
/*     } */

/*   revm_create_new_workspace("netgdb_ws"); */
/* } */


int             gdbwrap_simpleconnect(char *host, int port)
{
  int           rval;
  int           sd;
  revmjob_t     *job;
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

  XALLOC(__FILE__, __FUNCTION__, __LINE__,job, sizeof (revmjob_t), -1);

  job->ws.io.type     = REVM_IO_GDB;
  job->ws.io.input    = &revm_netgdb_input;
  job->ws.io.output   = &revm_netgdb_output;
  job->ws.createtime  = time(&job->ws.createtime);
  job->ws.io.input_fd = sd;

  hash_add(&world.jobs, "netgdb", job);
  world.curjob = job;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sd);
}
