/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: function.c,v 1.13 2007-01-25 01:51:34 thor Exp $
 *
 */
#include <libmjollnir.h>

void mjr_function_dump(char *where,mjrfunc_t *f)
{
  mjrcaller_t *cur,*next;
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

u_int	 mjr_function_flow_parents_save(mjrfunc_t *fnc, mjrbuf_t *buf)
{
  u_int curOff; 
  mjrcaller_t *cur;

  ELFSH_PROFILE_IN(__FILE__,__FUNCTION__,__LINE__);

  if (!fnc->parentfuncs)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      buf->data = elfsh_malloc(buf->allocated);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + (sizeof(elfsh_Addr)*fnc->parentnbr)))
    {
     buf->allocated += getpagesize();
     buf->data = elfsh_realloc(buf->data, buf->allocated);
    }
 curOff = buf->maxlen;

 cur = fnc->parentfuncs;

 while(cur)
   {
     memcpy(buf->data + buf->maxlen, cur->vaddr , sizeof(elfsh_Addr));
     buf->maxlen += sizeof(elfsh_Addr);
     cur = cur->next;
   }

 ELFSH_PROFILE_ROUT(__FILE__,__FUNCTION__,__LINE__, (curOff));
}



u_int	mjr_function_flow_childs_save(mjrfunc_t *fnc, mjrbuf_t *buf)
{
  u_int curOff; 
  mjrcaller_t *cur;

  ELFSH_PROFILE_IN(__FILE__,__FUNCTION__,__LINE__);

  if (!fnc->childfuncs)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      buf->data = elfsh_malloc(buf->allocated);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + (sizeof(elfsh_Addr)*fnc->childnbr)))
    {
     buf->allocated += getpagesize();
     buf->data = elfsh_realloc(buf->data, buf->allocated);
    }
 curOff = buf->maxlen;

 cur = fnc->childfuncs;

 while(cur)
   {
     memcpy(buf->data + buf->maxlen, cur->vaddr , sizeof(elfsh_Addr));
     buf->maxlen += sizeof(elfsh_Addr);
     cur = cur->next;
   }

 ELFSH_PROFILE_ROUT(__FILE__,__FUNCTION__,__LINE__, (curOff));
}

/* Copy the function in a special buffer to fingerprint it */
int		mjr_i386_function_copy(mjrcontext_t  *ctx, 
				       unsigned char *src, 
				       unsigned char *dst, 
				       int	     mlen)
{
  int             n, ilen, p;
  asm_instr       instr, hist[4];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  for (p = ilen = n = 0; n < mlen; n += ilen) 
    {
      ilen = asm_read_instr(&instr, src + n, mlen - n, &ctx->proc);
      if (ilen <= 0)
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, p);
      
      /* Filter this out */
      if (instr.instr != ASM_NOP)
	dst[p++] = src[n];
      
      /* epilog */
      if ((hist[0].instr == ASM_LEAVE || hist[0].instr == ASM_POP) && 
	  (instr.instr == ASM_RET))
	{
	  dst[p++] = src[n];
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, p);
	} 
      hist[1] = hist[0];
      hist[0] = instr;
    }  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 ret = NULL;

 /* Get function data */
 sect = elfsh_get_parent_section(ctx->obj, addr, &off);
 if (!sect)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unknown parent section", NULL);   
 buff = elfsh_get_raw(sect);
 buff += off;

 /* Select the desired fingerprinting function */
 switch (type) 
   {
   case MJR_FPRINT_TYPE_MD5:
     //memset(fbuf, 0, MJR_MAX_FUNCTION_LEN);
     mlen = mjr_i386_function_copy(ctx, buff, fbuf, MJR_MAX_FUNCTION_LEN);
     if (mlen <= 0) 
       ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
     MD5_Init(&md5ctx);
     MD5_Update(&md5ctx, fbuf, mlen);
     MD5_Final(digest, &md5ctx);
     ret = elfsh_malloc(33);
     if (!ret)
       ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
     memset(ret, 0, 33);
     pt = ret;
     for (i = 0; i < 16; i++, pt += 2) 
       sprintf(pt, "%02x", digest[i]);
     break;

   default:
     ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unknown fingerprint type", NULL);
   }
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/* Create a function in the original MJR format */
mjrfunc_t	*mjr_function_create(mjrcontext_t *c, 
					char *name, 
					elfsh_Addr addr, 
				        u_int size, 
					mjrblock_t *e)
{
 mjrfunc_t	*fun;
 
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__); 
 fun = elfsh_malloc(sizeof(mjrfunc_t));
 fun->vaddr = addr;
 fun->size  = size;
 fun->first = e;
 fun->name  = name;
 fun->parentfuncs = NULL;
 fun->parentnbr = 0;
 fun->childfuncs = NULL;
 fun->childnbr = 0;
 hash_add(&c->funchash, name, fun);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (fun)); 
}

