/*
** @file libui-color.h
** @ingroup librevm_hdr
** Started on  Sun Nov 26 22:13:54 2006 mxatone
**
** $Id: librevm-color.h,v 1.7 2008-02-16 12:32:27 thor Exp $
**
*/

#ifndef __LIBUI_COLOR_H__
 #define __LIBUI_COLOR_H__

#include "libelfsh.h"

#define S_STARTCOLOR "\033["
#define C_STARTCOLOR '\033'

#define COLOR_TOKENS       50
#define COLOR_TOKEN_LEN    256

/** 
 * ELFsh color structure 
 */
typedef struct          s_color
{

#define COLOR_NONE         0
#define COLOR_BOLD         1
#define COLOR_UNDERLINE    4
#define COLOR_RESET        160
#define COLOR_SEPARE       ";"

#define COLOR_FG_BLACK     30
#define COLOR_FG_RED       31
#define COLOR_FG_GREEN     32
#define COLOR_FG_YELLOW    33
#define COLOR_FG_BLUE      34
#define COLOR_FG_MAGENTA   35
#define COLOR_FG_CYAN      36
#define COLOR_FG_WHITE     37

#define COLOR_BG_BLACK     40
#define COLOR_BG_RED       41
#define COLOR_BG_GREEN     42
#define COLOR_BG_YELLOW    43
#define COLOR_BG_BLUE      44
#define COLOR_BG_MAGENTA   45
#define COLOR_BG_CYAN      46
#define COLOR_BG_WHITE     47
  unsigned int                 fground;
  unsigned int                 bground;
  unsigned int                 bold;
  unsigned int                 underline;                                            
  
}                       color_t;

extern unsigned int		nocolor;

/* Color functions */
color_t         *revm_colortable(char *t, char *te);
int             revm_colorpattern(color_t *t, char *te, char *p);
color_t         *revm_colorblank();
char            *revm_colorget(char *p, char *ty, void *o);
void            revm_endline();

/* Simple */
char		*revm_colorinstr(char *text);
char            *revm_colorstr(char *t);
char            *revm_colorfieldstr(char *t);
char            *revm_colortypestr(char *t);
char		*revm_colorend(char *text); 
char		*revm_colorwarn(char *text);
char 		*revm_colorfunction(char *text);
char 		*revm_colorfilename(char *text);

/* Advanced */
char 		*revm_coloradv(char *ty, char *p, char *te);
char		*revm_colorinstr_fmt(char* p, char *t);
char            *revm_coloraddress(char *p, eresi_Addr a);
char            *revm_colornumber(char *p, eresi_Off n);
char            *revm_colorstr_fmt(char *p, char *t);
char            *revm_colorfieldstr_fmt(char *p, char *t);
char            *revm_colortypestr_fmt(char *p, char *t);
char		*revm_colorwarn_fmt(char *pattern, char *text);

/* Misc */
int		revm_color_count(char *string);
int		revm_color_size(char *string);

#endif
