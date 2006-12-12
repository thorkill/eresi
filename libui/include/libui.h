/*
** libui.h for libui
** 
** Started on  Sun Jan 22 19:04:04 2006 mxatone
*/

#ifndef __LIBUI_H__
 #define __LIBUI_H__

#include <elfsh.h>
#include "libui-color.h"

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

#define	VM_OUTPUT_Q(str)					\
do								\
{								\
  int er;							\
  er = vm_output(str);						\
  if (er == -1)							\
  {								\
     vm_output("\n");						\
     ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, er);	\
  }								\
}								\
while (0)

#endif
