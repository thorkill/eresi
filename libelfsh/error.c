/*
** error.c for libelfsh
** 
** Started on  Thu Nov 08 02:08:28 2001 mm
** Last update Wed Jun 08 10:01:42 2005 mm
*/
#include "libelfsh.h"


#define		ELFSH_MAX_PATTERN	5			/* Maximum patterns depth */

static char	mem[2][ELFSH_MAX_PATTERN][80] = {{{0,0,0}}};	/* Patterns memory */
static u_int	hit[2][ELFSH_MAX_PATTERN]     = {{0,0}};	/* Patterns hit counter */
static u_int	sel;						/* Selection of memory */

char	*elfsh_error = NULL;

/* Reset profiler memory */
void		elfsh_profile_reset(u_int lsel)
{
  u_int		idx;
  
  for (idx = 0; idx < ELFSH_MAX_PATTERN; idx++)
      mem[lsel][idx][0] = 0x00;
}


/* Generic routine for profiler output */
int		elfsh_profile_print(char *file, char *func, int line, char *msg)
{
  char		buff[80];
  char		buf[BUFSIZ];
  char		mesg[70];
  volatile int  idx;
  volatile int  idx2;
  char		flag;

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

  /* Print memory when pattern matched */
  for (idx = ELFSH_MAX_PATTERN - 1; idx >= 0; idx--)
    if (hit[sel][idx])
      {

       
	for (idx2 = idx; idx2 >= 0; idx2--)
	  {
	    snprintf(buf, BUFSIZ, " [%c] --[ %-50s \n", (sel ? 'P' : 'W'), mem[sel][idx2]);
	    dbgworld.profile(buf);			
	  }
	

	snprintf(mesg, sizeof(mesg), " [%c] --- Last %u %s %u time(s) ---\n", 
		 (sel ? 'P' : 'W'), idx + 1, 
		 (sel ? "function(s) recalled" : "warning(s) repeated"),  
		 hit[sel][idx]);

	dbgworld.profile(mesg);
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
void		elfsh_profile_err(char *file, char *func, u_int32_t line, char *msg)
{
  char		buff[80];
  char		buf[BUFSIZ];
  
  if (elfsh_is_prof_enable())
    {
	if (elfsh_profile_print(file, func, line, msg))
	  return;
	
	snprintf(buff, sizeof(buff), "<%s@%s:%u>", func, file, line);
	snprintf(buf, BUFSIZ, " [W]     %-50s %s \n", buff, msg); 
	if (dbgworld.profile != NULL)		
	  dbgworld.profile(buf);			
	else					
	  fprintf(stderr, "[libelfsh:profile_err] No profiling function specified.\n");		
	elfsh_profile_reset(0);
    }
}


/* Write the last profiling information */
void		elfsh_profile_out(char *file, char *func, u_int line)
{
  char		buff[80];
  
  if (elfsh_is_prof_enable())
    {
      if (elfsh_profile_print(file, func, line, NULL))
	return;
      snprintf(buff, sizeof(buff), " + <%s@%s:%u> \n", func, file, line);
      dbgworld.profile(buff);			
      //elfsh_profile_reset(1);
    }
}

