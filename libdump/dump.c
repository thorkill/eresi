/*
** dump.c for elfsh
** 
** Started on Tue Feb 15 12:51:34 CET 2005 ym
** Updated on Wed Mar  9 22:02:29 CET 2005 ym
** 
**
** $Id: dump.c,v 1.5 2007-08-03 18:05:03 may Exp $
**
*/
#include "libdump.h"

/**************************/
/* libdump init functions */
/**************************/

/* dump initialization */
int	dump_init()
{
  /* hash tables initializations */
  hash_init(&dump_world.ports, "dump_ports", 
	    HASHTABLE_SIZE, ASPECT_TYPE_UNKNOW); 
  hash_init(&dump_world.RRrecently_seen, "dump_RRseen", 
	    HASHTABLE_SIZE, ASPECT_TYPE_UNKNOW); 
  hash_init(&dump_world.Rrrecently_seen, "dump_Rrseen", 
	    HASHTABLE_SIZE, ASPECT_TYPE_UNKNOW); 
  hash_init(&dump_world.myids, "dump_myids", 
	    HASHTABLE_SIZE, ASPECT_TYPE_UNKNOW); 

  /* initialize send wait queue */
  dump_world.send_wq = (pkt_wq_t *) NULL;
    
  /* initialize PRNG */
  srand(time(NULL));
    
  return 0;
}

/***********************/
/* neighbors managment */
/***********************/

/* add a neighbor */
int	dump_add_neighbor(int s, dump_id_t peer_addr)
{
  char	*tmp;
  DEBUGPRINT(printf("[DUMP] dump_add_neighbor : [%d,%s]\n", s,
		    inet_ntoa(peer_addr)));
  tmp = inet_ntoa(peer_addr);

  hash_add(&dump_world.ports, strdup(tmp), (void *) (int) s);
  return 0;
}

/* remove a neighbor */
int	dump_del_neighbor(dump_id_t peer_addr)
{
  hash_del(&dump_world.ports, inet_ntoa(peer_addr));
  return 0;
}

/* return the socket matching the peer_addr */
int	dump_lookup_neighbor(dump_id_t peer_addr)
{  
  return (long) hash_get(&dump_world.ports, inet_ntoa(peer_addr));
}

/*********************************/
/* recently seen table managment */
/*********************************/

/* add RR to RR's recently seen table */ 
int		dump_add_RR_recently_seen(pkt_id_t id)
{
  char		*str;
  time_t	*date; 

  NOPROFILER_IN();

  XALLOC(__FILE__, __FUNCTION__, __LINE__,str, sizeof (char)*20, -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,date, sizeof (time_t), -1);
    
  /* convert id to string */
  snprintf(str, 20-1, "%u", ntohl(id));
    
  /* get date */
  time(date);
   
  /* add it to hash table */
  hash_add(&dump_world.RRrecently_seen, str, (void *) date);
    
  return 0;
}

/* add Rr to Rr's recently seen table */ 
int		dump_add_Rr_recently_seen(pkt_id_t id)
{
  char		*str;
  time_t	*date; 

  NOPROFILER_IN();


  XALLOC(__FILE__, __FUNCTION__, __LINE__,str, sizeof (char)*20, -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,date, sizeof (time_t), -1);
    
  /* convert id to string */
  snprintf(str, 20-1, "%u", ntohl(id));
    
  /* get date */
  time(date);
   
  /* add it to hash table */
  hash_add(&dump_world.Rrrecently_seen, str, (void *) date);
    
  return 0;
}

/* recently seen lookup (in RR table) */
int	dump_lookup_RR_recently_seen(pkt_id_t id)
{
  char	str[20];
    
  snprintf(str, 20-1, "%u", ntohl(id));
    
  if (hash_get(&dump_world.RRrecently_seen, str) != NULL)
    {
      DEBUGPRINT(printf("[DUMP] DUP(RR)!!\n"));
      return 1;
    }
  else
    return 0;
}

