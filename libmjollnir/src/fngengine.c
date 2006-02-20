
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: fngengine.c,v 1.1.1.1 2006-02-20 23:25:27 thor Exp $
 */

#include <libmjollnir.h>

/* 
 finger print a function using X method 
 */

void * mjr_fingerprint_function(unsigned char *buff,int type, int mlen) {
 MD5_CTX md5ctx;
 void *ret;
 unsigned char *fbuf;
 
 ret = NULL;
 
 if (mlen < 0) {return NULL;}

 switch(type) {

  case FNG_TYPE_MD5:
   fbuf = malloc(MAX_FUNCTION_LEN);
   memset(fbuf,0,MAX_FUNCTION_LEN);

   mlen = mjr_i386_cut_function(buff,fbuf,MAX_FUNCTION_LEN);

   if (mlen <= 0) {free(fbuf); return NULL;}

   MD5Init(&md5ctx); 
   MD5Update(&md5ctx,fbuf,mlen);
   ret = MD5End(&md5ctx,NULL);
   free(fbuf);

   break;
 
 }
 
 return ret;
}

