/**
** @file error.c
** @ingroup libaspect
** 
** Started on  Thu Nov 08 02:08:28 2001 mm
** Last update Wed Jun 08 10:01:42 2005 mm
**
** $Id: profiler.c,v 1.10 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libaspect.h"

/* Patterns memory */
static char	mem[2][PROFILER_MAX_PATTERN][80] = {{{0,0,0}}};	

/* Patterns hit counters */
static unsigned int	hit[2][PROFILER_MAX_PATTERN]     = {{0,0}};	

/* Selection of memory */
static unsigned int	sel;						

/* The last error message */
char		*profiler_error_str = NULL;

/* Last function depth */
int		profiler_depth = 0;

/* Last function direction */
static char	profiler_direction = '+';

/* Allocation cache */
static profallocentry_t		allocentries[PROFILER_MAX_ALLOC];

/* Current allocation depth */
static unsigned int	profiler_adepth = 0;



/***************** Now the allocator profiler *********************/



/**
 * @brief Find an entry in the allocation profiler cache 
 * @param direction Either PROFILER_WARNING_LAST, PROFILER_WARNING_FIRST, or PROFILER_WARNING_UNKNOW
 * @param addr Allocated address to be found in profiling cache
 * @param optype Type of operation to be found
 * @return Allocation profiler entry corresponding to input criterions
 */
profallocentry_t	*profiler_alloc_find(unsigned char direction,
					     unsigned long addr, 
					     unsigned char optype)
{
  unsigned int			index;
  
  switch (direction)
    {
    case PROFILER_WARNING_LAST:
      for (index = profiler_adepth - 1; index; index--)
	if (allocentries[index].addr   == addr &&
	    allocentries[index].optype == optype)
	  return (allocentries + index);
      return (NULL);
    case PROFILER_WARNING_FIRST:
      for (index = 1; index < profiler_adepth; index++)
	if (allocentries[index].addr   == addr &&
	    allocentries[index].optype == optype)
	  return (allocentries + index);
      return (NULL);
    case PROFILER_WARNING_UNKNOW:
    default:
      if (aspectworld.profile)
	aspectworld.profile(" [E] Unknown alloc optype requested to the "
			    "allocator profiler\n");
      return (NULL);
    }
}


/** 
 * @brief Print the warning string 
 * @param str Header string to be printed
 * @param fatal 1 if error is fatal (exit afterwards)
 * @param idx Index corresponding to allocation entry to print
 */
void		profiler_alloc_warnprint(char *str, int fatal, int idx)
{
  char	        buf[BUFSIZ];
  
  snprintf(buf, sizeof(buf),
	   "%s\n (%s-[%s of addr: 0x%08lX at %s@%s:%u) \n\n", str,
	   (allocentries[idx].alloctype ==
	    PROFILER_ALLOC_LEGIT ? "LEGIT " :
	    PROFILER_ALLOC_PROXY ? "PROXY " : "UNKNOW "),
	   (allocentries[idx].optype ==
	    PROFILER_OP_ALLOC   ? "ALLOC  " :
	    PROFILER_OP_REALLOC ? "REALLOC" : 
	    PROFILER_OP_FREE    ? "FREE   " : "UNKNOW "),
	   allocentries[idx].addr,
	   allocentries[idx].filename,
	   allocentries[idx].funcname,
	   allocentries[idx].linenbr);
  if (aspectworld.profile)
    aspectworld.profile(buf);	
  if (fatal)
    exit(-1);
}




/** 
 * @brief Warn if anything bad is happening 
 * @param warntype Either PROFILER_WARNING_LAST, PROFILER_WARNING_FIRST or PROFILER_WARNING_UNKNOW
 */
