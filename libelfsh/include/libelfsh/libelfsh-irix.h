/*
** libelfsh-bsd.h
**
** Various undefined macros on IRIX
**
** Last update Mon Aug 23 05:05:27 2005 ym
*/

#ifndef STT_TLS
  #define STT_TLS  6
  #undef  STT_NUM
  #define STT_NUM  7
#endif