void		mjr_function_add_child(mjrfunc_t *fnc,
					elfsh_Addr vaddr,
					int type)
{
  mjrcaller_t *n;
  
  ELFSH_PROFILE_IN(__FILE__,__FUNCTION__,__LINE__);

  n = elfsh_malloc(sizeof(mjrcaller_t));
  n->vaddr = vaddr;
  n->type = type;
  n->next = fnc->childfuncs;
  fnc->childfuncs = n;
  fnc->childnbr = fnc->childnbr + 1;

  mjr_function_dump(__FUNCTION__, fnc);

  ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);
}

void		mjr_function_add_parent(mjrfunc_t *fnc,
					elfsh_Addr vaddr,
					int type)
{
  mjrcaller_t *n;
  
  ELFSH_PROFILE_IN(__FILE__,__FUNCTION__,__LINE__);

  n = elfsh_malloc(sizeof(mjrcaller_t));
  n->vaddr = vaddr;
  n->type = type;
  n->next = fnc->parentfuncs;
  fnc->parentfuncs = n;
  fnc->parentnbr++;

  mjr_function_dump(__FUNCTION__, fnc);

  ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);
}

void *mjr_functions_load(mjrcontext_t *ctxt)
{
  int		index,findex;
  elfshsect_t	*sect,*flowsect;
  u_int		fncnbr,cnt,off;
  elfsh_Addr	tmpaddr;
  mjrfunc_t	*curfnc;

  ELFSH_PROFILE_IN(__FILE__,__FUNCTION__,__LINE__);

  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No function section : use analyse command", NULL);

  /* get flow section */
  /* FIXME: use macros!!! */
  flowsect = elfsh_get_section_by_name(ctxt->obj, ".edfmt.fcontrol", 0, 0, 0);
  
  if (!flowsect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No function flow section : use analyse command", NULL);

  fncnbr = sect->shdr->sh_size / sizeof(mjrfunc_t);
  fprintf(D_DESC, " [D] %s: loading %d functions\n", __FUNCTION__, fncnbr);

  for (index=0; index < fncnbr; index++)
    {
      curfnc = (mjrfunc_t *) sect->data + index;
      /* FIXME: change store function to save name and md5 sums */
      curfnc->name=NULL;
      curfnc->md5=NULL;
      /* fill the flow list */
      for (findex=0; findex<curfnc->parentnbr; findex++)
	{
	  off = curfnc->parentfuncs;
	  curfnc->parentfuncs = NULL;

	  tmpaddr = (elfsh_Addr) flowsect->data + off + findex;
	  fprintf(D_DESC," [D] resotore parent: %x\n", tmpaddr);
	  mjr_function_add_parent(curfnc,tmpaddr,0);
	}

      for (findex=0; findex<curfnc->childnbr; findex++)
	{
	  off = curfnc->childfuncs;
	  curfnc->childfuncs = NULL;

	  tmpaddr = (elfsh_Addr) flowsect->data + off + findex;
	  fprintf(D_DESC," [D] resotore child: %x\n", tmpaddr);
	  mjr_function_add_child(curfnc,tmpaddr,0);
	}

      mjr_function_dump(__FUNCTION__, curfnc);
    }

  ELFSH_PROFILE_ROUT(__FILE__,__FUNCTION__,__LINE__, NULL);
}

/* Retreive control flow section content if any */
mjrblock_t*	mjr_functions_get(mjrcontext_t *ctxt)
{
  elfshsect_t	*sect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
 /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 0, 0, 0);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No functions section : use analyse command", NULL);

  /* Return or retreive information */
  if (sect->altdata)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);

  sect->altdata = mjr_functions_load(ctxt);

  if (sect->altdata)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sect->altdata);

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Flow analysis failed", 0);
}

