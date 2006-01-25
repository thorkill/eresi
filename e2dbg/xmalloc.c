/*
 * Allocator wrapper for the Embedded ELF Debugger
 *
 * Select the allocator depending on the thread ID of the caller
 *
 * Started Fri Oct  7 21:05:26 CEST 2005 crew
 */
#include "elfsh.h"


/* Debugger Thread specific information */
void		vm_dbgid_set(u_int pid) { e2dbgworld.dbgpid = pid; }
u_int		vm_dbgid_get()          { return (e2dbgworld.dbgpid); }	

/* Just a test to make use only the libc malloc */
u_int		pthread_self_my()
{
  return (-1);
}


/* Wrapper for malloc */
void		*malloc(size_t t)
{
  void		*(*mallocptr)();
  void		*chunk;

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED malloc\n", 22);
#endif
  
  if (!e2dbgworld.mallocsym)
    e2dbg_dlsym_init();
  if (vm_dbgid_get() != pthread_self() || !vm_dbgid_get())
    {
      mallocptr = (void *) e2dbgworld.mallocsym;
      if (!e2dbgworld.mallocsym)
	{
	  vm_output(" [!] Unable to use original malloc \n");
	  return (NULL);
	}
      chunk = mallocptr(t);
#if __DEBUG_EMALLOC__
      write(1, "LIBC malloc used\n", 17);
#endif
    }
  else
    {
      chunk = elfsh_malloc(t);
#if __DEBUG_EMALLOC__
      write(1, "E2DBG malloc used\n", 18);
#endif
    }

  if (!chunk)
    vm_output(" [!] Malloc failed \n");

  //printf("chunk : %08X\n", chunk);
  return (chunk);
}

/* Wrapper for calloc */
void		*calloc(size_t t, u_int nbr)
{
  void		*(*callocptr)();
  void		*chunk;

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED calloc \n", 22);
#endif

  if (!e2dbgworld.callocsym)
    e2dbg_dlsym_init();

  if (vm_dbgid_get() != pthread_self() || !vm_dbgid_get())
    {
      callocptr = (void *) e2dbgworld.callocsym;
      if (!e2dbgworld.callocsym)
	{
	  vm_output(" [!] Unable to use original calloc \n");
	  return (NULL);
	}
      chunk = callocptr(t, nbr);
#if __DEBUG_EMALLOC__
      write(1, "LIBC calloc used\n", 17);
#endif
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG calloc used\n", 18);
#endif
      chunk = elfsh_calloc(t, nbr);
    }
 
  if (!chunk)
    vm_output(" [!] Calloc failed \n");
 
  //printf("chunk : %08X\n", chunk);
  return (chunk);
}

/* Wrapper for realloc */
void	*realloc(void *a, size_t t)
{
  void	*b;
  void	*(*reallocptr)();

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED realloc\n", 23);
#endif

  if (!e2dbgworld.reallocsym)
    e2dbg_dlsym_init();
  if (vm_dbgid_get() != pthread_self() || !vm_dbgid_get())
    {
      reallocptr = (void *) e2dbgworld.reallocsym;
      if (!e2dbgworld.reallocsym)
	{
          vm_output(" [!] Unable to use original realloc \n");
          return (NULL);
        }
      b = reallocptr(a, t);
#if __DEBUG_EMALLOC__
      write(1, "LIBC realloc used\n", 18);
#endif
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG realloc used\n", 19);
#endif
      b = elfsh_realloc(a, t);
    }

  if (!b)
    vm_output(" [!] Realloc failed \n");

  return (b);
}

/* Wrapper for free */
void	free(void *a)
{
  void	*(*freeptr)();

#if __DEBUG_EMALLOC__
  write(1, "Calling HOOKED free\n", 20);
#endif

  if (!e2dbgworld.freesym)
    e2dbg_dlsym_init();
  if (vm_dbgid_get() != pthread_self() || !vm_dbgid_get())
    {
      freeptr = (void *) e2dbgworld.freesym;
      if (!e2dbgworld.freesym)
	{
	  vm_output(" [!] Unable to use original free \n");
          return;
        }
#if __DEBUG_EMALLOC__
      write(1, "LIBC free used\n", 15);
#endif
      freeptr(a);
    }
  else
    {
#if __DEBUG_EMALLOC__
      write(1, "E2DBG free used\n", 16);
#endif
      XFREE(a);
    }
}


void	wait4exit(void *a)
{
  //printf("\n [*] Debuggee wait4exit called \n");
  while (!e2dbgworld.exited)
    sleep(1);
  exit(0);
}


/* Wrapper for _exit */
void		_exit(int err)
{
  if (e2dbgworld.exited)
    {
      //      printf(" [*] Legit program terminating \n");
      raise(SIGKILL);
    }
  else
    {
      e2dbgworld.exited = 1;
      //      printf(" [*] Debugger exited \n\n");
      raise(SIGKILL);
    }
}
