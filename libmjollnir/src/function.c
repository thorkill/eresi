/*
 * (C) 2006-2007 Asgard Labs, thorolf
 *     2006-2007 devhell, mayhem
 *     2007      rfd labs, strauss
 *
 * BSD License
 * $Id: function.c,v 1.24 2007-02-26 17:47:15 thor Exp $
 *
 */
#include <libmjollnir.h>

/**
 * Function dumping procedure for debug purposes
 */

void mjr_function_dump(char *where,mjrfunc_t *f)
{
  mjrlink_t *cur;
  fprintf(D_DESC," [D] FUNC DUMP in {%s}: %x/<%s>[%s] Cnbr: %d Pnbr %d\n",
	  where, f->vaddr, (f->name) ? f->name : NULL ,(f->md5) ? f->md5 : NULL ,f->childnbr,f->parentnbr);

  if (f->childfuncs)
    {
      fprintf(D_DESC," [D] Child functions:\n [x] ");

      cur = f->childfuncs;

      while(cur){
	fprintf(D_DESC,"%x ", cur->vaddr);
	cur = cur->next;
      }
 
      fprintf(D_DESC,"\n");
    }

  if (f->parentfuncs)
    {
      fprintf(D_DESC," [D] Parent functions:\n [x] ");
      cur = f->parentfuncs;

      while(cur)
	{
	  fprintf(D_DESC,"%x ", cur->vaddr);
	  cur = cur->next;
	}
    
      fprintf(D_DESC,"\n");
    }
}

/**
 * Save and prepare a buffer which will be saved into the elfshobj.
 * @param fnc a function to store
 * @param type 0 for parents/1 for childs
 * @param buf buffer which contains the data
 * @return an offset where the data has been saved
 */

u_int	 mjr_function_flow_save(mjrfunc_t *fnc,u_int type, mjrbuf_t *buf)
{
  u_int curOff,nbr; 
  mjrlink_t *cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (type == 0) 
    {
      cur = fnc->parentfuncs;
      nbr = fnc->parentnbr;
    } 
  else if (type == 1)
    {
      cur = fnc->childfuncs;
      nbr = fnc->childnbr;
    }

  if (!cur)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->allocated, -1);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + (sizeof(elfsh_Addr)*nbr)))
    {
      buf->allocated += getpagesize();
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	       buf->data, buf->data, buf->allocated, -1);
    }

  curOff = buf->maxlen;

  while (cur)
    {
      memcpy(buf->data + buf->maxlen, (char *)&cur->vaddr , sizeof(elfsh_Addr));
      buf->maxlen += sizeof(elfsh_Addr);
      cur = cur->next;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (curOff));
}

/* Copy the function in a special buffer to fingerprint it */
int		mjr_function_copy(mjrcontext_t  *ctx, 
				  unsigned char *src, 
				  unsigned char *dst, 
				  int		mlen)
{
  int             n, ilen, p;
  asm_instr       instr, hist[2];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (p = ilen = n = 0; n < mlen; n += ilen) 
    {
      ilen = asm_read_instr(&instr, src + n, mlen - n, &ctx->proc);
      if (ilen <= 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, p);
      
      /* Filter this out */
      if ((ctx->proc.type == ASM_PROC_IA32  && instr.instr != ASM_NOP) ||
	  (ctx->proc.type == ASM_PROC_SPARC && instr.instr != ASM_SP_NOP))
	{
	  memcpy(dst + p, src, ilen);
	  p += ilen;
	}
      
      /* epilog */
      if (ctx->proc.type == ASM_PROC_IA32)
	{
	  if ((instr.instr == ASM_RET) && (hist[0].instr == ASM_LEAVE || 
					   hist[0].instr == ASM_POP ||
					   hist[0].instr == ASM_MOV))
	    {
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, p);
	    }
	} 
      else if (ctx->proc.type == ASM_PROC_SPARC)
	{
	  if ((instr.instr == ASM_SP_RESTORE && hist[0].instr == ASM_SP_RET) ||
	      hist[0].instr == ASM_SP_RETL)
	    {	  	
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, p);
	    }
	}
      hist[1] = hist[0];
      hist[0] = instr;
    }  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unable to copy function", 0);
}



