/*
 * (C) 2006-2007 Asgard Labs, thorolf
 *     2006-2007 devhell, mayhem
 *     2007      rfd labs, strauss
 *
 * BSD License
 * $Id: function.c,v 1.34 2007-04-09 15:18:05 thor Exp $
 *
 */
#include <libmjollnir.h>

/**
 * Function dumping procedure for debug purposes
 */

void mjr_function_dump(mjrcontext_t* ctxt, char *where, mjrcontainer_t *c)
{
  mjrfunc_t *f, *tmp;
  mjrlink_t *cur;
  f = (mjrfunc_t *) c->data;

  fprintf(D_DESC," [D] FUNC DUMP in {%s}: %x/<%s>[%s] No. Children: %d, No. Parents: %d\n",
	  where, f->vaddr, (f->name) ? f->name : NULL ,(f->md5) ? f->md5 : NULL ,c->out_nbr, c->in_nbr);
  
  if (c->output)
  {
    fprintf(D_DESC," [D] Child functions:\n [x] ");

    cur = c->output;

    while(cur)
      {
	tmp = (mjrfunc_t *) mjr_lookup_container(ctxt,cur->id)->data;
	fprintf(D_DESC,"%x ", tmp->vaddr);
	cur = cur->next;
      }
    
    fprintf(D_DESC,"\n");
  }
  
  if (c->input)
  {
    fprintf(D_DESC," [D] Parent functions:\n [x] ");
    cur = c->input;
    
    while(cur)
      {
	tmp = (mjrfunc_t *) mjr_lookup_container(ctxt,cur->id)->data;
	fprintf(D_DESC,"%x ", tmp->vaddr);
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

u_int	 mjr_function_flow_save(mjrcontainer_t *c, u_int type, mjrbuf_t *buf)
{
  u_int curOff,nbr;
  mjrlink_t *cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (type == 0) 
  {
    cur = c->input;
    nbr = c->in_nbr;
  } 
  else if (type == 1)
  {
    cur = c->output;
    nbr = c->out_nbr;
  }

  if (!cur)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf->maxlen));
  
  if (!buf->data) 
  {
    buf->allocated = getpagesize();
    XALLOC(__FILE__, __FUNCTION__, __LINE__, buf->data, buf->allocated, -1);
    buf->maxlen = 0;
  }
  else if (buf->allocated  < (buf->maxlen + (sizeof(unsigned int) * nbr * 2)))
  {
    buf->allocated += getpagesize();
    XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->data, buf->allocated, -1);
  }

#if __DEBUG_FUNCS__
  fprintf(D_DESC,"[D] %s: allocated: %d needed: %d endsize: %d pagesize: %d\n",
	  __FUNCTION__, buf->allocated,
	  (sizeof(unsigned int) * nbr * 2),
	  buf->maxlen + (sizeof(unsigned int) * nbr * 2),
	  getpagesize());
#endif

  while (cur)
  {
    memcpy(buf->data + buf->maxlen, (char *)&cur->id , sizeof(unsigned int));
    buf->maxlen += sizeof(unsigned int);
    memcpy(buf->data + buf->maxlen, (char *)&cur->type , sizeof(int));
    buf->maxlen += sizeof(int);
    cur = cur->next;
  }

  curOff = buf->maxlen;
  
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
  MD5_CTX		md5ctx;
  unsigned char		fbuf[MJR_MAX_FUNCTION_LEN] = {0x00};
  unsigned char		digest[16];
  char			*pt;
  void			*ret;
  u_int			i;
  int 			mlen;
  elfsh_SAddr		off;
  elfshsect_t		*sect;
  unsigned char		*buff;
  
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

/**
 * Load function data from the file into memory
 *
 * returns the number of functions (0 probably means something is wrong)
 */
int			mjr_functions_load(mjrcontext_t *ctxt)
{
  int			index, findex, tmptype, cnt;
  elfshsect_t		*sect, *flowsect;
  u_int			fncnbr, off;
  unsigned int		tmpid;
  mjrfunc_t		*curfnc;
  mjrcontainer_t 	*curcntnr;
  char			**keys;
  int			done,flowdone;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sect = elfsh_get_section_by_name(ctxt->obj, 
				   ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 
				   0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "No function section : use analyse command", 0);

  /* get flow section */
  /* FIXME: use macros!!! */
  flowsect = elfsh_get_section_by_name(ctxt->obj,
				       ELFSH_SECTION_NAME_EDFMT_FCONTROL,
				       0, 0, 0);
  
  if (!flowsect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "No function flow section : use analyse command", 0);
  
  fncnbr = sect->shdr->sh_size / (sizeof(mjrcontainer_t) + sizeof(mjrfunc_t));
  fprintf(D_DESC, " [D] %s: loading %d functions\n", __FUNCTION__, fncnbr);
  
  done = 0;
  for (index=0; index < fncnbr; index++)
    {
      mjrfunc_t *newfunction;
      mjrcontainer_t *newcntnr;

      curcntnr = (mjrcontainer_t *) ((char *)sect->data + done);
      done += sizeof(mjrcontainer_t);
      curfnc = (mjrfunc_t *) ((char *)sect->data + done);
      done += sizeof(mjrfunc_t);

      XALLOC(__FILE__, __FUNCTION__, __LINE__, newfunction, sizeof(mjrfunc_t), NULL);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, newcntnr, sizeof(mjrcontainer_t), NULL);
      
      memcpy(newcntnr, curcntnr, sizeof(mjrcontainer_t));
      memcpy(newfunction, curfnc, sizeof(mjrfunc_t));
      
      newcntnr->data = newfunction;
      mjr_register_container_id(ctxt,newcntnr);
      
      hash_add(&ctxt->funchash, (char *) _vaddr2str(curfnc->vaddr), newcntnr);
    }

  /*
   * 2nd run to relink the containers
   */
  keys = hash_get_keys(&ctxt->funchash, &done);
  for (index = 0; index < done; index++)
    {
      int tidx;		/* 
			   Temp index to prevent endless loops
			   linking increments the in_nbr/out_nbr
			*/

      curcntnr = hash_get(&ctxt->funchash, keys[index]);

      off = (u_int)curcntnr->input;
      curcntnr->input = NULL;

      /* fill the flow list */
      flowdone = 0;
      /* Prevent endless loops */
      tidx = curcntnr->in_nbr;
      curcntnr->in_nbr = 0;

      for (findex=0; findex < tidx; findex++)
	{	 
	  tmpid = *(unsigned int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(unsigned int);
	  tmptype = *(int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(int);

#if __DEBUG_FUNCS__	
	  fprintf(D_DESC,"[D] resotore parent: (%d/%d) sid:%u did:%u type:%u\n", 
		  findex,tidx,
		  curcntnr->id,tmpid,tmptype);
#endif

	  if (tmpid)
	    mjr_container_add_link(curcntnr, tmpid, tmptype, MJR_LINK_IN);
	}
    
      off = (u_int)curcntnr->output;
      curcntnr->output = NULL;
    
      /* Prevent endless loops */
      tidx = curcntnr->out_nbr;
      curcntnr->out_nbr = 0;

      for (findex=0; findex < tidx; findex++)
	{
	  tmpid = *(unsigned int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(unsigned int);
	  tmptype = *(int *)((char *)flowsect->data + off + flowdone);
	  flowdone += sizeof(int);

#if __DEBUG_FUNCS__
	  fprintf(D_DESC,"[D] resotore child: (%d/%d) sid:%u did:%u type:%u\n", 
		  findex,tidx,
		  curcntnr->id,tmpid,tmptype);
#endif

	  if (tmpid)
	    mjr_container_add_link(curcntnr, tmpid , tmptype, MJR_LINK_OUT);
	}

#if __DEBUG_FUNCS__
    mjr_function_dump((char *)__FUNCTION__, curcntnr);
#endif
  }
  
  cnt = hash_size(&ctxt->funchash);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
}


/** 
 * Retrieve control flow section content if any 
 *
 * returns the number of functions (0 probably means something is wrong)
 */
int			mjr_functions_get(mjrcontext_t *ctxt)
{
  elfshsect_t	*sect;
  int		cnt;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(ctxt->obj, 
				   ELFSH_SECTION_NAME_EDFMT_FUNCTIONS, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No functions section : use analyse command", 0);
  
  /* Return or retreive information */
  cnt = hash_size(&ctxt->funchash);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);

  cnt = mjr_functions_load(ctxt);

  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Flow analysis failed", 0);
}


/* Create the block information to be saved in file */
int			mjr_function_save(mjrcontainer_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrfunc_t		*curfunc, *fnc;
  mjrcontainer_t 	*curcntnr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!cur)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
  
  fnc = cur->data;

  /* At this points, no new block allocation should be done */
  snprintf(buffer, sizeof (buffer), "%s%lX",
	   (char *) config_get_data(MJR_COFING_CALL_PREFIX),
	   (unsigned long) fnc->vaddr);

  sym = elfsh_get_symbol_by_name(buf->obj, buffer);

  if (sym)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

#if __DEBUG_BLOCKS__
  fprintf(D_DESC," [*] Saving block at addr %s \n", buffer);
#endif

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
  {
    buf->allocated = getpagesize();
    XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	   buf->data, buf->allocated, -1);
    buf->maxlen = 0;
  }
  else if (buf->allocated  < (buf->maxlen + sizeof(mjrcontainer_t) + sizeof(mjrfunc_t)))
  {
    buf->allocated += getpagesize();
    XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	     buf->data, buf->data, buf->allocated, -1);
  }

  curcntnr = (mjrcontainer_t *) ((char *) buf->data + buf->maxlen);
  curfunc = (mjrfunc_t *) ((char *) buf->data + buf->maxlen + sizeof(mjrcontainer_t));

  cur->data = NULL;
  memcpy(curcntnr, cur, sizeof(mjrcontainer_t));
  memcpy(curfunc, fnc, sizeof(mjrfunc_t));

  cur->data = fnc;
  /* 
     Then we create the symbol for the function and return 
     when symbol doesn't exists in symtab
  */
  sym = elfsh_get_symbol_by_value(buf->obj, curfunc->vaddr, 0, ELFSH_EXACTSYM);

  if (!sym)
  {
    bsym = elfsh_create_symbol(curfunc->vaddr, curfunc->size, STT_FUNC, 0, 0, 0);
    elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  }

  buf->maxlen += sizeof(mjrcontainer_t);
  buf->maxlen += sizeof(mjrfunc_t);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* XXX: You should never use printf but vm_output */
int			mjr_functions_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf,cfbuf;
  mjrcontainer_t	*cntnr;
  int			err;
  char			**keys;
  int			keynbr;
  int			index;
  u_int			flowOffp,flowOffc,tmpoff;

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

  /* offset is set to 0 */
  flowOffp = flowOffc = 0;

  /* Iteratively save all blocks */
  keys = hash_get_keys(&ctxt->funchash, &keynbr);
  for (index = 0; index < keynbr; index++)
  {
    mjrlink_t *tmpp,*tmpc;

    tmpp = tmpc = NULL;

    cntnr = hash_get(&ctxt->funchash, keys[index]);

#if __DEBUG_FUNCS__
    mjr_function_dump((char *)__FUNCTION__, cntnr);
#endif

    /* Set New pointers */
    tmpp = cntnr->input;
    tmpc = cntnr->output;
    cntnr->input = (mjrlink_t *)flowOffp;
    cntnr->output = (mjrlink_t *)flowOffc;

    /* Save function structure */
    mjr_function_save(cntnr, &buf);

    /* restore old containers */
    cntnr->input = tmpp;
    cntnr->output = tmpc;

#if __DEBUG_FUNCS__
    fprintf(D_DESC, " [D] %s: parentOffset: %d\n", __FUNCTION__, flowOffp);
    fprintf(D_DESC, " [D] %s: childOffset: %d\n", __FUNCTION__, flowOffc);
#endif

    /* save function flow and get new offset */

    tmpoff = mjr_function_flow_save(cntnr, 0, &cfbuf);
    if (tmpoff > 0)
      flowOffp = tmpoff;

    if (tmpoff == -1)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to save function flow ", -1);

    tmpoff = mjr_function_flow_save(cntnr, 1, &cfbuf);
    if (tmpoff > 0)
      flowOffc = tmpoff;

    if (tmpoff == -1)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to save function flow ", -1);

  }

  /* Create control section */
  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_FUNCTIONS);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);

  printf(" [*] Saving %s section of %u bytes \n", 
	 ELFSH_SECTION_NAME_EDFMT_FUNCTIONS,
	 buf.maxlen);
  
  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);
  
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to save .edfmt.functions section", -1);

  sect = elfsh_create_section(ELFSH_SECTION_NAME_EDFMT_FCONTROL);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, cfbuf.maxlen, 0, 0, 0, 0);
  
  printf(" [*] Saving %s section of %u bytes\n",
	 ".edfmt.fcontrol",
	 cfbuf.maxlen);
  
  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, cfbuf.data);

  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to save .edfmt.fcontrol section", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}

