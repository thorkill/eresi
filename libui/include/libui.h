/*
** libui.h for libui
** 
** Started on  Sun Jan 22 19:04:04 2006 mxatone
*/

#ifndef __LIBUI_H__
 #define __LIBUI_H__

/* Thanks to GNU we have readline */
#if defined(USE_READLN)
 #include <readline/readline.h>
 #include <readline/history.h>
#endif

#include "revm.h"
#include "libaspect.h"

/* Completion structure */
typedef struct        s_comp
{
#define		      ELFSH_COMPMAX   16
  char		      **cmds[ELFSH_COMPMAX];         /* Matchs list for readline */
}		      rlcomp_t;

/* Prototypes for readline functions */
void		readln_ctrl_set(int i, char c);
char            *readln_match(const char *text, int state);
void            readln_completion_install(char mode);
void		readln_completion_commands(hash_t *cmd_hash);
char		**readln_completion(const char* text, int start, int end);
int		readln_column_update();
int             readln_prompt_update(char *ptr, int size);
int             readln_prompt_restore();
char            *readln_input_check();
void            readln_input_log(char *str);
void            readln_install_clearscreen();
void            readln_screen_change(u_short isnew, char promptdisplay);
void            readln_history_dump(char mode);
void            readln_terminal_prepare(char mode);
void            readln_terminal_unprepare(char mode);

#endif