void			profiler_alloc_warning(unsigned char warntype)
{
  profallocentry_t	*ent;
  profallocentry_t	*ent2;
  profallocentry_t	*ent3;

  /* Do not emit warning if not asked to */
  if (!(aspectworld.proflevel & PROFILE_ALLOC))
    return;

  switch (warntype)
    {      

      /* There are many checks for the last entry */
    case PROFILER_WARNING_LAST:
      ent2 = profiler_alloc_find(PROFILER_WARNING_LAST,
				 allocentries[profiler_adepth].addr,
				 PROFILER_OP_FREE);
      ent  = profiler_alloc_find(PROFILER_WARNING_LAST,
				 allocentries[profiler_adepth].addr, 
				 PROFILER_OP_ALLOC);
      ent3 = profiler_alloc_find(PROFILER_WARNING_LAST,
				 allocentries[profiler_adepth].addr, 
				 PROFILER_OP_REALLOC);
      
      /* We do emit a warning if we encountered a double free */
      if (allocentries[profiler_adepth].optype == PROFILER_OP_FREE &&
	  ent2 && ent2 > ent)
	profiler_alloc_warnprint(" [E] A pointer was freed"
				 " but it is already free ", 
				 1, profiler_adepth);
      
      /* We emit a warning when a free happens in the wrong allocator */
      if ((allocentries[profiler_adepth].optype == PROFILER_OP_FREE ||
	   allocentries[profiler_adepth].optype == PROFILER_OP_REALLOC)
	  && ent 
	  && ent->alloctype != allocentries[profiler_adepth].alloctype)
	profiler_alloc_warnprint(" [E] A pointer was freed"
				 " but it is was belonging "
				 " to another allocator ", 
				 1, profiler_adepth);

      /* We emit a warning when we free without remembering the alloc */
      if (allocentries[profiler_adepth].optype == PROFILER_OP_REALLOC
	  && !ent)
	profiler_alloc_warnprint(" [!] A pointer was reallocated but its"
				 " allocation was not found in history",
				 0, profiler_adepth);
      
      /* We emit a warning when we realloc without remembering alloc */
      if (allocentries[profiler_adepth].optype == PROFILER_OP_REALLOC
	  && !ent && !ent2)
	profiler_alloc_warnprint(" [!] A pointer was reallocated but its"
				 " allocation was not found in history",
				 0, profiler_adepth);

      /* We emit a warning when we realloc something freed */
      if (allocentries[profiler_adepth].optype == PROFILER_OP_REALLOC
	  && ent2 && (ent2 > ent))
	profiler_alloc_warnprint(" [E] A pointer was reallocated but it"
				 " was in a freed state",
				 1, profiler_adepth);

      /* We do emit a warning if the last entry is an allocation
	 and this pointer was already allocated after beeing freed */
      if (allocentries[profiler_adepth].optype == PROFILER_OP_ALLOC &&
	  ent && ((ent > ent2) || (ent > ent3)))
	profiler_alloc_warnprint(" [E] A pointer was reallocated"
				 " without beeing freed ", 
				 1, profiler_adepth);
      
      break;
      
      /* We do emit a warning if the first entry is a malloc
	 and it has not been freed before it was removed from
	 the cache, which mean its a potential memory leak */
    case PROFILER_WARNING_FIRST:
      if (allocentries[0].optype == PROFILER_OP_FREE)
	return;
      ent = profiler_alloc_find(PROFILER_WARNING_FIRST,
				allocentries[0].addr, 
				PROFILER_OP_FREE);
      if (!ent)
	profiler_alloc_warnprint(" [!] An allocation was removed"
				 " from the history without beeing free", 
				 0, 0);
      break;

      /* Just an error detection message */
    default:
    case PROFILER_WARNING_UNKNOW: 
      if (aspectworld.profile)
	aspectworld.profile(" [E] Unknown warning type requested to the "
			    "allocator profiler\n");
      exit(-1);
      break;
    }
}



/** 
 * @brief Shift the allocation history 
 */
void			profiler_alloc_shift()
{
  int			index;

  for (index = 1; index < PROFILER_MAX_ALLOC; index++)
    allocentries[index - 1] = allocentries[index];
  profiler_adepth--;
}



/** 
 * @brief Add an entry in the allocation cache 
 * @param file Generally called with __FILE__ (gcc), contains the file name where allocation is done
 * @param func Generally called with __FUNCTION__ (gcc), contains the function name where allocation is done
 * @param line Generally called with __LINE__ (gcc), contains the line number in related file
 * @param addr Newly allocated address
 * @param atype Type of allocation
 * @param otype Type of allocation operation
 */
static void		profiler_alloc_add(char *file, char *func, 
					   unsigned int line, unsigned long addr, 
					   unsigned char atype, unsigned char otype)
{
  allocentries[profiler_adepth].alloctype = atype;
  allocentries[profiler_adepth].optype    = otype;
  allocentries[profiler_adepth].filename  = file;
  allocentries[profiler_adepth].funcname  = func;
  allocentries[profiler_adepth].linenbr   = line;  
  allocentries[profiler_adepth].addr      = addr;
  profiler_adepth++;
}


/** 
 * @brief Update allocation cache with a new entry 
 * @param file Generally called with __FILE__ (gcc), contains the file name where allocation is done
 * @param func Generally called with __FUNCTION__ (gcc), contains the function name where allocation is done
 * @param line Generally called with __LINE__ (gcc), contains the line number in related file
 * @param addr Newly allocated address
 * @param atype Type of allocation
 * @param otype Type of allocation operation
 * @return 0 if we are filling the last cache entry, 0 if not
 */
