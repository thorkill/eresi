/*
** misc.c for libkernsh
**
** $Id: misc.c,v 1.2 2007-07-28 15:02:23 pouik Exp $
**
*/
#include "libkernsh.h"

/* This function find a needle inside a haystack */
void *kernsh_find_pattern(const void *haystack, int haystack_len,
			  const void *needle, int needle_len)
{
  const char *begin;
  const char *const last_possible
    = (const char *) haystack+ haystack_len - needle_len;

  if (needle_len == 0)
    return (void *) haystack;

  if (haystack_len < needle_len)
    return NULL;

  for (begin = (const char *) haystack; begin <= last_possible; ++begin)
    if (begin[0] == ((const char *) needle)[0] &&
	!memcmp ((const void *) &begin[1],
		 (const void *) ((const char *) needle + 1),
		 needle_len - 1))
      return (void *) begin;

  return NULL;
}

/* Resolve name with addr */
int kernsh_resolve_systemmap(unsigned long addr, char *name, size_t size)
{
  FILE *input;
  char line[256];
  char caddr[32];
  char *paddr, *pname, *end;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
		
  if(addr != 0)
    {
      memset(name, '\0', size);
      memset(caddr, '\0', sizeof(caddr));

      snprintf(caddr, sizeof(caddr) - 1, "0x%lx", (unsigned long)addr);
      if ((input = fopen((char *) config_get_data(LIBKERNSH_VMCONFIG_SYSTEMMAP), 
			 "r")) == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to open systemmap", -1);
	}
      
      while (fgets(line, sizeof(line) - 1, input) != NULL)
	{
	  paddr = strtok(line, " ");
	  if(strstr(caddr, paddr))
	    {
	      pname = strtok(NULL, " ");
	      pname = strtok(NULL, " ");
	      end = strstr(pname, "\n");
	      if (pname[0] != '_')
		{
		  memcpy(name, pname, size - 1);
		  name[end - pname] = '\0';
		  fclose(input);
		  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
		}
	    }
	}
      fclose(input);
    }

  memcpy(name, "UNKNOWN_NAME", size - 1);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot get name", -1);
}

/* Resolve add with name */
int kernsh_rresolve_systemmap(const char *name, unsigned long *addr, size_t size)
{
  FILE *input;
  char *paddr, *pname;
  char line[256];
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if ((input = fopen((char *) config_get_data(LIBKERNSH_VMCONFIG_SYSTEMMAP), 
		     "r")) == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unable to open systemmap", -1);
    }
	
  while (fgets(line, sizeof(line) - 1, input) != NULL)
    {
      paddr = strtok(line, " ");
      pname = strtok(NULL, " ");
      pname = strtok(NULL, " ");
      pname[strlen(pname) - 1] = '\0';
      
      if (!strcmp(pname, name))
	{
	  *addr = strtoull(paddr, NULL, 16);
	  fclose(input);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
    }
  
  fclose(input);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Cannot get addr", -1);
}
