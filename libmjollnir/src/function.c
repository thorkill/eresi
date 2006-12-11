/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: function.c,v 1.4 2006-12-11 13:57:39 may Exp $
 *
 */
#include <libmjollnir.h>


/* Copy the function in a special buffer to fingerprint it */
int		mjr_i386_function_copy(mjrsession_t  *ctx, 
				       unsigned char *src, 
				       unsigned char *dst, 
				       int	     mlen)
{
  int             n, ilen, p;
  asm_instr       instr, hist[4];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  for (p = ilen = n = 0; n < mlen; n += ilen) 
    {
      ilen = asm_read_instr(&instr, src + n, mlen - n, &ctx->cur->proc);
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
void		*mjr_fingerprint_function(mjrsession_t  *ctx, 
					  unsigned char *buff, 
					  int		type) 
{
 MD5_CTX	md5ctx;
 unsigned char  *fbuf, digest[16];
 char		*pt;
 void           *ret;
 u_int		i;
 int 		mlen;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 ret = NULL;

 /* Select the desired fingerprinting function */
 switch (type) 
   {
   case MJR_FNG_TYPE_MD5:
     XALLOC(fbuf, MJR_MAX_FUNCTION_LEN, NULL);
     memset(fbuf, 0, MJR_MAX_FUNCTION_LEN);
     mlen = mjr_i386_function_copy(ctx, buff, fbuf, MJR_MAX_FUNCTION_LEN);
     if (mlen <= 0) 
       {
	 XFREE(fbuf);
	 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));
       }
     MD5_Init(&md5ctx);
     MD5_Update(&md5ctx, fbuf, mlen);
     MD5_Final(digest, &md5ctx);
     XFREE(fbuf);
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
mjrfunction_t	*mjr_function_create(u_int vaddr) 
{
 mjrfunction_t *fun;
 
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__); 
 XALLOC(fun, sizeof(mjrfunction_t), NULL);
 fun->vaddr = vaddr;
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (fun)); 
}
