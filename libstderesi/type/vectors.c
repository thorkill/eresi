/**
 * @file vectors.c
 * @ingroup libstderesi
 * Started on  Sat Jan 06 06:43:11 2007 jfv
 *
 * $Id: vectors.c,v 1.1 2007-11-29 14:01:56 may Exp $
 *
 */
#include "libstderesi.h"

/** 
 * Get numbers of vector dimensions requested for the display 
 * @param str
 */
int		revm_vectors_getdimnbr(char *str)
{
  int		nbr;
  
  for (nbr = 0; str && *str; nbr++)
    {
      str = strchr(str, ':');
      if (!str)
	return (nbr);
      else
	str++;
    }
  return (nbr);
}

/** 
 * Get numbers of vector dimensions requested for the display 
 * @param str
 * @param dims
 */
int		revm_vectors_getdims(char *str, unsigned int *dims)
{
  revmobj_t	*v;
  int		nbr;
  char		*str2;

  /* FIXME : add elfsh vectors index macros to completion */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (nbr = 0; str && *str; nbr++)
    {
      str       = strchr(str, ':');
      if (!str)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      *str      = 0x00;
      str2      = strchr(++str, ':');
      if (str2)
	*str2 = 0x00;
      v = revm_lookup_immed(str);
      if (v->otype->type != ASPECT_TYPE_LONG && v->otype->type != ASPECT_TYPE_INT)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Invalid vector indexes", -1);
      dims[nbr] = (v->immed ? v->immed_val.ent : v->get_obj(v->parent));
      if (str2)
	*str2 = ':';
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Check vectors dimensions 
 * @param vect
 * @param dims
 * @param dimnbr
 * @return
 */
int		revm_vector_bad_dims(vector_t *vect, unsigned int *dims,
				   unsigned int dimnbr)
{
  int		idx;

  if (vect->arraysz != dimnbr)
    return (1);
  for (idx = 0; idx < dimnbr; idx++)
    if (dims[idx] >= vect->arraydims[idx])
      return (1);
  return (0);
}



/** 
 * Modify a value inside a vector 
 * @return
 */
static int	revm_vector_modify()
{
  vector_t	*cur;
  int		dimnbr;
  unsigned int	*dims;
  eresi_Addr	value;
  char		logbuf[BUFSIZ];
  int		len;
  int		index;
  revmobj_t	*obj;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup value to write */
  obj = revm_lookup_immed(world.curjob->curcmd->param[1]);
  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid update value for vector", -1);
  value = (obj->immed ? obj->immed_val.ent : 
	   (eresi_Addr) obj->get_obj(obj->parent));
  
  /* Lookup vector indexes */
  if (strchr(world.curjob->curcmd->param[0], ':'))
    {
      dimnbr = revm_vectors_getdimnbr(world.curjob->curcmd->param[0]);
      if (dimnbr < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to get vector indexes values", -1);
      dims = alloca(dimnbr * sizeof(unsigned int));
      revm_vectors_getdims(world.curjob->curcmd->param[0], dims);
      cur = aspect_vector_get(world.curjob->curcmd->param[0]);
      if (!cur)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unknown requested vector", -1);
      if (revm_vector_bad_dims(cur, dims, dimnbr))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Requested vector with bad dimensions", -1);
      aspect_vectors_insert(cur, dims, value);
      len = snprintf(logbuf, sizeof(logbuf), "\n [*] Modified : Vector[%s]", 
		     world.curjob->curcmd->param[0]);
      for (index = 0; index < dimnbr; index++)
	len += snprintf(logbuf + len, sizeof(logbuf) - len, "[%u]", dims[index]);
      snprintf(logbuf + len, sizeof(logbuf) - len, " = " XFMT "\n", value); 
      revm_output(logbuf);
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "You need to supply the vector indexes to be modified", -1);
}



/** 
 * Print a vector entry with its resolve 
 * @param vaddr
 * @param def
 */
static void	revm_vector_entry_display(eresi_Addr vaddr, eresi_Addr def)
{
  Dl_info	info;
  char		*bsep;
  char		*esep;
  char		*print;
  char		log[BUFSIZ];

  if (!dladdr((void *) ((unsigned long *) vaddr), &info))
    dlerror();
  esep = bsep = "";
  if (info.dli_sname && vaddr != def)
    {
      esep  = ">";
      bsep  = "<";
      print = (char *) info.dli_sname;
    }
  else if (vaddr == def)
    {
      esep  = ")";
      bsep  = "(";
      print = "default handler";
    }
  else
    print = "Unknown function";
  snprintf(log, sizeof(log), " = " XFMT " %s%s%s\n", vaddr, bsep, print, esep);
  revm_output(log);
}



/** 
 * Display the vector recursively 
 * @param tab
 * @param dims
 * @param index
 * @param depth
 * @patram dimsz
 * @param name
 * @param def
 */
static void	revm_vector_recdisplay(unsigned long *tab,  unsigned int *dims, 
				     unsigned int *index, unsigned int depth, 
				     unsigned int dimsz, char *name, unsigned long def)
{
  unsigned int	idx;
  unsigned int	idx2;  
  char		*log;
  int		len;
  int		sz;

  /* Check if we reached the last dimension */
  if (depth == dimsz)
    for (idx = 0; idx < dims[depth - 1]; idx++)
      {
	sz  = strlen(name) + 10 * depth;
	log = alloca(sz);
	len = snprintf(log, sz, "   %s", name);
	index[depth - 1] = idx;
	for (idx2 = 0; idx2 < depth; idx2++)
	  len += snprintf(log + len, sz - len, "[%02u]", index[idx2]);
	revm_output(log);
	revm_vector_entry_display((eresi_Addr) tab[idx], (eresi_Addr) def);
      }

  /* Otherwise recurse more */
  else
    for (idx = 0; idx < dims[depth - 1]; idx++)
      {
	index[depth - 1] = idx;
	revm_vector_recdisplay((unsigned long *) tab[idx], dims, index,
			     depth + 1, dimsz, name, def);
      }
  return;  
}


/** 
 * Display vector 
 * @param cur
 * @param name
 *
 */
static void	 revm_vector_print(vector_t *cur, char *name)
{
  unsigned int	*idx;
  char		logbuf[BUFSIZ];

  idx = alloca(cur->arraysz * sizeof(unsigned int));
  bzero(idx, cur->arraysz * sizeof(unsigned int));
  snprintf(logbuf, sizeof(logbuf), "  .:: Printing vector %s \n\n", name);
  revm_output(logbuf);
  revm_vector_recdisplay(cur->hook, cur->arraydims, idx, 
		       1, cur->arraysz, name, 
		       (unsigned long) cur->default_func);
  snprintf(logbuf, sizeof(logbuf), "\n .:: Vector %s printed \n\n", name);
  revm_output(logbuf);
}



/** 
 * Display the content of a vector 
 */
static void	revm_vectors_display()
{
  char		**keys;
  int		keynbr;
  int		index;
  int		index2;
  vector_t	*cur;
  char		buf[BUFSIZ];
  int		len;

  revm_output("  .:: Registered vectors \n\n");
  keys = hash_get_keys(vector_hash, &keynbr);
  for (len = index = 0; index < keynbr; index++)
    {
      cur = hash_get(vector_hash, keys[index]);
      len = snprintf(buf, sizeof(buf), "  + %-20s\t ElemType: %-15s Dimensions: ", 
		     keys[index], revm_ascii_vtype(cur));
      for (index2 = 0; index2 < cur->arraysz; index2++)
	len += snprintf(buf + len, sizeof(buf) - len, 
			"[%02u]", cur->arraydims[index2]);
      len += snprintf(buf + len, sizeof(buf) - len, "  Discriminated by: ");
      for (index2 = 0; index2 < cur->arraysz; index2++)
	len += snprintf(buf + len, sizeof(buf) - len, "%s ", cur->strdims[index2]);
      snprintf(buf + len, sizeof(buf) - len, "\n");
      revm_output(buf);
    }
  revm_output("\n Type vector vectname for specific vector details.\n\n");
}



/** 
 * Display vector content 
 */
static int	 revm_vector_display()
{
  vector_t	*cur;
  int		dimnbr;
  unsigned int	*dims;
  eresi_Addr	value;
  char		logbuf[BUFSIZ];
  int		len;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (strchr(world.curjob->curcmd->param[0], ':'))
    {
      dimnbr = revm_vectors_getdimnbr(world.curjob->curcmd->param[0]);
      if (dimnbr < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to get vector indexes values", -1);
      dims = alloca(dimnbr * sizeof(unsigned int));
      revm_vectors_getdims(world.curjob->curcmd->param[0], dims);
      cur = aspect_vector_get(world.curjob->curcmd->param[0]);
      if (!cur)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unknown requested vector", -1);
      if (revm_vector_bad_dims(cur, dims, dimnbr))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Requested vector with bad dimensions", -1);
      value = (eresi_Addr) aspect_vectors_select(cur, dims);
      len = snprintf(logbuf, sizeof(logbuf), "\n [*] Vector[%s]", 
		     world.curjob->curcmd->param[0]);
      for (index = 0; index < dimnbr; index++)
	len += snprintf(logbuf + len, sizeof(logbuf) - len, "[%u]", dims[index]);
      revm_output(logbuf);
      revm_vector_entry_display((eresi_Addr) value, (eresi_Addr) cur->default_func);
      revm_output("\n");
    }
  else
    {
      cur = aspect_vector_get(world.curjob->curcmd->param[0]);
      if (!cur)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Vector does not exist", -1);
      revm_vector_print(cur, world.curjob->curcmd->param[0]);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Simple internal cat builtin 
 */
int		cmd_vectors()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (world.curjob->curcmd->argc)
    {
      /* Just print vectors list */
    case 0:
      revm_vectors_display();
      break;
      
      /* Print a determined vector with determined (or not) indexes */
    case 1:
      if (revm_vector_display() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot print invalid vector element(s)", -1);
      break;

      /* Write vector entry */
    case 2:
      if (revm_vector_modify() < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot modify vector element", -1);
      break;

      /* Unknown mode */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid vectors syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