int			profiler_alloc_update(char *file, char *func, 
					      unsigned int line, unsigned long addr, 
					      unsigned char atype, unsigned char otype)
{
  //void			*profunc;
  // XXX: remove this ?
  //profunc             = aspectworld.profile;  
  //aspectworld.profile = (void *) printf;

  /* Just happens an entry at the end. Check the last entry. */
  if (profiler_adepth + 1 != PROFILER_MAX_ALLOC)
    {
      profiler_alloc_add(file, func, line, addr, atype, otype);
      profiler_alloc_warning(PROFILER_WARNING_LAST);
      //aspectworld.profile = profunc;
      return (0);
    }

  /* In that case we have to shift all entries. Check the removed entry */
  profiler_alloc_warning(PROFILER_WARNING_FIRST);
  profiler_alloc_shift();
  profiler_alloc_add(file, func, line, addr, atype, otype);  
  profiler_alloc_warning(PROFILER_WARNING_LAST);

  //aspectworld.profile = profunc;
  return (1);
}




/***************** Now the profiler for function calls ***************/






/** 
 * @brief Reset profiler memory
 * @param lsel Identifiant for profiler cache bank to be used
 */
void		profiler_reset(unsigned int lsel)
{
  unsigned int		idx;
  
  for (idx = 0; idx < PROFILER_MAX_PATTERN; idx++)
    {
      mem[lsel][idx][0] = 0x00;
      hit[lsel][idx] = 0;
    }
}


/** 
 * @brief Generic routine for profiler output 
 * @param file Generally called with __FILE__ (gcc), contains the file name where allocation is done
 * @param func Generally called with __FUNCTION__ (gcc), contains the function name where allocation is done
 * @param line Generally called with __LINE__ (gcc), contains the line number in related file
 * @param msg Profiling message suffix to be printed
 * @return 1 if message is already present in bank, 0 if not
 */
int		profiler_print(char *file, char *func, 
			       unsigned int line, char *msg)
{
  char		buff[80];
  char		buf[BUFSIZ];
  //  char		mesg[100];
  volatile int  idx;
  //  volatile int  idx2;
  char		flag;
  char		*fill;

  sel = (msg ? 0 : 1);

  /* Compute location string */
  snprintf(buf, sizeof(buf), "<%s@%s:%u>", func, file, line);
  snprintf(buff, sizeof(buff), "%-50s %s", buf, (msg ? msg : ""));
  flag = 0;
  
  for (idx = 0; idx < PROFILER_MAX_PATTERN; idx++)
    if (!strcmp(buff, mem[sel][idx]))
    { 
      hit[sel][idx]++;
      flag = 1;
    }
  if (flag)
    return (1);

  fill = alloca(profiler_depth + 1);
  memset(fill, ' ', profiler_depth);
  fill[profiler_depth] = 0x00;

  /* Print memory when pattern matched */
  for (idx = PROFILER_MAX_PATTERN - 1; idx >= 0; idx--)
    if (hit[sel][idx])
      {
	aspectworld.profile("\n");

	/*
	for (idx2 = idx; idx2 >= 0; idx2--)
	{
	snprintf(buf, BUFSIZ, " %s [%c] --[ (%u hit%s) %-50s \n", 
	fill,
	(sel ? 'P' : 'W'), 
	hit[sel][idx], 
	(hit[sel][idx] > 1 ? "s" : ""), 
	mem[sel][idx2]);
	aspectworld.profile(buf);			
	}	
	
	snprintf(mesg, sizeof(mesg), " %s [%c] --- Last %u %s %u time(s) ---\n\n", 
	fill,
	(sel ? 'P' : 'W'), idx + 1, 
	(sel ? "function(s) called/returned again" : "warning(s) repeated"),  
	hit[sel][idx]);
	
	aspectworld.profile(mesg);
	*/
	
	for (idx = 0; idx < PROFILER_MAX_PATTERN; idx++)
	  hit[sel][idx] = 0;
	break;
      }
  
  /* shifting of memories */ 
  for (idx = PROFILER_MAX_PATTERN - 1; idx > 0; idx--)
    strncpy(mem[sel][idx], mem[sel][idx - 1], 80);
  strncpy(mem[sel][0], buff, 80);

  return (0);
}


/** 
 * @brief Generic routine for profiler error output
 * @param file Generally called with __FILE__ (gcc), contains the file name where allocation is done
 * @param func Generally called with __FUNCTION__ (gcc), contains the function name where allocation is done
 * @param line Generally called with __LINE__ (gcc), contains the line number in related file
 * @param msg Profiling message suffix to be printed
 */
