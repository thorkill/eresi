/*
 * recv.c for elfsh
 *
 * Started on Tue Feb 15 12:51:34 CET 2005 ym
 * Updated on Wed Mar  9 22:02:29 CET 2005 ym
 *
 */

#include "libdump.h"


/* read a complete packet from given socket */
pkt_t	*dump_recv_pkt(int s)
{
  int	tmp  = 0;
  int	tmp2 = 0;
  int	len  = 0;
  pkt_t *msg;
  int	try = 0;

  XALLOC(msg, sizeof (pkt_t), NULL);

  /* read fixed size header part */
  do
    {
      tmp2 = recv(s, (void *) msg + tmp,
		  (size_t) (HDR_SIZE - tmp),
		  MSG_DONTWAIT);

      /* connection closed */
      if (tmp2 == 0)
        {
#if !defined(ELFSH_INTERN)
	  printf("[WW] Connection closed by remote host (1)\n");
#endif
	  free(msg);
	  return  (pkt_t *)(-1);
        }

      /* error */
      if (tmp2 == (-1))
        {
	  /* EAGAIN */
	  if (tmp2 == EAGAIN)
	    {
	      try++;
	      if (try > DUMP_MAXTRY)
		{
		  free (msg);
		  return (pkt_t *)(-1);
		}
	      continue;
	    }
#if !defined(ELFSH_INTERN)
	  printf("[EE] Error while reading on socket (1)\n");
	  perror("recv");
#endif
	  free(msg);
	  return (pkt_t *)(-1);
        }
      
      len += tmp2;
      tmp += tmp2;

    } while (tmp != (size_t)(HDR_SIZE));

  if (ntohl(msg->path_len) != 0)
    {
      /* read pkt path from header */
      XALLOC(msg->path, ntohl(msg->path_len)*sizeof (dump_id_t), (pkt_t *) -1);

      tmp = 0;
      do 
        {
	  tmp2 = recv(s,
		      (void *) msg->path + tmp,
		      (size_t) ntohl(msg->path_len)*sizeof (dump_id_t) - tmp, 
		      MSG_DONTWAIT);

	  /* connection closed */
	  if (tmp2 == 0)
            {
#if !defined(ELFSH_INTERN)
	      printf("[WW] Connection closed by remote host (2)\n");
#endif
	      free(msg);
	      return (pkt_t *)(-1);
            }

	  /* error */
	  if (tmp2 == (-1))
            {
	      /* EAGAIN */
	      if (tmp2 == EAGAIN)
		{
		  try++;
		  if (try > DUMP_MAXTRY)
		    {
		      free(msg->path);
		      free(msg);
		      return (pkt_t *)(-1);
		    }
		  continue;
		}
#if !defined(ELFSH_INTERN)
	      printf("[EE] Error while reading on socket (2)\n");
	      perror("recv");
#endif
	      free(msg->path);
	      free(msg);
	      return  (pkt_t *)(-1);
            }

	  len += tmp2;
	  tmp += tmp2;

        } while (tmp != ntohl(msg->path_len)*sizeof (dump_id_t));
    }
  else
    msg->path=NULL;

  /* read payload */
  if (ntohl(msg->size) != 0)
    {
      XALLOC(msg->data, (size_t) sizeof (char)*ntohl(msg->size), (pkt_t *) -1);

      tmp = 0;
      do
        {
	  tmp2 = recv(s,
		      (void *) msg->data + tmp,
		      (size_t) ntohl(msg->size)*sizeof (char) - tmp, 
		      MSG_DONTWAIT);

	  /* connection closed */
	  if (tmp2 == 0)
            {
#if !defined(ELFSH_INTERN)
	      printf("[WW] Connection closed by remote host (3)\n");
#endif
	      free(msg->path);
	      free(msg->data);
	      free(msg);
	      return (pkt_t *)(-1);
            }

	  /* error */
	  if (tmp2 == (-1))
            {
	      /* EAGAIN */
	      if (errno == EAGAIN)
		{
		  try++;
		  if (try > DUMP_MAXTRY)
		    {
		      free(msg->path);
		      free(msg->data);
		      free(msg);
		      return (pkt_t *)(-1);
		    }
		  continue;
		}
#if !defined(ELFSH_INTERN)
	      printf("[EE] Error while reading on socket (3)\n");
	      perror("recv");
#endif
	      free(msg->path);
	      free(msg->data);
	      free(msg);
	      return (pkt_t *)(-1);
            }

	  len += tmp2;
	  tmp += tmp2;

        } while (tmp != ntohl(msg->size)*sizeof (char));
    }
  else
    msg->data = NULL;
  
  return msg;
}


