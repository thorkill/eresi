/**
** @file hash.c
** @ingroup elf
*/
/**
 *
 * 
 * @brief Hash features
 * 
 * Started on  Sun Oct 07 10:20:18 2005 mxatone
 *
 * $Id$
*
*/
#include "libstderesi.h"

/**
 * Need doxygen comment
 * @param index
 * @return
 */
int		revm_hashunk(int index)
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ -1,
	   " %s %s\n", 
	   revm_colornumber("[%03u]", index),
	   revm_colortypestr("STN_UNDEF"));
  revm_output(logbuf);
  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment.
 * @param type
 * @param index
 * @param symid
 * @param name
 * @param rhash
 * @param hash
 * @param check
 * @return
 */

int		revm_hashbucket_print(int type, int index, int symid, 
				   char *name, int rhash, int hash, int check)
{
  char		logbuf[BUFSIZ];
  char		ipattern[16];
  int		quiet;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(ipattern, 15, 
	   (type == 0 ? "[%%03u]" : "~>%%03u"));

  quiet = world.state.revm_quiet;

  snprintf(logbuf, BUFSIZ - 1,
	   " %s %s%s %s%s %s%s %s%s %s%s\n", 
	   revm_colornumber(ipattern, index), 
	   revm_colorfieldstr("sym:"),
	   revm_colornumber("%03u", symid), 
	   revm_colorfieldstr("name:"),
	   revm_colorstr_fmt((!quiet ? "%-40s" : "%-15s"), name), 
	   revm_colorfieldstr("hash:"),
	   revm_colornumber("%08x", rhash),
	   revm_colorfieldstr("hashm:"),
	   revm_colornumber("%02u", hash),
	   revm_colorfieldstr("check:"),
	   (check ? "ok" : "bad"));

  revm_output(logbuf);
  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment.
 * @param index
 * @param symid
 * @param name
 * @param rhash
 * @param hash
 * @return
 */
int		revm_hashchain_print(int index, int symid, char *name, 
				  int rhash, int hash)
{
  char		logbuf[BUFSIZ];
  int		quiet;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  quiet = world.state.revm_quiet;

  snprintf(logbuf, BUFSIZ - 1,
	   " %s %s%s %s%s %s%s %s%s\n", 
	   revm_colornumber("[%03u]", index), 
	   revm_colorfieldstr("sym:"),
	   revm_colornumber("%03u", symid), 
	   revm_colorfieldstr("name:"),
	   revm_colorstr_fmt((!quiet ? "%-40s" : "%-15s"), name), 
	   revm_colorfieldstr("hash:"),
	   revm_colornumber("%08x", rhash),
	   revm_colorfieldstr("hashm:"),
	   revm_colornumber("%02u", hash));

  revm_output(logbuf);
  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment
 * @param sect
 * @param sym
 * @param nbr
 * @return
 */
int		revm_hashprint(elfshsect_t *sect, elfsh_Sym *sym, int nbr)
{
  void		*data;
  elfsh_Word   	hash, chash;
  elfsh_Word   	rhash, rchash;
  elfsh_Word   	nbucket;
  elfsh_Word   	nchain;
  elfsh_Word   	*chain;
  elfsh_Word   	*bucket;
  elfsh_Word   	index, cindex;
  elfsh_Word   	symid;
  char		logbuf[BUFSIZ];
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = sect->data;

  nbucket = elfsh_get_hashnbucket(data);
  nchain  = elfsh_get_hashnchain(data);
  bucket  = elfsh_get_hashbucket(data);
  chain   = elfsh_get_hashchain(data);

  snprintf(logbuf, BUFSIZ - 1,
	   " {Section %s}\n"
	   " {nbucket: %u nchain: %u}\n\n", 
	   sect->name, nbucket, nchain);
  
  revm_output(logbuf);
  revm_endline();

  revm_output(" [bucket]\n");

  for (index = 0; index < nbucket; index++)
    {
      symid = bucket[index];
      name = elfsh_get_dynsymbol_name(world.curjob->curfile,
				      sym + symid);

      rhash = elfsh_get_symbol_hash(name);
      hash = rhash % nbucket;
      
      if (symid == STN_UNDEF)
	{
	  revm_hashunk(index);
	  continue;
	}

      revm_hashbucket_print(0, index, symid, name, rhash, hash, (index == hash));
      
      for (cindex = symid; cindex < nchain; cindex = symid)
	{
	  symid = chain[cindex];

	  if (symid == STN_UNDEF)
	    break;

	  name = elfsh_get_dynsymbol_name(world.curjob->curfile,
					  sym + symid);
	  rchash = elfsh_get_symbol_hash(name);
	  chash = rchash % nbucket;

	  revm_hashbucket_print(1, cindex, symid, name, rchash, chash, (hash == chash));
	}
    }

  revm_output("\n [chain]\n");
  
  for(index = 0; index < nchain; index++)
    {
      symid = chain[index];
      name = elfsh_get_dynsymbol_name(world.curjob->curfile,
				      sym + symid);

      rhash = elfsh_get_symbol_hash(name);
      hash = rhash % nbucket;

      if (symid == STN_UNDEF)
	{
	  revm_hashunk(index);
	  continue;
	}

	revm_hashchain_print(index, symid, name, rhash, hash);
    }

  revm_output("\n");  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment.
 *
 *
 */
int 		cmd_hashx()
{
  elfshsect_t	*sect;
  elfsh_Sym	*sym;
  char		logbuf[BUFSIZ];
  eresi_Addr	range = 0;
  int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sym = elfsh_get_dynsymtab(world.curjob->curfile, NULL);
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot get DYNSYM",  0);
  if (!elfsh_get_hashtable(world.curjob->curfile, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot get .hash section",  0);

  snprintf(logbuf, BUFSIZ - 1,
	   " [HASH SYMBOL TABLE]\n [Object %s]\n\n",
	   world.curjob->curfile->name);
  
  revm_output(logbuf);
  revm_endline();

  sect = elfsh_get_hashtable_by_range(world.curjob->curfile, range, &nbr);

  while (sect != NULL)
    {
      revm_hashprint(sect, sym, nbr);
      sect = elfsh_get_hashtable_by_range(world.curjob->curfile, ++range, &nbr);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