/* Finger print a function using X method */
void		*mjr_fingerprint_function(mjrcontext_t  *ctx, 
					  elfsh_Addr	addr, 
					  int		type) 
{
 MD5_CTX	md5ctx;
 unsigned char  fbuf[MJR_MAX_FUNCTION_LEN] = {0x00};
 unsigned char	digest[16];
 char		*pt;
 void           *ret;
 u_int		i;
 int 		mlen;
 elfsh_SAddr	off;
 elfshsect_t	*sect;
 unsigned char	*buff;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 ret = NULL;

 /* Get function data */
 sect = elfsh_get_parent_section(ctx->obj, addr, &off);
 if (!sect)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unknown parent section", NULL);   
 buff = elfsh_get_raw(sect);
 buff += off;

 /* Select the desired fingerprinting function */
 switch (type) 
   {
   case MJR_FPRINT_TYPE_MD5:
     //memset(fbuf, 0, MJR_MAX_FUNCTION_LEN);
     mlen = mjr_function_copy(ctx, buff, fbuf, MJR_MAX_FUNCTION_LEN);
       
     if (mlen <= 0) 
       PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
     MD5_Init(&md5ctx);
     MD5_Update(&md5ctx, fbuf, mlen);
     MD5_Final(digest, &md5ctx);
     XALLOC(__FILE__, __FUNCTION__, __LINE__, ret, 33, NULL);
     if (!ret)
       PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
     memset(ret, 0, 33);
     pt = ret;
     for (i = 0; i < 16; i++, pt += 2) 
       sprintf(pt, "%02x", digest[i]);
     break;

   default:
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unknown fingerprint type", NULL);
   }
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/* Create a function in the original MJR format */
mjrfunc_t	*mjr_function_create(mjrcontext_t *c, 
					char *name, 
					elfsh_Addr addr, 
				        u_int size, 
					mjrblock_t *e)
{
 mjrfunc_t	*fun;
 
 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 
 XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	fun, sizeof(mjrfunc_t), NULL);
 fun->vaddr = addr;
 fun->size  = size;
 fun->first = e;
 snprintf(fun->name,sizeof(fun->name),"%s", name);
 fun->parentfuncs = NULL;
 fun->parentnbr = 0;
 fun->childfuncs = NULL;
 fun->childnbr = 0;

 memset(fun->md5, 0x00, sizeof(fun->md5));
 hash_add(&c->funchash, name, fun);

 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (fun)); 
}

/**
 ** Expand the list of child functions
 **/

void		mjr_function_add_child(mjrfunc_t *fnc,
					elfsh_Addr vaddr,
					int type)
{
  mjrlink_t *n;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, n, sizeof(mjrlink_t), );
  n->vaddr = vaddr;
  n->type = type;
  n->next = fnc->childfuncs;
  fnc->childfuncs = n;
  fnc->childnbr = fnc->childnbr + 1;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 ** Expand the list of parent functions
 */
/* XXX: Duplicate ! please factorize with the previous function ! */
void		mjr_function_add_parent(mjrfunc_t *fnc,
					elfsh_Addr vaddr,
					int type)
{
  mjrlink_t *n;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__, n, sizeof(mjrlink_t), );
  n->vaddr = vaddr;
  n->type = type;
  n->next = fnc->parentfuncs;
  fnc->parentfuncs = n;
  fnc->parentnbr++;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

