/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: function.c,v 1.11 2007-01-09 17:14:23 thor Exp $
 *
 */
#include <libmjollnir.h>


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
     XALLOC(ret, 33, NULL);
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
 XALLOC(fun, sizeof(mjrfunc_t), NULL);
 fun->vaddr = addr;
 fun->size  = size;
 fun->first = e;
 fun->name  = name;
 fun->parentfuncs = NULL;
 fun->childfuncs = NULL;
 hash_add(&c->funchash, name, fun);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (fun)); 
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

  ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);
}

/* Create the block information to be saved in file */
int			mjr_function_save(mjrfunc_t *cur, mjrbuf_t *buf)
{
  char			buffer[24];
  elfsh_Sym		bsym;
  elfsh_Sym		*sym;
  mjrfunc_t		*curfunc;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
  
  /* Then we create the symbol for the bloc and returns */
  bsym = elfsh_create_symbol(cur->vaddr, cur->size, STT_FUNC, 0, 0, 0);
  elfsh_insert_symbol(buf->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, buffer);
  buf->maxlen += sizeof(mjrfunc_t);
  //  buf->block_counter++;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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

  ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);
}

int			mjr_functions_store(mjrcontext_t *ctxt) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf;
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

  /* Iteratively save all blocks */
  keys = hash_get_keys(&ctxt->funchash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      func = hash_get(&ctxt->funchash, keys[index]);
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.block_counter);
}

