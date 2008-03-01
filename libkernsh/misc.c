/*
** misc.c for libkernsh
**
** $Id: misc.c,v 1.4 2007-09-02 21:47:25 pouik Exp $
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
	  memcpy(name, "UNKNOWN_NAME", size - 1);
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

int kernsh_find_end(unsigned long addr)
{
  unsigned char buff[BUFSIZ];
  int curr, sizemax;
  unsigned long addrcur, addrfinal, start;
  asm_instr instr;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  addrcur = addr;
  sizemax = (int) config_get_data(LIBKERNSH_VMCONFIG_FENDSIZE);
  addrfinal = addr+sizemax;

  while (addrcur < addrfinal)
    {
      if (kernsh_is_mem_mode())
	{
	  kernsh_readmem(addrcur, buff, sizeof(buff));
	}
      else
	{
	  start = elfsh_get_foffset_from_vaddr(libkernshworld.root, 
					       addrcur);
	  elfsh_raw_read(libkernshworld.root, start, buff, sizeof(buff));

	}
      curr = 0;
      while(curr < sizeof(buff))
	{
	  if (asm_read_instr(&instr, 
			     buff + curr, sizeof(buff) - curr, 
			     &libkernshworld.proc) > 0)
	    {
#if __DEBUG_KERNSH__
	      printf("CURR %d INSTR %d TYPE %d => ", 
		     curr, instr.instr, instr.type);
	      int i;
	      for (i = 0; i < instr.len; i++)
		printf("%02x ", *(buff + curr + i));
	      printf("\n");
#endif
	      curr += asm_instr_len(&instr);
	      
	      if (instr.instr == ASM_RET || instr.instr == ASM_IRET)
		{
		  PROFILER_ROUT(__FILE__, 
				__FUNCTION__, 
				__LINE__, 
				curr + (addrcur - addr));
		}
	    }
	  else
	    {
	      curr++;
	    }
	}
      
      addrcur += sizeof(buff);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unable to find the end", 0);
}



void kernsh_hexdump(unsigned char * data, 
		    unsigned int amount, 
		    size_t addr)
{
  unsigned int dp;
  unsigned int p;
  const char trans[] =
    "................................ !\"#$%&'()*+,-./0123456789"
    ":;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm"
    "nopqrstuvwxyz{|}~...................................."
    "....................................................."
    "........................................";

  for ( dp = 1; dp <= amount; dp++ )
    {
      if ( (dp % 16) == 1 )
	{
	  fprintf( stdout, "%#08x | ", addr+dp-1 );
	}

      fprintf( stdout, "%02x ", data[dp-1] );
      if ( (dp % 8) == 0 && (dp % 16) != 0 )
	{
	  fputs( " ", stdout );
        }
      if ( (dp % 16) == 0 )
	{
	  fputs( "| ", stdout );
	  p = dp;
	  for ( dp -= 16; dp < p; dp++ )
	    {
	      fprintf( stdout, "%c", trans[data[dp]] );
            }
	  fputs( "\n", stdout );
        }
    }
  
  if ( (amount % 16) != 0 )
    {
      p = dp = 16 - ( amount % 16 );
      for ( dp = p; dp > 0; dp-- )
	{
	  fputs( "   ", stdout );
	  if ( ((dp % 8) == 0) && (p != 8) ) 
	    {
	      fputs( " ", stdout );
	    }
	}
      fputs( "| ", stdout );
      for ( dp = (amount - (16 - p)); dp < amount; dp++ ) {
	fprintf( stdout, "%c", trans[data[dp]] );
      }
    }
  
  fputs( "\n", stdout );
  return;
}