/* handle RR packet */
int		dump_receive_RR(pkt_t *pkt)
{
  dump_id_t	prev;
  dump_id_t	*npath;
  hashent_t	*actual;
  int		index;
    
  if (dump_lookup_RR_recently_seen(pkt->id))
    {
      return (-1);
    }
    
  /* add packet's id to RR table */
  dump_add_RR_recently_seen(pkt->id);

  /* sanity check */
  if (ntohl(pkt->path_len) == 0)
    return (-1);

  /* add myid to path's tail */
  XALLOC(npath, sizeof (dump_id_t)*(ntohl(pkt->path_len)+1), -1);
    
  /* back the previous hop up */
  prev = pkt->path[ntohl(pkt->path_len)-1];

  /* copy previous path's nodes */
  memcpy (npath, pkt->path, ntohl(pkt->path_len)*sizeof (dump_id_t));

  /* is my id ? */
  if (dump_is_myid(pkt->dst))
    {
      /* add myid to path */
      npath[ntohl(pkt->path_len) + 1 - 1] = 
	dump_get_myid(dump_lookup_neighbor (prev));

#if __DEBUG_DUMP__
      printf("[DUMP] dump_receive_RR : it's me !! \n");
#endif
      /* send Rr packet with new id */
      if (dump_send_Rr(pkt->dst, 
		       pkt->src, 
		       (ntohl(pkt->path_len)+1), 
		       npath, 
		       dump_lookup_neighbor(prev), 
		       0) < 0)
        {
#if !defined(ELFSH_INTERN)
	  fprintf(stderr, "dump_send_Rr error (1)\n");
#endif
	  free(npath);
	  return (-1);
        }
    }
  else
    {
#if __DEBUG_DUMP__
      printf("[DUMP] dump_receive_RR : it's NOT me !! \n");
#endif
      /* send it to all neighbors ... */
      for (index = 0; index < dump_world.ports.size; index++)
	for (actual = &dump_world.ports.ent[index];
	     actual != NULL && actual->key != NULL;
	     actual = actual->next)
	  {
	    /* ... but the sender */
	    if (strcmp(actual->key, inet_ntoa(prev)))
	      {
#if __DEBUG_DUMP__
		printf("[DUMP] dump_receive_RR :"
		       " send to %s, it's NOT the Request sender\n",
		       actual->key);
#endif
		/* add myid to path */
		npath[ntohl(pkt->path_len) + 1 - 1] = 
		  dump_get_myid((long) actual->data);
      
		/* send RR packet with same id */
		if (dump_send_RR(pkt->src, 
				 pkt->dst, 
				 (ntohl(pkt->path_len)+1), 
				 npath, 
				 (long) actual->data, 
				 pkt->id) < 0)
		  {
#if !defined(ELFSH_INTERN)
		    printf("[EE] dump_send_RR error (2)\n");
#endif
		    free(npath);
		    return (-1);
		  }
	      }

#if __DEBUG_DUMP__ && !defined(ELFSH_INTERN)
	    else
	      {
		printf("[DUMP] dump_receive_RR :"
		       " don't send to %s, it's the Request sender\n",
		       actual->key);
	      }
#endif
	  }
      
    }
    
  free(npath);
  return 0;
}

