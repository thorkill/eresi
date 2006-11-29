/*
** libui-color.h for libui
** 
** Started on  Sun Nov 26 22:13:54 2006 mxatone
*/

#ifndef __LIBUI_COLOR_H__
 #define __LIBUI_COLOR_H__

#include "libelfsh.h"

#define S_STARTCOLOR "\033["
#define C_STARTCOLOR '\033'

#define COLOR_TOKENS       50
#define COLOR_TOKEN_LEN    140

/* ELFsh color structure */
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
  u_int                 fground;
  u_int                 bground;
  u_int                 bold;
  u_int                 underline;                                            
  
}                       color_t;

extern u_int		nocolor;

/* Color functions */
color_t         *vm_colortable(char *t, char *te);
int             vm_colorpattern(color_t *t, char *te, char *p);
color_t         *vm_colorblank();
char            *vm_colorget(char *p, char *ty, void *o);
void            vm_endline();

/* Simple */
char		*vm_colorinstr(char *text);
char            *vm_colorstr(char *t);
char            *vm_colorfieldstr(char *t);
char            *vm_colortypestr(char *t);
char		*vm_colorend(char *text); 
char		*vm_colorwarn(char *text);

/* Advanced */
char 		*vm_coloradv(char *ty, char *p, char *te);
char		*vm_colorinstr_fmt(char* p, char *t);
char            *vm_coloraddress(char *p, elfsh_Addr a);
char            *vm_colornumber(char *p, u_int n);
char            *vm_colorstr_fmt(char *p, char *t);
char            *vm_colorfieldstr_fmt(char *p, char *t);
char            *vm_colortypestr_fmt(char *p, char *t);
char		*vm_colorwarn_fmt(char *pattern, char *text);

/* Misc */
int		vm_color_count(char *string);
int		vm_color_size(char *string);


#endif
