/**
** @file revm.h for librevm in ERESI
** @ingroup librevm_hdr 
** Started on  Thu Feb 22 07:19:04 2001 jfv
** Moved from elfsh to librevm on January 2007 jfv
** $Id: revm-io.h,v 1.56 2008-02-16 12:32:27 thor Exp $
*/
#ifndef __REVM_IO_H_
 #define __REVM_IO_H_

#ifndef __KERNEL__
 #include <regex.h>
 #include <sys/types.h>
 #include <sys/select.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
#endif

#include "librevm-color.h"

#if defined(ERESI_NET)
 #include <libdump.h>
#endif

/* Network related defines */
#define REVM_PORT		4444
#define REVM_DUMP_PORT		4445
#define REVM_MAX_CLIENTS	4
#define REVM_MAX_SEND_SIZE	2048

/* DUMP related defines */
#define REVM_DUMP_CMD	1
#define REVM_DUMP_MSG	2

#define REVM_NAME	"revm"
#define	REVM_VERSION	"0.81"
#define	REVM_RELEASE	"b4"
#define REVM_EDITION	"dev"

/* Unused, feel free to try it, its awesome */
#define ELFSH_CIRCUS_PROMPT \
"\033[00;01;30m("        \
"\033[00;01;31me"        \
"\033[00;01;32ml"        \
"\033[00;01;33mf"        \
"\033[00;01;34ms"        \
"\033[00;01;35mh"        \
"\033[00;01;36m-"        \
"\033[00;01;30m"         \
 ERESI_VERSION           \
"\033[00;01;35m-"        \
"\033[05;01;35mcircus"   \
"\033[00;01;35m-edition" \
"\033[00;01;30m)"        \
"\033[00m " 

/* The ERESI team crew prompt */
#define ELFSH_CREW_PROMPT  \
"\033[00;01;30m("          \
 "\033[00;01;31melfsh"     \
 "\033[00;01;30m-"         \
 "\033[00;01;33m"          \
 ERESI_VERSION             \
 "\033[00;01;30m-"         \
 "\033[00;01;33m"          \
 ERESI_RELEASE             \
 "\033[00;01;30m-"         \
 "\033[00;01;32mdevhell"   \
 "\033[00;01;30m)"         \
 "\033[00m " 

/* Be original, do your own */
#define  ERESI_PROMPT ELFSH_CREW_PROMPT
//#define ERESI_PROMPT ELFSH_CIRCUS_PROMPT

/* Now some strings for graphviz/dot */
#define GVZ_COLOR_BLUE		"\"blue\""
#define GVZ_COLOR_CORAL		"\"coral\""
#define GVZ_COLOR_CYAN		"\"cyan\""
#define GVZ_COLOR_RED		"\"red\""
#define GVZ_COLOR_GREEN		"\"green\""
#define GVZ_COLOR_BLACK		"\"black\""
#define GVZ_COLOR_GREY		"\"grey\""
#define GVZ_COLOR_BROWN		"\"brown\""
#define GVZ_COLOR_PINK		"\"pink\""
#define GVZ_COLOR_YELLOW	"\"yellow\""

#define GVZ_NODE_EPOINT		0
#define GVZ_NODE_MAIN		1
#define GVZ_NODE_INTERN		2
#define GVZ_NODE_EXTERN		3
#define GVZ_NODE_UNKNOWN	4

/* XXX: Deprecated? */
#define BLK_COLOR_FUNC		GVZ_COLOR_GREEN
#define BLK_COLOR_NORM		GVZ_COLOR_CYAN
#define BLK_COLOR_RET		GVZ_COLOR_CORAL
#define BLK_COLOR_TRUE		GVZ_COLOR_GREEN
#define BLK_COLOR_FALSE		GVZ_COLOR_RED
#define BLK_COLOR_CONT		GVZ_COLOR_BLACK
#define BLK_COLOR_CALL		GVZ_COLOR_BLUE
#define BLK_COLOR_JUMP		GVZ_COLOR_BLACK

