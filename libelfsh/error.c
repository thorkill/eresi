/*
** error.c for libelfsh
** 
** Started on  Thu Nov 08 02:08:28 2001 mm
** Last update Wed Jun 08 10:01:42 2005 mm
*/
#include "libelfsh.h"

#define		ELFSH_MAX_PATTERN	80			/* Maximum patterns depth */


static char	mem[2][ELFSH_MAX_PATTERN][80] = {{{0,0,0}}};	/* Patterns memory */
static u_int	hit[2][ELFSH_MAX_PATTERN]     = {{0,0}};	/* Patterns hit counters */
static u_int	sel;						/* Selection of memory */

/* The last error message */
char		*elfsh_error_str = NULL;

/* Last function depth */
int		elfsh_depth = 0;

/* Last function direction */
static char	elfsh_direction = '+';



/* Reset profiler memory */
void		elfsh_profile_reset(u_int lsel)
{
  u_int		idx;
  
  for (idx = 0; idx < ELFSH_MAX_PATTERN; idx++)
    {
      mem[lsel][idx][0] = 0x00;
      hit[lsel][idx] = 0;
    }
}



/* Generic routine for profiler output */
int		elfsh_profile_print(char *file, char *func, u_int line, char *msg)
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
  
  for (idx = 0; idx < ELFSH_MAX_PATTERN; idx++)
    if (!strcmp(buff, mem[sel][idx]))
    { 
      hit[sel][idx]++;
      flag = 1;
    }
  if (flag)
    return (1);

  fill = alloca(elfsh_depth + 1);
  memset(fill, ' ', elfsh_depth);
  fill[elfsh_depth] = 0x00;

  /* Print memory when pattern matched */
  for (idx = ELFSH_MAX_PATTERN - 1; idx >= 0; idx--)
    if (hit[sel][idx])
      {
	dbgworld.profile("\n");

	/*
	for (idx2 = idx; idx2 >= 0; idx2--)
	  {
	    snprintf(buf, BUFSIZ, " %s [%c] --[ (%u hit%s) %-50s \n", 
		     fill,
		     (sel ? 'P' : 'W'), 
		     hit[sel][idx], 
		     (hit[sel][idx] > 1 ? "s" : ""), 
		     mem[sel][idx2]);
	    dbgworld.profile(buf);			
	  }	

	snprintf(mesg, sizeof(mesg), " %s [%c] --- Last %u %s %u time(s) ---\n\n", 
		 fill,
		 (sel ? 'P' : 'W'), idx + 1, 
		 (sel ? "function(s) called/returned again" : "warning(s) repeated"),  
		 hit[sel][idx]);

	dbgworld.profile(mesg);
	*/

	for (idx = 0; idx < ELFSH_MAX_PATTERN; idx++)
	  hit[sel][idx] = 0;
	break;
      }
  
  /* shifting of memories */ 
  for (idx = ELFSH_MAX_PATTERN - 1; idx > 0; idx--)
    strncpy(mem[sel][idx], mem[sel][idx - 1], 80);
  strncpy(mem[sel][0], buff, 80);

  return (0);
}


/* Write the last error information */
void		elfsh_profile_err(char *file, char *func, u_int line, char *msg)
{
  char		buff[80];
  char		buf[BUFSIZ];
  char		*fill;

  if (dbgworld.proflevel >= ELFSH_ERRPROF)
    {
      
      /* Stock a pattern without printing */
      if (elfsh_profile_print(file, func, line, msg))
	return;

      fill = (elfsh_depth - 6 > 0 ? alloca(elfsh_depth + 1) : "");
      if (elfsh_depth - 6 > 0)
	{
	  memset(fill, ' ', elfsh_depth);
	  fill[elfsh_depth] = 0x00;
	}

      if (dbgworld.endline != NULL)
	{
	  snprintf(buff, sizeof(buff), " <%s@%s:%s>", 
		   dbgworld.colorstr(func), dbgworld.colorstr(file), 
		   dbgworld.colornumber("%u", line));
	  snprintf(buf, BUFSIZ, " %s %s %-50s %s \n", 
		   dbgworld.colorwarn("[W]"), fill, buff, dbgworld.colorwarn(msg)); 
	}
      else
	{
	  snprintf(buff, sizeof(buff), " <%s@%s:%u>", 
		   func, file, line);
	  snprintf(buf, BUFSIZ, " [W] %s %-50s %s \n", 
		   fill, buff, msg);
	}

      if (dbgworld.profile_err != NULL)		
	dbgworld.profile_err(buf);			
      else					
	fprintf(stderr, "[libelfsh:profile_err] No profiling function specified.\n");		
      
      if (dbgworld.endline != NULL)
	dbgworld.endline();

      elfsh_profile_reset(0);
    }
}


/* Write the last profiling information */
void		elfsh_profile_out(char *file, char *func, u_int line)
{
  char		buff[160];
  char		*space;
  char		b_dir[2];

  if (dbgworld.proflevel >= ELFSH_OUTPROF)
    {

      /* Stock a pattern, without printing */
      if (elfsh_profile_print(file, func, line, NULL))
	return;

      if (elfsh_depth > 80)
	elfsh_depth = 1;
      space = alloca(elfsh_depth + 1);
      memset(space, 0x00, elfsh_depth);
      memset(space, ' ', elfsh_depth);
      space[elfsh_depth] = 0x00;

      if (dbgworld.endline != NULL)
	{
	  b_dir[0] = elfsh_direction;
	  b_dir[1] = '\0';

	  snprintf(buff, sizeof(buff), "%s %s %s <%s@%s:%s> \n", 
		   space, dbgworld.colornumber("%u", elfsh_depth), 
		   dbgworld.colorfieldstr(b_dir), 
		   dbgworld.colorstr(func), dbgworld.colorstr(file),
		   dbgworld.colornumber("%u", line));
	}
      else
	{
	  snprintf(buff, sizeof(buff), "%s %u %c <%s@%s:%u> \n", 
		   space, elfsh_depth, elfsh_direction, 
		   func, file,line);
	  
	}
      dbgworld.profile(buff);	
      
      if (dbgworld.endline != NULL)
	dbgworld.endline();
    }
}


/* Set the current function depth and direction */
void		elfsh_incdepth()
{
  elfsh_depth++;
}

void		elfsh_updir()
{
  elfsh_direction = '+';
}

void		elfsh_decdepth()
{
  if (elfsh_depth > 0)
    elfsh_depth--;
  elfsh_direction = '-';
}


/* Display last error message */
void		elfsh_error()
{
  char		buf[BUFSIZ];

  if (elfsh_error_str)
    {
      snprintf(buf, BUFSIZ, " [E] %s\n\n", elfsh_error_str);
      dbgworld.profile_err(buf);
    }  
  elfsh_error_str = NULL;
}