/**
 * Link function layer
 * @param ctxt mjollnir context structure
 * @param str source address
 * @param dst destination address
 * @param ret return address
 */
int	mjr_functions_link_call(mjrcontext_t *ctxt, 
				elfsh_Addr src, 
				elfsh_Addr dst, 
				elfsh_Addr ret)
{
  u_int			tmpaddr;
  mjrcontainer_t	*true,*fun;
  mjrfunc_t		*tmpfunc;
  char			*tmpstr,*md5;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_FUNCS__
  fprintf(D_DESC,"[D] %s: src:%x dst:%x ret:%x\n",
	  __FUNCTION__, src, dst, ret);
#endif
  /* Link/Prepare function layer */
  true = mjr_block_get_by_vaddr(ctxt, dst, MJR_BLOCK_GET_STRICT);

  /* XXX: put this in a vector of fingerprinting techniques */
  tmpaddr = ((mjrblock_t *)mjr_lookup_container(ctxt,true->id)->data)->vaddr;

  tmpstr = _vaddr2str(tmpaddr);
  fun = hash_get(&ctxt->funchash, tmpstr);

  if (!fun)
    {
      fun = mjr_create_function_container(ctxt, tmpaddr, 0, tmpstr, NULL, NULL);
      hash_add(&ctxt->funchash, tmpstr, fun);
    }

  if (ctxt->curfunc)
    {
      mjr_container_add_link(fun, ctxt->curfunc->id, 
			     MJR_LINK_FUNC_RET, MJR_LINK_IN);
      mjr_container_add_link(ctxt->curfunc, fun->id, 
			     MJR_LINK_FUNC_CALL, MJR_LINK_OUT);
    }

  // when a function start, we do a fingerprint of it
  md5 = mjr_fingerprint_function(ctxt, tmpaddr, MJR_FPRINT_TYPE_MD5);

  tmpfunc = (mjrfunc_t *) fun->data;

  if (md5)
    snprintf(tmpfunc->md5,sizeof(tmpfunc->md5),"%s",md5);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