/* recently seen lookup (in Rr table) */
int	dump_lookup_Rr_recently_seen(pkt_id_t id)
{
  char	str[20];

  snprintf(str, 20-1, "%u", ntohl(id));

  if (hash_get(&dump_world.RRrecently_seen, str) != NULL)
    {
      DEBUGPRINT(printf("[DUMP] DUP(Rr)!!\n"));
      return 1;
    }
  else
    return 0;
}

/* clean recently seen table */
int		dump_clean_recently_seen()
{
  listent_t	*actual;
  time_t	current;
  int		index;

  /* get current date */
  time(&current);
 
  for (index = 0; index < dump_world.RRrecently_seen.size; index++)
    for (actual = &dump_world.RRrecently_seen.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {   
	if (difftime(current, *(((time_t *) actual->data))) > DUMP_RS_LIMIT)
	  {
	    hash_del(&dump_world.RRrecently_seen, actual->key);
	  }
      }   

  for (index = 0; index < dump_world.Rrrecently_seen.size; index++)
    for (actual = &dump_world.Rrrecently_seen.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	if (difftime(current, *(((time_t *) actual->data))) > DUMP_RS_LIMIT)
	  {
	    hash_del(&dump_world.Rrrecently_seen, actual->key);
	  }
      }

  return 0;
}

/*****************************/
/* send wait queue managment */
/*****************************/

/* add a packet ready to be send */
int		dump_add_send_queue(pkt_t *pkt)
{
  pkt_wq_t	*tmp = dump_world.send_wq;
  pkt_wq_t	*new;

  NOPROFILER_IN();
  DEBUGPRINT(printf("[DUMP] dump_add_send_queue \n"));

  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof (pkt_wq_t), -1);

  new->next = NULL;
  new->pkt = pkt;

  DEBUGPRINT(printf("[DUMP] dump_add_send_queue : "));
  if (tmp != NULL)
    {  
      while (tmp->next != NULL)
	{
	  DEBUGPRINT(printf("."));
	  tmp = tmp->next;
	}
      tmp->next = new;
    }
  else
    dump_world.send_wq = new;
  DEBUGPRINT(printf("\n"));
  return 0;
}

/* search for a packet waiting for given route */
pkt_t		*dump_lookup_send_queue(dump_id_t dst)
{
  pkt_wq_t	*tmp = dump_world.send_wq;

  DEBUGPRINT(printf("[DUMP] dump_lookup_send_queue\n"));    
  if (tmp == NULL)
    {
      DEBUGPRINT(printf("[DUMP] dump_lookup_send_queue -> nothing to send\n"));
      return NULL;
    }

  for (; tmp != NULL; tmp = tmp->next)
    {
      DEBUGPRINT(printf("[DUMP] dump_lookup_send_queue "
			"-> proposed dst %s :\n", inet_ntoa(dst)));
      DEBUGPRINT(printf("[DUMP] dump_lookup_send_queue "
			"-> packet dst %s :\n", inet_ntoa(tmp->pkt->dst)));
      if (memcmp(&(tmp->pkt->dst), &dst, sizeof (dump_id_t))==0)
	return tmp->pkt;
    }
  DEBUGPRINT(printf("[DUMP] dump_lookup_send_queue "
		    "-> nothing matching this destination \n"));
  return NULL;
}

/* remove a packet waiting for given route */
int		dump_del_send_queue(pkt_t *pkt)
{
  pkt_wq_t	*tmp  = dump_world.send_wq;
  pkt_wq_t	*prev = dump_world.send_wq;
  /*
   * DO NOT free pkt->path ...
   */
  DEBUGPRINT(printf("[DUMP] dump_del_send_queue\n"));    
  for (; tmp != NULL; prev = tmp, tmp = tmp->next)
    {
      if (tmp->pkt == pkt)
	{
	  if (prev == dump_world.send_wq)
	    dump_world.send_wq = tmp->next;
	  else 
	    prev->next = tmp->next;
	    
	  XFREE(__FILE__, __FUNCTION__, __LINE__,tmp);
	  /* XXX to improve : pkt never freed */
	  return 0;
	}
    }

  return 0;
}