#define LNK_COLOR_CALL		GVZ_COLOR_BROWN
#define LNK_COLOR_RET		GVZ_COLOR_YELLOW
#define LNK_COLOR_TRUE		GVZ_COLOR_GREEN
#define LNK_COLOR_FALSE		GVZ_COLOR_RED
#define LNK_COLOR_DELAY		GVZ_COLOR_PINK

#define FNC_COLOR_INTERN	GVZ_COLOR_GREY
#define FNC_COLOR_EPOINT	GVZ_COLOR_GREEN
#define FNC_COLOR_EXTERN	GVZ_COLOR_CYAN
#define FNC_COLOR_MAIN		GVZ_COLOR_BLUE
#define FNC_COLOR_DEFAULT	GVZ_COLOR_GREY

/* Return of an input function in case of ignorable input */
#define REVM_INPUT_VOID		(-1)
#define REVM_INPUT_EXIT		(-2)
#define	REVM_INPUT_TRANSFERED	(-3)

/** ERESI outut caching structure */
typedef struct          s_outbuf
{
  int			nblines;
  unsigned int          cols;
  unsigned int          lines;
  char                  ignore;
}			revmoutbuf_t;

/** REVM socket structure */
typedef struct       s_socket
{
  int                socket;      /*! The socket */
  char               **recvd;     /*! List of received buffer */
  
  /* XXX: NEW/OLD is not explicit enough, rename ... */
#define NEW   1
#define OLD   0
  int                recvd_f;     /*! NEW if the buffer was not passed to the parser yet */
#define YES 1
#define NO  0
  int                ready_f;     /*! Have we received the trailing \n ? */

#ifndef __KERNEL__
  struct sockaddr_in addr;        /*! sockaddr_in struct */
#endif

}                    revmsock_t;


/** ERESI INPUT/OUTPUT abstraction */
typedef struct	s_io
{  
#define		REVM_IO_STD     1
#define		REVM_IO_NET     2
#define		REVM_IO_DUMP    3
#define		REVM_IO_GDB	4
#define		REVM_IO_NUM     5

  char		type;                   /*! IO type           */
  int		input_fd;               /*! Input file        */
  int		output_fd;              /*! Output file       */
  char		*(*input)(void);        /*! Read Input data   */
  char		*(*old_input)(void);    /*! Old Input handler */
  int		(*output)(char *buf);   /*! Write output data */
  revmoutbuf_t	outcache;

  /* Readline IO specific */
  char		*buf;                  /*! readline line */
  char		*savebuf;
  int		rl_point;
  int		rl_end;

  /* DUMP IO specific */
  int		new;                   /*! 0 if already used */

#if defined(ERESI_NET)
  pkt_t		*pkt;                  /*! Last received dump */
#else
  void		*pkt;		       /*! Unused else */
#endif

  revmsock_t	sock;		       /* Socket information */
}               revmio_t;


/**
 * Screen cache for each workspace 
 */
typedef struct        s_screen
{
  unsigned int        x;              /*! Screen height */
  unsigned int        y;              /*! Screen width */
  char                *buf;           /*! Screen buffer */
  char                *head;          /*! Buffer's beginning */
  char                *tail;          /*! Buffer's end */
}                     revmscreen_t;


/** 
 * @brief REVM job structure, one per client 
 */
typedef struct		s_workspace
{
  char			*name;		  /*! Name of the job */
  u_char		active;		  /*! Is the workspace active ? */
  time_t		createtime;       /*! Workspace creation time */
  int			logfd;            /*! Log file descriptor */
  revmscreen_t		screen;           /*! Last printed screen */
  char			*oldline;	  /*! Previous command line */

#define			REVM_JOB_LOGGED (1 << 0)
  u_char                state;            /*! Job state flags */

#define			REVM_INPUT     0
#define			REVM_OUTPUT    1
  revmio_t		io;		  /*! Current IO for this job */

}			revmworkspace_t;


/**
 * @brief Hold all the VM flags, sort of global context 
 */
