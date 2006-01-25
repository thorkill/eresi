/*
** version.c for elfsh
** 
** Hash features
** 
** Started on  Sun Oct 07 10:20:18 2005 mxatone
*/
#include "elfsh.h"

int		vm_hashunk(int index)
{
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ -1,
	   " %s %s\n", 
	   vm_colornumber("[%03u]", index),
	   vm_colortypestr("STN_UNDEF"));
  vm_output(logbuf);
  vm_endline();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		vm_hashbucketprint(int type, int index, int symid, 
				   char *name, int rhash, int hash, int check)
{
  char		logbuf[BUFSIZ];
  char		ipattern[16];
  int		quiet;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(ipattern, 15, 
	   (type == 0 ? "[%%03u]" : "~>%%03u"));

  quiet = world.state.vm_quiet;

  snprintf(logbuf, BUFSIZ - 1,
	   " %s %s%s %s%s %s%s %s%s %s%s\n", 
	   vm_colornumber(ipattern, index), 
	   vm_colorfieldstr("sym:"),
	   vm_colornumber("%03u", symid), 
	   vm_colorfieldstr("name:"),
	   vm_colorstr_fmt((!quiet ? "%-40s" : "%-15s"), name), 
	   vm_colorfieldstr("hash:"),
	   vm_colornumber("%08x", rhash),
	   vm_colorfieldstr("hashm:"),
	   vm_colornumber("%02u", hash),
	   vm_colorfieldstr("check:"),
	   (check ? "ok" : "bad"));

  vm_output(logbuf);
  vm_endline();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		vm_hashchainprint(int index, int symid, char *name, 
				  int rhash, int hash)
{
  char		logbuf[BUFSIZ];
  int		quiet;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  quiet = world.state.vm_quiet;

  snprintf(logbuf, BUFSIZ - 1,
	   " %s %s%s %s%s %s%s %s%s\n", 
	   vm_colornumber("[%03u]", index), 
	   vm_colorfieldstr("sym:"),
	   vm_colornumber("%03u", symid), 
	   vm_colorfieldstr("name:"),
	   vm_colorstr_fmt((!quiet ? "%-40s" : "%-15s"), name), 
	   vm_colorfieldstr("hash:"),
	   vm_colornumber("%08x", rhash),
	   vm_colorfieldstr("hashm:"),
	   vm_colornumber("%02u", hash));

  vm_output(logbuf);
  vm_endline();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 		cmd_hashx()
{
  elfshsect_t	*sect;
  elfsh_Sym	*sym;
  void		*data;
  int		hash, chash;
  int		rhash, rchash;
  int		nbucket;
  int		nchain;
  int		*chain;
  int		*bucket;
  int		index, cindex;
  int		symid;
  int		nbr;
  char		logbuf[BUFSIZ];
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_hashtable(world.curjob->current, NULL);
  if (data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      " Cannot get HASH",  0);

  sect = elfsh_get_section_by_type(world.curjob->current, SHT_HASH, 
				   NULL, NULL, &nbr, 0);
  if (sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get HASH by type", NULL);


  sym = elfsh_get_dynsymtab(world.curjob->current, NULL);
  if (sym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get DYNSYM",  0);

  nbucket = elfsh_get_hashnbucket(data);
  nchain  = elfsh_get_hashnchain(data);
  bucket  = elfsh_get_hashbucket(data);
  chain   = elfsh_get_hashchain(data);

  snprintf(logbuf, BUFSIZ - 1,
	   " [HASH SYMBOL TABLE]\n [Object %s]\n [Section %s]\n"
	   " [nbucket %u]\n [nchain %u]\n\n", 
	   world.curjob->current->name, sect->name,
	   nbucket, nchain);
  
  vm_output(logbuf);
  vm_endline();

  vm_output(" [bucket]\n");

  for (index = 0; index < nbucket; index++)
    {
      symid = bucket[index];
      name = elfsh_get_dynsymbol_name(world.curjob->current,
				      sym + symid);

      rhash = elfsh_get_symbol_hash(name);
      hash = rhash % nbucket;
      
      if (symid == STN_UNDEF)
	{
	  vm_hashunk(index);
	  continue;
	}

      vm_hashbucketprint(0, index, symid, name, rhash, hash, (index == hash));
      
      for (cindex = symid; cindex < nchain; cindex = symid)
	{
	  symid = chain[cindex];

	  if (symid == STN_UNDEF)
	    break;

	  name = elfsh_get_dynsymbol_name(world.curjob->current,
					  sym + symid);
	  rchash = elfsh_get_symbol_hash(name);
	  chash = rchash % nbucket;

	  vm_hashbucketprint(1, cindex, symid, name, rchash, chash, (hash == chash));
	}
    }

  vm_output("\n [chain]\n");
  
  for(index = 0; index < nchain; index++)
    {
      symid = chain[index];
      name = elfsh_get_dynsymbol_name(world.curjob->current,
				      sym + symid);

      rhash = elfsh_get_symbol_hash(name);
      hash = rhash % nbucket;

      if (symid == STN_UNDEF)
	{
	  vm_hashunk(index);
	  continue;
	}

	vm_hashchainprint(index, symid, name, rhash, hash);
    }

  vm_output("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
