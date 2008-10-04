/*
** @file misc.c
** @ingroup kernsh
*/
#include "kernsh.h"
#include "libkernsh.h"

int		kernsh_hexdump(unsigned char * data, 
			       unsigned int amount, 
			       size_t addr)
{
  unsigned int	dp;
  unsigned int	p;
  const char	trans[] =
    "................................ !\"#$%&'()*+,-./0123456789"
    ":;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm"
    "nopqrstuvwxyz{|}~...................................."
    "....................................................."
    "........................................";

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


unsigned int		kernsh_instr_display(unsigned int index, eresi_Addr vaddr, 
				     unsigned int foffset, unsigned int size, char *name, 
				     unsigned int nindex, char *buff)
			      
{
  char		*s;
  char		buf[256];
  unsigned int		idx_bytes;
  unsigned int		ret;
  asm_instr	ptr;
  char		base[16] = "0123456789ABCDEF";
  char		logbuf[BUFSIZ];
  char		c1[2];
  char		c2[2];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!buff)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));    

  /* Print the instr. itself : vaddr and relative symbol resolution */
  ret = asm_read_instr(&ptr, (unsigned char *)buff + index, size - index, 
		       &libkernshworld.proc);
  if (ret == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "asm_read_instruction faild (-1)", (ret));

  s = (!ret ? "(bad)" : asm_display_instr_att(&ptr, (vaddr)));

  snprintf(buf, sizeof(buf), "%s", 
	   revm_coloraddress(XFMT, vaddr + index));
  size = snprintf(logbuf, BUFSIZ, "%-40s %-30s ", 
		  buf, revm_colorinstr(s));
  
  /* Print bytes in hexa for this instruction */
  ret = asm_instr_len(&ptr);
  if (!ret)
    ret++;
  
  for (idx_bytes = 0; idx_bytes < ret; idx_bytes++)
    {
      c1[0] = base[(buff[index + idx_bytes] >> 4) & 0x0F];
      c2[0] = base[buff[index + idx_bytes] & 0x0F];
      c1[1] = c2[1] = 0x00;
      size += snprintf(logbuf + size, sizeof(logbuf) - size, "%s%s ", 
		       revm_colorfieldstr(c1), 
		       revm_colorfieldstr(c2));
    }
  
  revm_output(logbuf);
  revm_output("\n");
  revm_endline();
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

int		kernsh_disasm(char *buffer, int len, unsigned long addr)
{

  int		index, value;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  index = 0;
  while(index < len && len > 0)
    {
      value = kernsh_instr_display(index, addr, index, len, NULL, 0, buffer);
      if (value <= 0)
	  break;

      index += value;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		kernsh_addrlen(revmlist_t *actual, eresi_Addr *addr, int *len)
{
  revmexpr_t	*expr;
  revmobj_t     *obj;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!actual)
    {
      *addr = 0;
      *len = 0;

      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid revmlist",
		     -1);
    }

 /* Is it directly an addr ? */
  if (IS_VADDR(actual->rname))
    {
      if (sscanf(actual->rname + 2, AFMT, addr) != 1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid virtual address requested",
		     -1);
    }
  /* Get the value of the object */
  else
    {
      expr = revm_lookup_param(actual->rname);
      
      obj = expr->value;
      
      switch (obj->otype->type)
	{
	case ASPECT_TYPE_LONG:
	case ASPECT_TYPE_CADDR:
	case ASPECT_TYPE_DADDR:
	  *addr = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
	      break;
	      
	case ASPECT_TYPE_INT:
	  *addr = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
	  break;
	}
    }
  
  *len = actual->size;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