/* Create the block information to be saved in file */
int			mjr_function_save(mjrfunc_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrfunc_t		*curfunc;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!cur)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  /* At this points, no new block allocation should be done */
  snprintf(buffer, sizeof (buffer), "%s%lX",
	   (char *)elfsh_config_get_data(MJR_COFING_CALL_PREFIX),
	   (unsigned long) cur->vaddr);
  sym = elfsh_get_symbol_by_name(buf->obj, buffer);
  if (sym)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC," [*] Saving block at addr %s \n", buffer);
#endif

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
    {
      buf->allocated = getpagesize();;
      buf->data = elfsh_malloc(buf->allocated);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + sizeof(mjrblock_t)))
    {
      buf->allocated += getpagesize();
      buf->data = elfsh_realloc(buf->data, buf->allocated);
    }
  curfunc         = (mjrfunc_t *) ((char *) buf->data + buf->maxlen);
  memcpy(curfunc, cur, sizeof(mjrfunc_t));
  
  /* Clean the parent/child lists 
     FIXME: this should be set to an offset in .edfmt.fcontrol so we
	    will be able to restore all data structures on load
	    without recomputing it.
 
  curfunc->parentfuncs = NULL;
  curfunc->childfuncs = NULL;
  */

  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(cur->vaddr, cur->size, STT_FUNC, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(mjrfunc_t);
  //  buf->block_counter++;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

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
  char			funcname[50];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Remove previous control section if any */
  sect = elfsh_get_section_by_name(ctxt->obj, ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 0, 0, 0);
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
      mjr_function_dump(__FUNCTION__, func);

      /* 1st Save flow information and get offset from the beginning of .edfmt.fcontrol */
      flowOffp = mjr_function_flow_parents_save(func, &cfbuf);
      fprintf(D_DESC, " [D] %s: parentOffset: %d\n", __FUNCTION__, flowOffp);

      flowOffc = mjr_function_flow_childs_save(func, &cfbuf);
      fprintf(D_DESC, " [D] %s: childOffset: %d\n", __FUNCTION__, flowOffc);
      
      /* Set New pointers */
      func->parentfuncs = flowOffp;
      func->childfuncs = flowOffc;

      /* 2nd Save function structure */
      mjr_function_save(func, &buf);

    }

  /* Create control section */
  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_FUNCTIONS);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);
  sect->altdata = ctxt->blklist = (mjrfunc_t *) buf.data;

  printf(" [*] Saving %s section of %u bytes \n", 
	 ELFSH_SECTION_NAME_EDFMT_FUNCTIONS,
	 buf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);

  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save .edfmt.functions section", -1);

  sect = elfsh_create_section(".edfmt.fcontrol");
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, cfbuf.maxlen, 0, 0, 0, 0);
  sect->altdata = cfbuf.data;

  printf(" [*] Saving %s section of %u bytes\n",
	 ".edfmt.fcontrol",
	 cfbuf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, cfbuf.data);

  if (err < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to save .edfmt.fcontrol section", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}