void		profiler_err(char *file, char *func, 
			     unsigned int line, char *msg)
{
  char		buff[80];
  char		buf[BUFSIZ];
  char		*fill;
  
  if (!(aspectworld.proflevel & PROFILE_WARN))
    return;
      
  /* Stock a pattern without printing */
  if (profiler_print(file, func, line, msg))
    return;
  
  fill = (profiler_depth - 6 > 0 ? alloca(profiler_depth + 1) : "");
  if (profiler_depth - 6 > 0)
    {
      memset(fill, ' ', profiler_depth);
      fill[profiler_depth] = 0x00;
    }
  
  if (aspectworld.endline != NULL)
    {
      snprintf(buff, sizeof(buff), " <%s@%s:%s>", 
	       aspectworld.colorfunction(func), 
	       aspectworld.colorfilename(file), 
	       aspectworld.colornumber("%u", line));
      snprintf(buf, BUFSIZ, " %s %s %-70s %s \n", 
	       aspectworld.colorwarn("[W]"), 
	       fill, buff, aspectworld.colorwarn(msg)); 
    }
  else
    {
      snprintf(buff, sizeof(buff), " <%s@%s:%u>", 
	       func, file, line);
      snprintf(buf, BUFSIZ, " [W] %s %-70s %s \n", 
	       fill, buff, msg);
    }
  
  if (aspectworld.profile_err != NULL)		
    aspectworld.profile_err(buf);			
  else					
    fprintf(stderr, "No profiling function specified.\n");
  if (aspectworld.endline != NULL)
    aspectworld.endline();
  profiler_reset(0);
}


/** 
 * @brief Write the last profiling information 
 * @param file Generally called with __FILE__ (gcc), contains the file name where allocation is done
 * @param func Generally called with __FUNCTION__ (gcc), contains the function name where allocation is done
 * @param line Generally called with __LINE__ (gcc), contains the line number in related file
 */
void		profiler_out(char *file, char *func, unsigned int line)
{
  char		buff[160];
  char		*space;
  char		b_dir[2];

  if (!(aspectworld.proflevel & PROFILE_FUNCS))
    return;

  /* Stock a pattern, without printing */
  if (profiler_print(file, func, line, NULL))
    return;
  
  if (profiler_depth > 80)
    profiler_depth = 1;
  space = alloca(profiler_depth + 1);
  memset(space, 0x00, profiler_depth);
  memset(space, ' ', profiler_depth);
  space[profiler_depth] = 0x00;

  if (aspectworld.endline != NULL)
    {
      b_dir[0] = profiler_direction;
      b_dir[1] = '\0';
      snprintf(buff, sizeof(buff), "%s %s %s <%s@%s:%s>\n", 
	       space, 
	       aspectworld.colornumber("%u", profiler_depth), 
	       aspectworld.colorfieldstr(b_dir), 
	       aspectworld.colorfunction(func), 
	       aspectworld.colorfilename(file),
	       aspectworld.colornumber("%u", line));
    }
  else
    {
      snprintf(buff, sizeof(buff), "%s %u %c <%s@%s:%u>\n", 
	       space, profiler_depth, profiler_direction, 
	       func, file,line);
    }
  if (aspectworld.profile)
    aspectworld.profile(buff);
  
  if (aspectworld.endline != NULL)
    aspectworld.endline();
}

/** 
 * @brief Profiler is started ? 
 */
unsigned char		profiler_started()
{
  return (aspectworld.profstarted);
}


/** 
 * @brief Set the current function depth and direction 
 */
void		profiler_incdepth()
{
  profiler_depth++;
}

/** 
 * @brief Set current function direction to be UP (function entering)
 */
void		profiler_updir()
{
  profiler_direction = '+';
}

/** 
 * @brief Set current function direction to be DOWN (function exiting)
 */
void		profiler_decdepth()
{
  if (profiler_depth > 0)
    profiler_depth--;
  profiler_direction = '-';
}

/** 
 * @brief Remove lastly generated error (nulify error string)
 */
void		profiler_error_reset()
{
  profiler_error_str = NULL;
}

/**
 * @brief Display last error message 
 */
void		profiler_error()
{
  char		buf[BUFSIZ];

  if (profiler_error_str)
    {
      snprintf(buf, BUFSIZ, " [E] %s\n\n", profiler_error_str);
      if (aspectworld.profile_err)
	aspectworld.profile_err(buf);
      else
	printf("[WARNING] : profile_err() is NULL . Reverting to prinf.\n%s\n", buf);
    }  
 
  profiler_error_reset(); 
}