typedef struct        s_state
{
  char                revm_quiet;       /*! Quiet mode : 0 or 1 */
  char                revm_gvl;         /*! Graph verbose level : 0 or 1 */
  char                revm_force;       /*! Force mode : 0 or 1 */
  char                revm_use_regx;    /*! Is a global regx available ? */
  regex_t             revm_regx;        /*! Global regx */
  char                *revm_sregx;      /*! Global regx in string format */
  char                revm_use_alert;   /*! Is an alert regx available ? */
  regex_t	      revm_alert;       /*! Alert regx */
  char                *revm_salert;     /*! ALert regx in string format */
  char                *sort;            /*! Actual sorting choice */
  char                *input;           /*! Implicit File input (-f option) */
  char                *output;          /*! Implicit File output (-w option) */
#define       REVM_STATE_CMDLINE	0
#define       REVM_STATE_SCRIPT		1
#define       REVM_STATE_INTERACTIVE    2
#define       REVM_STATE_DEBUGGER	3
#define	      REVM_STATE_TRACER		4
  char                revm_mode;        /*! Command line, scripting, interactive, debugger ? */
#define	      REVM_SIDE_CLIENT		0
#define	      REVM_SIDE_SERVER		1
  char		      revm_side;	      /*! Useful for the bi-partite debugger */
  char		      revm_sourcing;    /*! Are we sourcing a script ? */
  char                revm_stopped;     /*! We are in a signal handler */
  char                revm_shared;      /*! Next opened object must be shared */
  char                revm_net;         /*! We are a node connected to the ERESI network */
  u_int               lastid;           /*! Last Object ID */
}		       revmstate_t;

/* Extern variables */
extern int		 elfsh_net_client_count;

/* Parsing, Scanning, I/O functions */
char            *revm_getln();
int		revm_initio();
char            **revm_input(int *argc, char *mayhavealine);
char		*revm_stdinput();
char		*revm_fifoinput();
int		revm_flush();
int		revm_output(char *str);
int		revm_output_nolog(char *str);
int		revm_outerr(char *str);
int		revm_output_bcast(char *str);
int		revm_stdoutput(char *str);
int		revm_display_prompt();
void		revm_setinput(revmworkspace_t *j, int fd);
void		revm_setoutput(revmworkspace_t *j, int fd);
void		revm_log(char *str);
int             revm_closelog();
char		*revm_get_prompt();
void		revm_set_prompt(void (*func) (char *name, u_int size));

/* Network related functions */
int		revm_net_init();
int		revm_check_net_select(fd_set *sel_sockets, int sock);
int		revm_select();
char		*revm_net_input();
int		revm_net_output(char *buf);
int		revm_net_recvd();
int		revm_net_accept();
int		revm_dump_accept();
int		revm_socket_del(char *inet_addr);
int		revm_socket_get_nb_recvd(char *inet);
int		revm_update_recvd(revmsock_t *socket);
char		*revm_socket_merge_recvd(revmsock_t *socket);
int             revm_getmaxfd();
int             revm_clean_jobs();

char            *revm_netgdb_input(void);
int             revm_netgdb_output(void);

/* Readline interface */
int             revm_init_history(int mode);
int             revm_quit(int mode);
void            revm_ctrl_set(int i, char c);
void            revm_completion_install(char mode, char side);
void            revm_completion_commands(hash_t *cmd_hash);
char            **revm_completion(const char* text, int start, int end);
int             revm_column_update();
int             revm_prompt_update(char *ptr, int size);
int             revm_prompt_restore();
void		revm_prompt_log();
void		revm_prompt_postselect_restore(fd_set *sel_sockets);
char            *revm_input_check();
void            revm_input_log(char *str);
void		revm_input_prelog(char *buf);
void            revm_screen_change(u_short isnew, char promptdisplay);
void            revm_terminal_prepare(char mode);
void            revm_terminal_unprepare(char mode);
void		revm_buffer_reset(u_short isnew);
void		revm_job_preswitch();
void		revm_job_postswitch();
void		revm_screen_getsize(int *lines, int *cols);
void		revm_buffer_free(char *buf);
void		revm_strip_char(char *str, char c);
void		revm_callback_handler_remove();
void		revm_conditional_rlquit();
void		revm_rlfifo_write();
int		revm_is_enabled();
int		revm_is_stdinput();
void		revm_callback_handler_install(char *prompt, void (*fct)(char *str));
void		revm_line_handler(char *c);

#endif
