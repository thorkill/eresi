/*
** bt.c for elfsh
** 
** Started on  Wed Jul 27 04:08:52 2005 ym
** 
*/
#include "libelfsh.h"



/* XXX: arch dependant, some ifdef needed for multi-arch/OS support */
void		*elfsh_bt_get_frame()
{
  void *frame=0;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if !defined(IRIX)
  __asm__ volatile ("mov %%ebp, %0 \n"
		    : "=m" (frame));
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, frame);
}

void		*elfsh_bt(void *frame)
{
  void		*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = elfsh_bt_ia32(frame);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


void		*elfsh_bt_ia32(void *frame)
{
  /*
    void		*val;
    char		*name;
  */

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  if (!frame)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (NULL));

  /*
  val = (*((long *)frame));
  printf("%08lx \n", val, elfsh_reverse_metasym(file, val, NULL));
  val = (*((long *)frame + 1));
  printf("%08lx \n", val, elfsh_reverse_metasym(file, val, NULL));
  val = (*((long *)frame + 2));
  printf("%08lx \n", val, elfsh_reverse_metasym(file, val, NULL));
  val = (*((long *)frame + 3));
  printf("%08lx \n", val, elfsh_reverse_metasym(file, val, NULL));
  val = (*((long *)frame + 4));
  printf("%08lx \n", val, elfsh_reverse_metasym(file, val, NULL));

  printf("\nframe pointer : %x\n", ((long) frame));  
  printf("\nret addr : %x\n", (*((long *)frame + 1)));
  */

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ((void *) *(long*)frame));
}

