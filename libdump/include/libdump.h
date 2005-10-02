/*
 * libdump.h for elfsh
 * 
 * Started on Tue Feb 15 12:53:05 CET 2005 ym
 * Updated on Wed Mar  9 22:02:29 CET 2005 ym
 *
 * */

#ifndef _LIBDUMP_H_
#define _LIBDUMP_H_ 1

#include <libelfsh.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#include <netdb.h>


#ifndef __socklen_t_defined
typedef uint32_t socklen_t;
# define __socklen_t_defined
#endif


/* DUMP Recently Seen limit (s) */
#define DUMP_RS_LIMIT	60

/* DUMP max read try */
#define DUMP_MAXTRY	5

/* Debug flags */
#define __DEBUG_DUMP__	0

/* DUMP error msg */
char	*dump_error_msg;

typedef struct in_addr	dump_id_t;

typedef uint16_t	pkt_type_t;

typedef uint32_t	pkt_id_t;
typedef uint32_t	dump_len_t;

/* A packet */
typedef struct	s_pkt
{
  dump_id_t	src;
  dump_id_t	dst;
#define RR	1
#define Rr	2
#define DATA	3
  pkt_id_t	id;
  pkt_type_t	type;
  dump_len_t	size;
  dump_len_t	path_len;
  dump_id_t	*path;
  void		*data;
}		pkt_t;


/* Packet send queue */
typedef struct		s_pkt_wq 
{
  pkt_t			*pkt;
  struct s_pkt_wq	*next;
}			pkt_wq_t;


#define HDR_SIZE	(sizeof (pkt_t) - sizeof (void *) - sizeof (dump_id_t *)) 
#define HASHTABLE_SIZE	10

/* Dump world */
typedef struct	s_dump_world 
{
  hash_t	RRrecently_seen;	/* RR recently seen's id */
  hash_t	Rrrecently_seen;	/* Rr recently seen's id */
  hash_t	ports;			/* Neighbors */
  hash_t	myids;			/* Local ids */
  pkt_wq_t	*send_wq;		/* Send wait queue */
  int		sock;			/* Main socket */
}		dump_world_t;

/* DUMP world */
dump_world_t	dump_world;



/* DUMP functions */
/* dump.c */
int		dump_init();
pkt_id_t	dump_mk_pkt_id(); 
int		dump_free(pkt_t *pkt);
int		dump_lookup_RR_recently_seen(pkt_id_t id);
int		dump_lookup_Rr_recently_seen(pkt_id_t id);
int		dump_add_RR_recently_seen(pkt_id_t id);
int		dump_add_Rr_recently_seen(pkt_id_t id);
int		dump_clean_recently_seen();
int		dump_add_neighbor(int s, dump_id_t peer_addr);
int		dump_lookup_neighbor(dump_id_t peer_addr); 
int		dump_del_neighbor(dump_id_t peer_addr);
pkt_t		*dump_lookup_send_queue(dump_id_t dst);
int		dump_add_send_queue(pkt_t *pkt);
int		dump_del_send_queue(pkt_t *pkt);
int		dump_print_pkt(pkt_t *pkt);
dump_id_t	dump_get_myid(int s);
int		dump_is_myid(dump_id_t id);
int		dump_add_myid(dump_id_t id, int s);
int		dump_del_myid(int s);
int		dump_disconnect(int s);
int		dump_disconnect_host(char *host);
int		dump_connect_to(char *host, u_int port);
/* recv.c */
void		*dump_receive_cb(int s);
/* send.c */
int		dump_send_DATA(void); 
int		dump_send_real(int s, pkt_t *data);
int		dump_forward_RR(void);
int		dump_forward_Rr(void);
int		dump_send_RR(dump_id_t src, 
			     dump_id_t dst, 
			     dump_len_t path_len, 
			     dump_id_t *path, 
			     int next_hop_socket, 
			     pkt_id_t id);
int		dump_send_Rr(dump_id_t src, 
			     dump_id_t dst,
			     dump_len_t path_len, 
			     dump_id_t *path, 
			     int next_hop_scoket, 
			     pkt_id_t id);
int		dump_send(dump_id_t dst, void *data, dump_len_t len);

#endif /* _LIBDUMP_H_ */
