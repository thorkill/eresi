/**
 * @file function.c
 * 
 * @brief funtion containers related api
 *
 * (C) 2006-2008 Asgard Labs, thorolf
 *     2006-2007 devhell, jfv
 *     2007      rfd labs, strauss
 *
 */
 
#include <libmjollnir.h>


/** 
 * @brief Retrieve control flow section content if any 
 * @param ctxt mjorllnir context
 * @return returns the number of functions (0 probably means something is wrong)
 */
int		mjr_functions_get(mjrcontext_t *ctxt)
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
  
  /* Retreive information */
  cnt = hash_size(&ctxt->funchash);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);

  /* Return result */
  cnt = mjr_flow_load(ctxt, ASPECT_TYPE_FUNC);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Flow analysis failed", -1);
}



/**
 * @brief Function dumping procedure for debug purposes
 */
void		mjr_function_dump(mjrcontext_t *ctxt, char *where, container_t *c)
{
  mjrfunc_t	*f, *tmp;
  listent_t	*ent;
  mjrlink_t	*cur;

  f = (mjrfunc_t *) c->data;
  fprintf(D_DESC," [D] FUNC DUMP in {%s}: " AFMT "/<%s>[%s] ID:%d No. Children: %d, No. Parents: %d\n",
	  where, f->vaddr, (f->name) ? f->name : NULL ,
	  (f->md5) ? f->md5 : NULL , c->id, c->outlinks->elmnbr, c->inlinks->elmnbr);
  
  if (c->outlinks->head)
    {
      fprintf(D_DESC," [D] Child functions:\n [x] ");
      for (ent = c->outlinks->head; ent; ent = ent->next)
	{
	  cur = (mjrlink_t *) ent->data;
	  tmp = (mjrfunc_t *) mjr_lookup_container(ctxt,cur->id)->data;
	  fprintf(D_DESC, AFMT" ", tmp->vaddr);
	}
      fprintf(D_DESC,"\n");
    }
  
  if (c->inlinks->head)
    {
      fprintf(D_DESC," [D] Parent functions:\n [x] ");
      for (ent = c->inlinks->head; ent; ent = ent->next)
	{
	  cur = (mjrlink_t *) ent->data;
	  tmp = (mjrfunc_t *) mjr_lookup_container(ctxt,cur->id)->data;
	  fprintf(D_DESC, AFMT" ", tmp->vaddr);
	}
      fprintf(D_DESC,"\n");
    }
}


/** 
 * @brief Copy the function in a special buffer to fingerprint it 
 */
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



/** 
 * @brief Finger print a function using X method 
 */
void			*mjr_fingerprint_function(mjrcontext_t  *ctx, 
						  eresi_Addr	addr, 
						  int		type) {

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
      XALLOC(__FILE__, __FUNCTION__, __LINE__, ret, 34, NULL);
      
      if (!ret)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
      
      memset(ret, 0, 34);
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
 * @brief Register function container in the context
 * @param ctx mjollnir context structure
 * @param vaddr virtual address of the function
 * @param fun function container
 */
int		mjr_function_register(mjrcontext_t *ctx, u_int vaddr, container_t *fun)
{
  char		*tmpstr;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_FUNCS__
  fprintf(D_DESC, "[D] %s: vaddr:%x ID:%d\n",
	  __FUNCTION__, vaddr, fun->id);
#endif

  if (!mjr_function_get_by_vaddr(ctx, vaddr))
    {
#if __DEBUG_FUNCS__
      fprintf(stderr, " [*] Registered new function starting at 0x%08X \n", vaddr);
#endif
      tmpstr = _vaddr2str(vaddr);
      hash_add(&ctx->funchash, tmpstr, fun);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * @brief Get function container by vaddr
 * @param ctx mjollnir context structure
 * @param vaddr virtual address of requested function
 */
container_t		*mjr_function_get_by_vaddr(mjrcontext_t *ctx, u_int vaddr)
{
  container_t	*container;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_FUCNS_LOOKUP__
  fprintf(D_DESC,"[D] %s: vaddr:%x\n",__FUNCTION__,vaddr);
#endif

  container = (container_t *) hash_get(&ctx->funchash, _vaddr2str(vaddr));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, container);
}




/**
 * @brief Inject a symbol associated to a function container
 * @param csrc Container for which a symbol has to be injected
 * @param curaddr Address of currently analyzed instruction
 * @return Always 0
 */
int			mjr_function_symbol(mjrcontext_t *ctxt, container_t *csrc)
{
  mjrfunc_t		*func;
  elfsh_Sym		bsym;
  char			*prefix;
  char			buffer[BUFSIZ];
  char			*name;
  elfsh_SAddr		off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  func   = (mjrfunc_t *) csrc->data;

  /* Check if symbol already exists */
  name = elfsh_reverse_metasym(ctxt->obj, func->vaddr, &off);
  prefix = (char *) config_get_data(MJR_CONFIG_BLOC_PREFIX);
  if (!off && !strstr(name, prefix))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* If not, inject it */
  prefix = (char *) config_get_data(MJR_CONFIG_FUNC_PREFIX);
  snprintf(buffer, sizeof(buffer), "%s"AFMT, prefix, func->vaddr);
  bsym = elfsh_create_symbol(func->vaddr, func->size, STT_FUNC, 0, 0, 0);
  elfsh_insert_symbol(ctxt->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
