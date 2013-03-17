/**
 * @defgroup REVM The Reflective ERESI Virtual Machine data
 */
/**
 * @file librevm/include/revm-data.h
 * @ingroup librevm
 *
 * $Id
 */
#ifndef __REVM_DATA_H_
 #define __REVM_DATA_H_

#if defined(__FreeBSD__)
  extern char **environ;
#endif
 
extern asm_processor	proc;

/* The world */
extern revmworld_t	world;

/* All the StandAlone hashtables */
extern list_t		frames_list;     /* List of frames for the ERESI interpreter */
extern hash_t		instrlists_hash; /* Tables of expression lists */
extern hash_t		cmd_hash;	 /* commands handlers */
extern hash_t		parser_hash;	 /* parsers handlers */
extern hash_t		file_hash;	 /* elfshobj_t pointers */
extern hash_t		const_hash;	 /* elf.h picked up constants values */
extern hash_t		redir_hash;	 /* Function redirections hash table */
extern hash_t		mod_hash;	 /* Modules name hash table */

/* The Level 1 object hash table : hash the object name and returns a L1handler_t* */
extern hash_t		L1_hash;	/* For HDR, SHT, PHT, GOT, DTORS, CTORS, DYNAMIC, SECTIONS */

/* The Level 2 object hash table list : hash the object name and returns a L2handler_t* */
extern hash_t		elf_L2_hash;	/* For the ELF header fields */
extern hash_t		sht_L2_hash;	/* For the Section header table fields */
extern hash_t		pht_L2_hash;	/* For the Program header table fields */
extern hash_t		got_L2_hash;	/* For the Global offset table fields */
extern hash_t		crs_L2_hash;	/* For the .ctors fields */
extern hash_t		drs_L2_hash;	/* For the .dtors fields */
extern hash_t		sym_L2_hash;	/* For symbol fields */
extern hash_t		rel_L2_hash;	/* For Relocation table fields */
extern hash_t		sct_L2_hash;	/* For Section data access */
extern hash_t		dynsym_L2_hash;	/* For .dynsym */
extern hash_t		dyn_L2_hash;	/* For .dynamic */

extern hash_t		vers_L2_hash;   /* For .gnu.version */
extern hash_t		verd_L2_hash;   /* For .gnu.version_d */
extern hash_t		vern_L2_hash;   /* For .gnu.version_r */
extern hash_t		hashb_L2_hash;  /* For .hash (bucket) */
extern hash_t		hashc_L2_hash;  /* For .hash (chain) */

extern hash_t           bg_color_hash; 	/* colors def */
extern hash_t           fg_color_hash; 	/* colors def */
extern hash_t           t_color_hash;  	/* colors type */

//extern hash_t		traces_cmd_hash;/* trace cmd table */
extern hash_t		goto_hash;	/* goto hash */

/* Lattice for I/O */
extern char		*(*hooks_input[REVM_IO_NUM])();
extern int		(*hooks_output[REVM_IO_NUM])(char *buf);

/* This has to be prototyped in libelfsh and not put in extern ! */
extern int 		elfsh_load_core_info(elfshobj_t *);

/* Network related variables */
//extern hash_t      elfsh_net_client_list;  /* The client socket's list */
extern int         elfsh_net_client_count; /* Number of clients connected */
//extern revmsock_t    elfsh_net_serv_sock;    /* The main socket structur */

/* Lib path */
extern char	   elfsh_libpath[BUFSIZ];

/* String table for .elfsh.strings */
extern char	   *strtable;
extern u_int	   strtable_current;
extern u_int	   strtable_max;

/* Prompt storage */
extern void	(*prompt_token_setup)(char *name, u_int size);
extern char	prompt_token[512];

#endif
