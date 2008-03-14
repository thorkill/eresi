/*
** @file hash.c
** @ingroup libkernsh
** $Id: hash.c,v 1.2 2008-08-02 20:43:00 pouik Exp $
**
*/
#include "libkernsh.h"
#include <openssl/md5.h>

unsigned char *kernsh_hash_md5(unsigned long addr, int size)
{
  MD5_CTX md5ctx;
  unsigned char *buff;
  unsigned char *digest;
  unsigned long start;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, buff, size + 1, NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, digest, LIBKERNSH_HASH_MD5_SIZE, NULL);

  memset(buff, '\0', size);
  memset(digest, '\0', LIBKERNSH_HASH_MD5_SIZE);

  if (kernsh_is_mem_mode())
    {
      kernsh_readmem(addr, buff, size);
    }
  else
    {
      start = elfsh_get_foffset_from_vaddr(libkernshworld.root, 
					   addr);
      elfsh_raw_read(libkernshworld.root, start, buff, size);
    }

  MD5_Init(&md5ctx);
  MD5_Update(&md5ctx, buff, size);
  MD5_Final(digest, &md5ctx);

  XFREE(__FILE__, __FUNCTION__, __LINE__, buff);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, digest);
}

unsigned char *kernsh_hash(unsigned long addr, int size, int *new_size)
{
  int hash;
  unsigned char *buffer;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  buffer = NULL;
  hash = (int)config_get_data(LIBKERNSH_VMCONFIG_HASH);

  if (size < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Size is negative", NULL);
    }
  
  /* We must find ret */
  if (size == 0)
    {
      size = kernsh_find_end(addr);
      if (!size)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Can't find the size", NULL);
	}
    }

  *new_size = size;

  switch (hash)
    {
    case LIBKERNSH_HASH_MD5 :
      buffer = kernsh_hash_md5(addr, size);
      break;
    case LIBKERNSH_HASH_SHA1 :
      break;

    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buffer);
}
