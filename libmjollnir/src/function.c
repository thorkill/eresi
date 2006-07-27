/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: function.c,v 1.3 2006-07-27 16:50:53 thor Exp $
 *
 */

#include <libmjollnir.h>

int
mjr_i386_function_copy(mjrSession *ctx, unsigned char *src, unsigned char *dst, int mlen)
{
        int             n         , ilen, p;
        asm_instr       instr, hist[4];

        p = 0;
        ilen = 0;
        for (n = 0; n < mlen; n += ilen) {
                ilen = asm_read_instr(&instr, src + n, mlen - n, &ctx->cur->proc);
                if (ilen <= 0) {
                        return p;
                }
                /* filter this out */
                if (instr.instr != ASM_NOP) {
                        dst[p++] = src[n];
                }
                /* epilog */
                if ((hist[0].instr == ASM_LEAVE) && (instr.instr == ASM_RET)) {
                        dst[p++] = src[n];
                        return p;
                } else if ((hist[0].instr == ASM_POP) && (instr.instr == ASM_RET)) {
                        dst[p++] = src[n];
                        return p;
                }
                hist[1] = hist[0];
                hist[0] = instr;
        }

        return 0;
}


/*
 * finger print a function using X method
 */

void * mjr_fingerprint_function(mjrSession * ctx, unsigned char *buff, int type) {
 MD5_CTX		md5ctx;
 unsigned char  *fbuf, digest[16], *pt;
 void           *ret;
 u_int		i;
 int 		mlen;
// mjr_block	*blk;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 ret = NULL;

 switch (type) {

  case MJR_FNG_TYPE_MD5:

   XALLOC(fbuf, MJR_MAX_FUNCTION_LEN, NULL);
   memset(fbuf, 0, MJR_MAX_FUNCTION_LEN);

   mlen = mjr_i386_function_copy(ctx, buff, fbuf, MJR_MAX_FUNCTION_LEN);

   if (mlen <= 0) {
    XFREE(fbuf);
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(NULL));
   }

   MD5_Init(&md5ctx);
   MD5_Update(&md5ctx, fbuf, mlen);
   MD5_Final(digest, &md5ctx);
   XFREE(fbuf);

   XALLOC(ret,33, NULL);
   if (!ret)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,(NULL));

   memset(ret, 0, 33);
   pt = ret;
   for (i = 0; i < 16; i++) {
    sprintf(pt, "%02x", digest[i]);
    pt += 2;
   }

   break;
  }
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


mjrFunction * mjr_function_create(u_int vaddr) {
 mjrFunction *fun;
 
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 
 XALLOC(fun, sizeof(mjrFunction), NULL);
 
 fun->vaddr = vaddr;

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (fun)); 
}