/* handle Rr packet */
int	dump_receive_Rr(pkt_t *pkt)
{
  int	i;
  int	next_hop_socket;
  pkt_t *spkt;

    
  if (dump_lookup_Rr_recently_seen(pkt->id))
    {
      return (-1);
    }
    
  /* add packet's id to Rr table */
  dump_add_Rr_recently_seen(pkt->id);

  /* sanity check */
  if (ntohl(pkt->path_len) == 0)
    return (-1);

  /* is my id ? */
  if (dump_is_myid(pkt->dst))
    {
#if __DEBUG_DUMP__
      printf("[DUMP] dump_receive_Rr : it's me !! \n");
#endif
      /* if data are waiting for this route to be sent, send it */
      while ((spkt = dump_lookup_send_queue(pkt->src)) != NULL)
	{
#if __DEBUG_DUMP__
	  printf("[DUMP] dump_receive_Rr : a packet to send \n");
#endif
	  spkt->path = pkt->path;
	  spkt->path_len = pkt->path_len; // pkt->path_len is in NBO
	  
	  /* XXX ensure that it is correct */
	  next_hop_socket = dump_lookup_neighbor(pkt->path[1]);
	  
	  if (next_hop_socket == 0)
	    {
	      /* ERROR improve that */
#if !defined(ELFSH_INTERN)
	      fprintf(stderr, "[EE] error while sending packet\n");
#endif
	      exit(-1); 
	    }
	  
	  // XXX 
	  // spkt->src = dump_get_myid (next_hop_socket);
	  
	  /* send the packet */
	  dump_send_real(next_hop_socket, spkt);
	  
	  /* free pkt */
	  free(spkt->data);
	  free(spkt);

	  /* remove it from wait queue */
	  dump_del_send_queue(spkt);
	}
      return 0;
    }
  else
    {
#if __DEBUG_DUMP__
      printf("[DUMP] dump_receive_Rr : it's NOT me !! \n");
#endif
      /* send it to next path's hop with same id*/
      for (i = 0; i < ntohl(pkt->path_len); i++)
        {
	  if (dump_is_myid(pkt->path[i]))
            {
	      if (i > 0)
		{
		  /* update my id in path according to outgoing socket */
		  pkt->path[i] = 
		    dump_get_myid(dump_lookup_neighbor(pkt->path[i - 1]));
		  
		  if (dump_send_Rr(pkt->src, 
				   pkt->dst, 
				   ntohl(pkt->path_len), 
				   pkt->path, 
				   dump_lookup_neighbor(pkt->path[i - 1]), 
				   pkt->id) < 0)
		    {
#if !defined(ELFSH_INTERN)
		      printf("[EE] error occurs while forwarding Rr packet\n");
#endif
		      return (-1); 
		    }   
                }
	      else
                {
		  printf("[EE] error in path\n");
		  return (-1); 
                }
	      return 0;
            }
        }
    }
  return 0;
}

/* handle DATA packet */
pkt_t	*dump_receive_DATA(pkt_t *pkt)
{
  int	i;
  int	next_hop_socket;
    
  /* sanity check */
  if (ntohl(pkt->path_len) == 0)
    return (pkt_t *) (-1);

  /* is my id ? */
  if (dump_is_myid(pkt->dst))
    {
#if __DEBUG_DUMP__
      printf("[DUMP] dump_receive_DATA : it's me !! \n");
#endif
      return pkt;
    }
  else
    {
#if __DEBUG_DUMP__
      printf("[DUMP] dump_receive_DATA : it's NOT me !! \n");
#endif
      /* send it to next path's hop */
      for (i = ntohl(pkt->path_len) ; i >= 0; i--)
        {
	  if (dump_is_myid(pkt->path[i]))
            {
	      
	      /* XXX ensure that it is correct */
	      next_hop_socket = dump_lookup_neighbor(pkt->path[i + 1]);
	      
	      if (next_hop_socket == 0)
		{
		  /* ERROR improve that */
#if !defined(ELFSH_INTERN)
		  printf("[EE] error while forwarding packet\n");
#endif
		  return (pkt_t *) (-1);
		}
	      
	      
	      if (dump_send_real(next_hop_socket, pkt) < 0)
		{
#if !defined(ELFSH_INTERN)
		  printf("[EE] error occurs while forwarding DATA packet\n");
#endif
		  return (pkt_t *) (-1);
		}   
	      return NULL;
	    }
	}
    }
  return NULL;
}


/* receive callback */
void	*dump_receive_cb(int s)
{
  pkt_t *pkt;

#if __DEBUG_DUMP__
  printf("[DUMP] dump_receive_cb [%d]\n", s);
#endif

  /* clean recently seen tables */
  dump_clean_recently_seen();
   
  /* read an entire packet */
  pkt = dump_recv_pkt(s);

  if (pkt == (pkt_t *) (-1))
    {
#if __DEBUG_DUMP__
      printf("[DUMP] error -> disconnecting\n");
#endif
      dump_disconnect(s);
      return (void *) (-1);
    }
#if __DEBUG_DUMP__
  printf("[DUMP] ----recv-----\n");  
  dump_print_pkt(pkt);
  printf("[DUMP] -------------\n");
#endif

  /* if packet readed */
  if (pkt != NULL)
    {
      /* determine packet type */
      switch (ntohs(pkt->type))
	{
	case RR:
	  dump_receive_RR(pkt);
	  dump_free(pkt);
	  break;
	case Rr:
	  dump_receive_Rr(pkt);
	  dump_free(pkt);
	  break;
	case DATA:
	  pkt = dump_receive_DATA(pkt);
	  /* need some mem free */
#if !defined(ELFSH_INTERN)
	  if (pkt < 0)
	    {
	      printf("[EE] error while forwaring data\n");
	    }
#endif
	  return pkt;
	  break;
	default:
#if !defined(ELFSH_INTERN)
	  fprintf(stderr, "[EE] Unknown packet type %u \n", ntohs(pkt->type));
#endif
	  /* XXX anything to add ? */
	  exit(-1);
	}
    }
       
  return NULL; 
}


