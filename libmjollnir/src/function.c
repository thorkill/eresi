/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: function.c,v 1.10 2007-01-08 11:23:58 may Exp $
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
 hash_init(&fun->parentfuncs, 5, ELEM_TYPE_PROC);
 hash_init(&fun->childfuncs, 5, ELEM_TYPE_PROC);
 hash_add(&c->funchash, name, fun);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (fun)); 
}