/************/
/* Various  */
/************/

/* return a timestamp */
time_t	dump_mk_timestamp()
{
  return time(NULL);
}

/* return a random pkt id */
pkt_id_t dump_mk_pkt_id()
{
  /* XXX to improve */
  return htonl(rand());
}

/* free a pkt */
int	dump_free(pkt_t *pkt)
{
  if (pkt != NULL)
    {
      if (pkt->path != NULL)
	XFREE(__FILE__, __FUNCTION__, __LINE__,pkt->path);
      if (pkt->data != NULL)
	XFREE(__FILE__, __FUNCTION__, __LINE__,pkt->data);
      XFREE(__FILE__, __FUNCTION__, __LINE__,pkt);
    }
  return 0;
}

/* print the given packet */
int	dump_print_pkt(pkt_t *pkt)
{
  int	i;
    
  if (pkt == NULL && pkt != (pkt_t *) (-1))
    {
      printf("|  null packet\n");
      return 0;
    }
    
  printf("|  src : %s\n", inet_ntoa(pkt->src));
  printf("|  dst : %s\n", inet_ntoa(pkt->dst));
  printf("|  id : %u\n",  ntohl(pkt->id));
  printf("|  type : %s\n", 
	 pkt->type ==htons(RR)?
	 "RR":(pkt->type == htons(Rr)?
	       "Rr":(pkt->type == htons(DATA)?
		     "DATA":"unknown")));
  printf ("|  size : %u \n",     ntohl(pkt->size));
  printf ("|  path_len : %u \n", ntohl(pkt->path_len));
  printf ("|  path : \n");
  for (i = 0; i < ntohl(pkt->path_len); i++)
    {
      printf("|  -> %s\n", inet_ntoa(pkt->path[i]));
    }
  return 0;
}

/************/
/* myid mgt */
/************/

/* is my id ? */
int		dump_is_myid(dump_id_t id) 
{
  listent_t	*actual;
  int		index;

  for (index = 0; index < dump_world.myids.size; index++)
    for (actual = &dump_world.myids.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	if (!strcmp(actual->data, inet_ntoa(id)))
	  {
	    DEBUGPRINT(printf("[DUMP] dump_is_myid : IT'S ME\n"));
	    return 1;
	  }
      }
  return 0;
}

/* return a node id */
dump_id_t	dump_get_myid(int s)
{
  dump_id_t	ret;
  char		tmp[5];
  char		*tmp2;

  listent_t	*actual;
  int		index;

  /* special case ... */
  if (s == 0)
    {

      for (index = 0; index < dump_world.myids.size; index++)
	for (actual = &dump_world.myids.ent[index];
	     actual != NULL && actual->key != NULL;
	     actual = actual->next)
	  {
	    DEBUGPRINT(printf("[DUMP] dump_get_myid : default id : %s \n",
			      (char *) actual->data));
	    inet_aton((char *)actual->data, &ret);
	    return ret;
	  } 
    }

  
  snprintf(tmp, 5-1, "%d", s);

  tmp2 = (char *) hash_get(&dump_world.myids, tmp); 
  
  if (tmp2 == NULL)
    {
      DEBUGPRINT(printf("[DUMP] dump_get_myid : id not found\n"));
      ret.s_addr = htonl(INADDR_ANY);
      return ret;
    }

  inet_aton(tmp2, &ret);
  DEBUGPRINT(printf("[DUMP] dump_get_myid : using id -> %s\n", tmp2));
  return ret;
}


/* add an id to myids hash table */
int	dump_add_myid(dump_id_t id, int s)
{
  char	tmp[5];
  
  snprintf(tmp, 5-1, "%d", s);
  
  hash_add(&dump_world.myids, strdup(tmp), (void *) strdup(inet_ntoa(id)));
  return 0;
}