void		*mjr_functions_load(mjrcontext_t *ctxt)
{
  int		index,findex;
  elfshsect_t	*sect,*flowsect;
  u_int		fncnbr,off;
  elfsh_Addr	tmpaddr;
  mjrfunc_t	*curfnc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sect = elfsh_get_section_by_name(ctxt->obj, 
				   ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 
				   0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No function section : use analyse command", NULL);

  /* get flow section */
  /* FIXME: use macros!!! */
  flowsect = elfsh_get_section_by_name(ctxt->obj,
				       ELFSH_SECTION_NAME_EDFMT_FCONTROL
				       , 0, 0, 0);
  
  if (!flowsect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No function flow section : use analyse command", NULL);

  fncnbr = sect->shdr->sh_size / sizeof(mjrfunc_t);
  fprintf(D_DESC, " [D] %s: loading %d functions\n", __FUNCTION__, fncnbr);

  for (index=0; index < fncnbr; index++)
    {
      curfnc = (mjrfunc_t *) sect->data + index;
      off = (u_int)curfnc->parentfuncs;
      curfnc->parentfuncs = NULL;

      /* fill the flow list */
      for (findex=0; findex<curfnc->parentnbr; findex++)
	{	 
	  tmpaddr = *((elfsh_Addr *)flowsect->data + off + findex);
	  if (tmpaddr)
	    {
	      fprintf(D_DESC," [D] resotore parent: %x\n", tmpaddr);
	      mjr_function_add_parent(curfnc,tmpaddr,0);
	    }
	}

      off = (u_int)curfnc->childfuncs;
      curfnc->childfuncs = NULL;

      for (findex=0; findex<curfnc->childnbr; findex++)
	{
	  tmpaddr = *((elfsh_Addr *)flowsect->data + off + findex);
	  if (tmpaddr)
	    {
	    fprintf(D_DESC," [D] resotore child: %x\n", tmpaddr);
	    mjr_function_add_child(curfnc,tmpaddr,0);
	    }
	}

      mjr_function_dump((char *)__FUNCTION__, curfnc);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}



/* Retreive control flow section content if any */
mjrblock_t*	mjr_functions_get(mjrcontext_t *ctxt)
{
  elfshsect_t	*sect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
 /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, 
				   ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No functions section : use analyse command", NULL);

  /* Return or retreive information */
  if (sect->altdata)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);

  sect->altdata = mjr_functions_load(ctxt);

  if (sect->altdata)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Flow analysis failed", 0);
}

/* Create the block information to be saved in file */
int			mjr_function_save(mjrfunc_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrfunc_t		*curfunc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!cur)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  /* At this points, no new block allocation should be done */
  snprintf(buffer, sizeof (buffer), "%s%lX",
	   (char *) config_get_data(MJR_COFING_CALL_PREFIX),
	   (unsigned long) cur->vaddr);
  sym = elfsh_get_symbol_by_name(buf->obj, buffer);
  if (sym)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC," [*] Saving block at addr %s \n", buffer);
#endif

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->allocated, -1);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + sizeof(mjrblock_t)))
    {
      buf->allocated += getpagesize();
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	       buf->data, buf->data, buf->allocated, -1);
    }
  curfunc         = (mjrfunc_t *) ((char *) buf->data + buf->maxlen);
  memcpy(curfunc, cur, sizeof(mjrfunc_t));

  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(cur->vaddr, cur->size, STT_FUNC, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(mjrfunc_t);
  //  buf->block_counter++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* XXX: You should never use printf but vm_output */
int			mjr_functions_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf,cfbuf;
  mjrfunc_t		*func;
  int			err;
  char			**keys;
  int			keynbr;
  int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Remove previous control section if any */
  sect = elfsh_get_section_by_name(ctxt->obj, 
				   ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 0, 0, 0);
  if (sect)
    elfsh_remove_section(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_FUNCTIONS);

  /* Initialize data buffer */
  buf.allocated     = 0;
  buf.maxlen        = 0;
  buf.block_counter = 0;
  buf.data          = 0;
  buf.obj           = ctxt->obj;

    /* Initialize data buffer for control flow */
  cfbuf.allocated     = 0;
  cfbuf.maxlen        = 0;
  cfbuf.block_counter = 0;
  cfbuf.data          = 0;
  cfbuf.obj           = ctxt->obj;

  /* Iteratively save all blocks */
  keys = hash_get_keys(&ctxt->funchash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      u_int flowOffp,flowOffc;

      func = hash_get(&ctxt->funchash, keys[index]);
      mjr_function_dump((char *)__FUNCTION__, func);

      /* 1st Save flow information and get offset from the beginning 
	 of .edfmt.fcontrol */
      flowOffp = mjr_function_flow_save(func, 0, &cfbuf);
      fprintf(D_DESC, " [D] %s: parentOffset: %d\n", __FUNCTION__, flowOffp);

      flowOffc = mjr_function_flow_save(func, 1, &cfbuf);
      fprintf(D_DESC, " [D] %s: childOffset: %d\n", __FUNCTION__, flowOffc);
      
      /* Set New pointers */
      func->parentfuncs = (mjrlink_t *)flowOffp;
      func->childfuncs = (mjrlink_t *)flowOffc;

      /* 2nd Save function structure */
      mjr_function_save(func, &buf);

    }

  /* Create control section */
  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_FUNCTIONS);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);
  sect->altdata = ctxt->blklist = (void *) buf.data;

  printf(" [*] Saving %s section of %u bytes \n", 
	 ELFSH_SECTION_NAME_EDFMT_FUNCTIONS,
	 buf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save .edfmt.functions section", -1);

  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_FCONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, cfbuf.maxlen, 0, 0, 0, 0);
  sect->altdata = cfbuf.data;

  printf(" [*] Saving %s section of %u bytes\n",
	 ".edfmt.fcontrol",
	 cfbuf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, cfbuf.data);

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save .edfmt.fcontrol section", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}

