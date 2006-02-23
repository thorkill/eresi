
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: fngengine.c,v 1.1.1.2 2006-02-23 20:25:15 thor Exp $
 */

#include <libmjollnir.h>

/* 
 finger print a function using X method 
 */

void * mjr_fingerprint_function(unsigned char *buff,int type, int mlen) {
 MD5_CTX md5ctx;
 unsigned char *fbuf,digest[16],*pt;
 void *ret;
 u_int i;
 
 ret = NULL;
 
 if (mlen < 0) {return NULL;}

 switch(type) {

  case FNG_TYPE_MD5:
   fbuf = malloc(MAX_FUNCTION_LEN);
   memset(fbuf,0,MAX_FUNCTION_LEN);

   mlen = mjr_i386_cut_function(buff,fbuf,MAX_FUNCTION_LEN);

   if (mlen <= 0) {free(fbuf); return NULL;}

   MD5_Init(&md5ctx); 
   MD5_Update(&md5ctx,fbuf,mlen);
   MD5_Final(digest,&md5ctx);
   free(fbuf);
   
   ret = malloc(32+1);
    if (!ret)
	 return NULL;
   
   pt = ret;

   for(i=0;i<16;i++){
    sprintf(pt,"%02x",digest[i]);
	pt += 2;
   }
   break;
 }
 
 return ret;
}
