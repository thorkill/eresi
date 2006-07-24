/*
** libio.h for libio
** 
** Started on  Sun Jan 22 19:04:04 2006 mxatone
*/
#ifndef __LIBIO_H__
 #define __LIBIO_H__

#include "elfsh.h"

#define	VM_OUTPUT(str)						\
do								\
{								\
  int er;							\
  er = vm_output(str);						\
  if (er < 0)							\
  {								\
     vm_output("\n");						\
     ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, er);	\
  }								\
}								\
while (0)

#endif
