/*
 * send.c for elfsh
 *
 * Started on Tue Feb 15 12:51:34 CET 2005 ym
 * Updated on Wed Mar  9 22:02:29 CET 2005 ym
 *
 */

#include "libdump.h"

/* really do send */
int	dump_send_real(int s, pkt_t *pkt)
{
  int	ret;
  void  *data;
  int	len = HDR_SIZE + ntohl(pkt->path_len)*sizeof (dump_id_t) + ntohl(pkt->size);
    
  ELFSH_NOPROFILE_IN();

  if (pkt == NULL)
    {
      /* ERROR error handling */
#if !defined(ELFSH_INTERN)
      printf("[EE] dump_send_real illegal argument");
#endif 
      return (-1);
    }
    

  XALLOC(data, sizeof (char)*len, -1);

  /* copy the header part */
  memcpy(data, pkt, HDR_SIZE);

  /* copy the path part */
  memcpy(data + HDR_SIZE, pkt->path, ntohl(pkt->path_len)*sizeof (dump_id_t));
    
  /* copy the data part */
  memcpy(data + HDR_SIZE + ntohl(pkt->path_len)*sizeof (dump_id_t), 
	 pkt->data, 
	 ntohl(pkt->size));

#if __DEBUG_DUMP__    
  printf("[DUMP] ----send - len = %d----\n", len);
  dump_print_pkt(pkt);
  printf("[DUMP] ----------\n");
#endif 

  ret = send(s, data, len, 0);
  if (ret < 0)
    {
      /* ERROR error handling */
#if !defined(ELFSH_INTERN)
      printf("[EE] send failed");
#endif
      exit(-1);
    }
    
  free(data);
    
  return ret;
}

/* send a RR packet */
int	dump_send_RR(dump_id_t src, 
		     dump_id_t dst, 
		     dump_len_t path_len, 
		     dump_id_t *path, 
		     int next_hop_socket,
		     pkt_id_t id
		     )
{
  int	 ret;
  pkt_t	 *pkt;

  ELFSH_NOPROFILE_IN();

  XALLOC(pkt, sizeof (pkt_t), -1);

  pkt->src = src;
  pkt->dst = dst;
  if (id == 0)
    pkt->id = dump_mk_pkt_id();
  else 
    pkt->id = id;
  pkt->type = htons(RR);
  pkt->size = htonl(0);
  pkt->path_len = htonl(path_len);
  pkt->path = path;
  pkt->data = NULL;
    
  /* send it now */
  ret = dump_send_real(next_hop_socket, pkt);

  free(pkt);
  return ret;
}

/* send a Rr packet */
int	dump_send_Rr(dump_id_t src, 
		     dump_id_t dst, 
		     dump_len_t path_len, 
		     dump_id_t *path, 
		     int next_hop_socket,
		     pkt_id_t id
		     )
{
  int	ret;
  pkt_t *pkt;

  ELFSH_NOPROFILE_IN();

  XALLOC(pkt, sizeof (pkt_t), -1);
   
  pkt->src = src;
  pkt->dst = dst;
  if (id == 0)
    pkt->id = dump_mk_pkt_id();
  else 
    pkt->id = id;
  pkt->type = htons(Rr);
  pkt->size = htonl(0);
  pkt->path_len = htonl(path_len);
  pkt->path = path;
  pkt->data = NULL;
    
  if (next_hop_socket == 0)
    return (-1);

  /* send it now */
  ret = dump_send_real(next_hop_socket, pkt);

  free(pkt);
  return ret;
}
 
/* send a DATA packet */
int		dump_send(dump_id_t dst, void *data, dump_len_t len)
{
  /* do not actually send data, but send RR 
     and data will be sent when Rr arrive 
  */
  dump_id_t	*path;
  int		next_hop_socket;
  int		index;
  hashent_t	*actual;
  pkt_t		*pkt;

  ELFSH_NOPROFILE_IN();

  XALLOC(pkt, sizeof (pkt_t), -1);
  
  pkt->src = dump_get_myid(0);
  pkt->dst = dst;
  pkt->id = dump_mk_pkt_id();
  pkt->type = htons(DATA);
  pkt->size = htonl(len);
  pkt->path_len = htonl(0);
  pkt->path = NULL;
  pkt->data = data;

  /* add pkt to send wait queue */
  dump_add_send_queue(pkt);

  /* prepare RR */
  XALLOC(path, sizeof (dump_id_t), -1);
  
  /* search all neighbors ...*/
  for (index = 0; index < dump_world.ports.size; index++)
    for (actual = &dump_world.ports.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	next_hop_socket = (long) actual->data;
	     
	/* fill the first path node */
	*path = dump_get_myid(next_hop_socket);

	/* ... and send RR to them */
#if __DEBUG_DUMP__
	printf ("[DUMP] send RR\n");
#endif
	dump_send_RR(dump_get_myid(next_hop_socket), 
		     dst, 
		     1,	
		     path, 
		     next_hop_socket, 
		     0);
      }

  free(path);
  return 0;
}