/* del an id from myids hash table */
int	dump_del_myid(int s)
{
  char	tmp[5];

  snprintf(tmp, 5-1, "%d", s);
  
  hash_del(&dump_world.myids, tmp);
  
  return 0;
}

/******************/
/* connection mgt */
/******************/

/* disconnect from given peer */
int		dump_disconnect(int s)
{
  listent_t	*actual;
  int		index;

  /* special case ... */
  for (index = 0; index < dump_world.ports.size; index++)
    for (actual = &dump_world.ports.ent[index];
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	if ((long) actual->data == s)
	  {
	    DEBUGPRINT(printf("[DUMP] dump_disconnect from : %s", actual->key));
	    dump_del_myid(s);

	    hash_del(&dump_world.ports, actual->key);
	    
	    close(s);
	    
	    return 0;
	  }
      } 
  DEBUGPRINT(printf("[DUMP] dump_disconnect : "
		    "not connected on socket %d\n", s));
  return (-1);
}

/* disconnect from given peer */
int			dump_disconnect_host(char *host)
{
  struct hostent        *h;
  struct sockaddr_in    serv_addr;

  h = gethostbyname(host);

  if (h == NULL)
    {
#if !defined(ERESI_INTERNAL)
      printf("[EE] Unknown host '%s'\n", host);
#endif
      return (-1);
    }

  memcpy((char *) &serv_addr.sin_addr.s_addr,
	 h->h_addr_list[0],
	 h->h_length);

  return dump_disconnect(dump_lookup_neighbor(serv_addr.sin_addr));  
}




/* connect to given host 
   returns socket's fd in case of success,
   0 otherwise.
*/
int			dump_connect_to(char *host, unsigned int port)
{
  int                   sd, rc;
  struct hostent        *h;
  struct sockaddr_in    local_addr;
  struct sockaddr_in    serv_addr;
  struct sockaddr_in    loc;
  socklen_t             lloc = sizeof (struct sockaddr_in);

  h = gethostbyname(host);

  if (h == NULL)
    {
#if !defined(ERESI_INTERNAL)
      printf("[EE] Unknown host '%s'\n", host);
#endif
      return 0;
    }
  
  serv_addr.sin_family = h->h_addrtype;
  memcpy((char *) &serv_addr.sin_addr.s_addr,
	 h->h_addr_list[0],
	 h->h_length);

  serv_addr.sin_port = htons(port);

  if (dump_lookup_neighbor(serv_addr.sin_addr) != 0)
    {
#if !defined(ERESI_INTERNAL)
      printf("[EE] Already connected to %s\n", host);
#endif
      return -1;
    }

  if (dump_is_myid(serv_addr.sin_addr) == 1 )
    {
#if !defined(ERESI_INTERNAL)
      printf("[EE] Attempt to connect to local node\n");
#endif
      return -1;
    }

  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0)
    {
#if !defined(ERESI_INTERNAL)
      perror("cannot open socket ");
#endif
      return -1;
    }

  /* bind */
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  local_addr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr *) &local_addr, sizeof (local_addr));
  if (rc < 0)
    {
#if !defined(ERESI_INTERNAL)
      printf("[EE] Cannot bind port TCP %u\n", port);
      perror("error ");
#endif
      return -1;
    }

  /* connect to server */
  rc = connect(sd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
  if (rc < 0)
    {
#if !defined(ERESI_INTERNAL)
      perror("[EE] Cannot connect ");
      return -1;
#endif
    }

  /* add a neighbor */
  dump_add_neighbor(sd, serv_addr.sin_addr);

  /* add a new id (if not already registred) */
  /* get local id */
  getsockname(sd, (struct sockaddr *) &loc, &lloc);
  DEBUGPRINT(printf("[+] local id : %s \n",
		    inet_ntoa(((struct sockaddr_in *) &loc)->sin_addr)));

  DEBUGPRINT(printf("[+] add new id : %s\n",
		    inet_ntoa(((struct sockaddr_in *) &loc)->sin_addr)));
  dump_add_myid(((struct sockaddr_in *) &loc)->sin_addr, sd);
  return sd;
}
