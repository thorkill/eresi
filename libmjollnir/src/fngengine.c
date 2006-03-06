
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de 
 * BSD License 
 * $Id: fngengine.c,v 1.1.1.3 2006-03-06 22:35:22 thor Exp $
 */

#include <libmjollnir.h>

/*
 * finger print a function using X method
 */

void           *
mjr_fingerprint_function(Mjr_CTX * ctx, unsigned char *buff, int type, int mlen)
{
	MD5_CTX		md5ctx;
	unsigned char  *fbuf, digest[16], *pt;
	void           *ret;
	u_int		i;

	ret = NULL;

	if (mlen < 0) {
		return NULL;
	}
	switch (type) {

	case FNG_TYPE_MD5:
		fbuf = elfsh_malloc(MAX_FUNCTION_LEN);
		memset(fbuf, 0, MAX_FUNCTION_LEN);

		mlen = mjr_i386_cut_function(ctx, buff, fbuf, MAX_FUNCTION_LEN);

		if (mlen <= 0) {
			elfsh_free(fbuf);
			return NULL;
		}
		MD5_Init(&md5ctx);
		MD5_Update(&md5ctx, fbuf, mlen);
		MD5_Final(digest, &md5ctx);
		elfsh_free(fbuf);

		ret = elfsh_malloc(32 + 1);
		if (!ret)
			return NULL;

		memset(ret, 0, 33);

		pt = ret;

		for (i = 0; i < 16; i++) {
			sprintf(pt, "%02x", digest[i]);
			pt += 2;
		}
		break;
	}

	return ret;
}
