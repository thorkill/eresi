/*
** 
** skelet.h in 
** 
** Author  : <sk at devhell dot org>
** Started : Thu May 29 20:35:01 2003
** Updated : Fri Nov 28 00:56:54 2003
*/

#ifndef SKELET_H_
#define SKELET_H_

#include <stdio.h>
#include <elfsh.h>
#include <libelfsh.h>
#include <libasm.h>
#include "libbtree.h"


#define GVZ_COLOR_BLUE  "\"blue\""
#define GVZ_COLOR_CORAL "\"coral\""
#define	GVZ_COLOR_CYAN	"\"cyan\""
#define GVZ_COLOR_RED	"\"red\""
#define GVZ_COLOR_GREEN	"\"green\""
#define GVZ_COLOR_BLACK	"\"black\""

#define BLK_COLOR_FUNC	GVZ_COLOR_GREEN
#define BLK_COLOR_NORM	GVZ_COLOR_CYAN
#define BLK_COLOR_RET	GVZ_COLOR_CORAL

#define BLK_COLOR_TRUE	GVZ_COLOR_GREEN
#define BLK_COLOR_FALSE	GVZ_COLOR_RED
#define BLK_COLOR_CONT	GVZ_COLOR_BLACK
#define BLK_COLOR_CALL	GVZ_COLOR_BLUE
#define BLK_COLOR_JUMP	GVZ_COLOR_BLACK



#define ELFSH_CMD_FLOWJACK	"flowjack"
#define	ELFSH_CMD_GRAPH		"graph"
#define ELFSH_CMD_FLOW		"flow"
#define ELFSH_CMD_INSPECT	"inspect"
#define ELFSH_CMD_FLOWLOAD	"flowload"
#define ELFSH_CMD_FLOWSAVE	"flowsave"

#define ELFSH_SECTION_BLOCKSTR	".control"


/**
 * structure s_list used for arguments enumeration/
 *
 */

#define EXEC_MISSED	0
#define EXEC_PASSED	1



struct s_arglist {
  int   num;		/* argument number		*/
  int	used;		/* number of access to argument	*/
  struct s_arglist	*next;
};

struct s_buf
{
  char	*data;
  u_int	maxlen;
  u_int	allocated;
  u_int	block_counter;
  elfshobj_t	*obj;
};

struct s_disopt
{
  elfshobj_t	*file;
  u_int		counter;
  int		level;
};

/**
 *
 *
 *
 */

struct s_condition {
  int	sign;
#define CONDITION_SIGNED
#define CONDITION_UNSIGNED
  char	*dst;
  char	*src;
  char	*cmp;
};



extern  hash_t		block_hash;


/**	struct s_caller is to reference blocks calling block
 * field type may help us to analyse blocks to build higher
 * logical structure.
 * it contains 
 * - a type field to know how the block is invoked
 * - a vaddr field to know address of instruction which has invoked it.
 *
 *
 */

struct s_caller {
#define   CALLER_CONT   0	/* block is invoked from previous block	*/
#define   CALLER_JUMP	1	/* block is invoked by some JUMP,JCC	*/
#define   CALLER_CALL	2	/* block is invoked by a call		*/
#define	  CALLER_RET	3	/* block is ended by a RET		*/
#define   CALLER_UNKN	4	/* block was not analysed		*/
  int			type;
  u_int			vaddr;
  struct s_caller	*next;
};

/**
 * structure used to describe blocks
 *  
 */

struct s_iblock {
  char			*symstr;
  u_int			vaddr;	/* block starting virtual address	*/
  u_int			size;	/* block size				*/
  struct s_caller	*caller;/* linked list of invvoking instr.      */
  u_int			altern;	/* vaddr of alternate path		*/
  u_int			altype; /* alternative path invokation type	*/
  u_int			contig; /* vaddr of next block if continuing	*/
  struct s_iblock	*next;
  btree_t		*btree;
  int			passed;
};

/**
 * block structure as it is stored in ".control" section
 *
 */

typedef struct s_elfshblock {
  u_int	vaddr;		/* virtual address of block	*/
  u_int	size;		/* size of block		*/
  u_int	contig;		/* contiguous execution path	*/
  u_int	altern;		/* alternate execution path	*/
  u_int	altype;		/* execution breaking instr.	 */
} elfshblk_t;

typedef struct s_elfshblref {
  u_int	vaddr;		/* address of instruction invoking block	*/
  u_int	type;		/* type of invokation				*/
} elfshblkref_t;

struct s_function {
  u_int			vaddr;
  u_int			size;
  char			*symstr;
  struct s_caller	*blocks;
  struct s_function	*next;
};



/*
 *
 */

int	cmd_flowload(void);
int	cmd_flowsave(void);
int	cmd_graph(void);
int	inspect_cmd(void);
int	cmd_flow(void);
int	cmd_testflow(void);
int	cmd_flowjack(void);
/*	sk_data.c	*/
/*
struct s_list *list_get_arg_by_num(struct s_list *, int);
void	list_add_arg(struct s_list **, int);
*/

/*	sk_control.c	*/
u_int	trace_start(elfshobj_t *, char *, u_int, u_int, struct s_iblock **,
		    struct s_function **);
void	trace_control(elfshobj_t *, asm_instr *, u_int, struct s_iblock **);

/*	sk_blocks.c	*/
void	free_blocks(struct s_iblock *);
int     load_blocks(elfshobj_t *, struct s_iblock **);
int	store_blocks(elfshobj_t *, struct s_iblock *, int);
struct s_iblock	*block_get_by_vaddr(struct s_iblock *, u_int, int);
void	block_clean_passed(struct s_iblock *);
void	block_add_caller(struct s_iblock *, u_int, int);
void	block_add_list(struct s_iblock **, struct s_iblock *);
int	block_is_called(struct s_iblock *);
int	display_blocks(elfshobj_t *, struct s_iblock *, int);
void	dump_block(struct s_iblock *b);
struct s_iblock	*block_create(u_int, u_int);
int insert_destination_address(elfshobj_t *, asm_instr *, u_int, 
			       struct s_iblock **);


/*	sk_functions.c	*/
/*
struct s_function	*function_get_by_vaddr(struct s_function *, u_int);
u_int	function_get_parent(struct s_iblock *, struct s_iblock *, int);
void	display_functions(elfshobj_t *, struct s_function *);
void	trace_functions(elfshobj_t *, struct s_function **, struct s_iblock *);
void	functions_find(struct s_function **, struct s_iblock *);
void	function_add_list(struct s_function **, struct s_function *);
struct s_function	*function_create(u_int, char *);
*/

#endif



