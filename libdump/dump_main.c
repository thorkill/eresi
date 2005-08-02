/*
 * dump_main.c for elfsh
 *
 * Started on Tue Feb 20 14:40:34 CET 2005 ym 
 * Updated on Wed Mar  9 22:02:29 CET 2005 ym
 *
 */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include "libdump.h"

#define SERVER_PORT 4445

/* XXX some commands may have to add sockets to this list !!! */
int	sd_list[24];  
int	sd_num = 0;

/* disconnect from given host */
int	disconnect_from (char *host)
{
  struct hostent	*h;
  struct sockaddr_in	serv_addr;
  int			socket;
  int			i;
  
  h = gethostbyname (host);
  
  if (h == NULL)
    {
      printf ("[EE] Unknown host '%s'\n", host);
      return 0;
    }

  memcpy ((char *) &serv_addr.sin_addr.s_addr,
          h->h_addr_list[0],
          h->h_length);

  socket = dump_lookup_neighbor (serv_addr.sin_addr);

  if (socket == 0)
    {
      printf ("[EE] No connection to host '%s'\n", host);
      return 0;
    }
  /*  
      printf ("[+] disconnecting : del_neighbor \n");
      dump_del_neighbor (serv_addr.sin_addr);
      printf ("[+] disconnecting : del_myid \n");
      dump_del_myid (socket);
      printf ("[+] disconnecting : close \n");
      close (socket);
  */
  
  dump_disconnect (socket);


  for (i = 2; i < sd_num; i++)
    {
      if (sd_list[i] == socket)
	sd_list[i] = 0;
      /* XXX dirty */
    } 
  return 0;
}

/* send data to host */
int	send_to (char *host, char *data)
{
  struct hostent	*h;
  struct sockaddr_in	serv_addr;

  h = gethostbyname (host);

  if (h == NULL)
    {
      printf ("[EE] Unknown host '%s'\n", host);
      return 0;
    }
  
  printf ("[+] sending data\n");

  memcpy ((char *) &serv_addr.sin_addr.s_addr,
          h->h_addr_list[0],
          h->h_length);

  return dump_send (serv_addr.sin_addr, data, strlen (data));
}


/* connect to given host */
int	 connect_to (char *host)
{
  int			sd, rc;
  struct hostent	*h;
  struct sockaddr_in	local_addr, serv_addr;
  struct sockaddr_in	loc;
  socklen_t		lloc = sizeof (struct sockaddr_in);

  h = gethostbyname (host);
  
  if (h == NULL)
    {
      printf ("[EE] Unknown host '%s'\n", host);
      return 0;
    }
  
  serv_addr.sin_family = h->h_addrtype;
  memcpy ((char *) &serv_addr.sin_addr.s_addr,
	  h->h_addr_list[0],
	  h->h_length);
  serv_addr.sin_port = htons (SERVER_PORT);
  
  /* create socket */
  sd = socket (AF_INET, SOCK_STREAM, 0);
  if (sd < 0)
    {
      perror ("cannot open socket ");
      exit (1);
    }
  
  /* bind any port number */
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  local_addr.sin_port = htons (0);
  
  rc = bind (sd, (struct sockaddr *) &local_addr, sizeof (local_addr));
  if (rc < 0)
    {
      printf ("[EE] Cannot bind port TCP %u\n", SERVER_PORT);
      perror ("error ");
      exit (1);
    }

  /* connect to server */
  rc = connect (sd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
  if (rc < 0)
    {
      perror ("[EE] Cannot connect ");
      exit (1);
    }

  
  sd_list[sd_num] = sd;
  sd_num ++;

  /* adding a neighbor */
  dump_add_neighbor (sd, serv_addr.sin_addr);

  /* adding a new id (if not already registred) */

  /* get local id */
  getsockname (sd, (struct sockaddr *) &loc, &lloc);

  printf ("[+] local id : %s \n", 
	  inet_ntoa (((struct sockaddr_in *) &loc)->sin_addr));

  printf ("[+] add new id : %s\n", 
	  inet_ntoa (((struct sockaddr_in *) &loc)->sin_addr));
  
  dump_add_myid (((struct sockaddr_in *) &loc)->sin_addr, sd);

  printf ("[+] added\n");
  
  return 0;
}

/* parse command */
void	lhandler (char *c)
{
  char	*w;
  
  if (c != NULL)
    {
      add_history (c);
      //printf ("[+] -> readline : %s\n", c);
      
      w = strtok (c, " ");
      if (w == NULL)
	return;
      
      if (!strcmp ("connect", w))
	{
	  printf ("[+] connect\n");
	  w = strtok (NULL, " ");
	  if (w == NULL)
	    return ;
	  connect_to (w);
	  return ;
	}
      else if (!strcmp ("exit", w))
	{
	  printf ("\n[+] leaving\n");
	  exit (0);
	}
      else if (!strcmp ("quit", w))
        {
          printf ("\n[+] leaving\n");
          exit (0);
        }
      else if (!strcmp ("send", w))
	{
	  w = strtok (NULL, " ");
          if (w == NULL)
            return ;
          send_to (w, strtok (NULL, " "));
	  return ;
	}
      else if (!strcmp ("disconnect", w))
	{
	  w = strtok (NULL, " ");
          if (w == NULL)
            return ;
	  disconnect_from (w);
	  return ;
	}
    }
  else 
    {
      printf ("\n[+] leaving\n");
      exit (0);
    }
}

/* create main socket */
int	main_socket ()
{
  int	main_socket;
  int	one = 1;
  struct sockaddr_in serv_addr;

  main_socket = socket (PF_INET, SOCK_STREAM, 0);

  if (main_socket < 0)
    {
      perror ("[EE] Cannot open socket ");
      return (-1);
    }

  /* bind server port */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons (SERVER_PORT);

  /* REUSEADDR */
  setsockopt (main_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof (one));

  /* bind socket */
  if (bind (main_socket, 
	    (struct sockaddr *) &serv_addr, 
	    sizeof (serv_addr)) < 0)
    {
      perror ("[EE] Cannot bind port ");
      return (-1);
    }
  
  /* listen */
  listen (main_socket, 5);

  printf ("[+] Ready to accept connection on TCP port %u\n", SERVER_PORT);
  
  return main_socket;
}

/* Dump test main loop */
int	main_loop (int main_sd, int input_fd)
{
  int	sd_max = 0;
  int	new_sd;
  int	i;

  struct sockaddr_in	cli_addr;
  int			cli_len;

  int			readsocks;
  struct timeval	timeout;

  void *data;

  struct sockaddr	loc;
  socklen_t		lloc = sizeof (struct sockaddr);


  fd_set socks;

  /* add input fd to socket list */
  sd_list[0] = input_fd;
  sd_num ++;

  /* add main socket to socket list */
  sd_list[1] = main_sd;
  sd_num ++;
  

  /* loops */
  while (1)
    {

      FD_ZERO (&socks);

      /* add sockets to fd_set */
      for (i = 0; i < sd_num; i++)
	{
	  if (i >= 2 && sd_list[i] == 0)
	    continue;		// skip closed connections
	  sd_max = (sd_max<sd_list[i])?sd_list[i]:sd_max;

	  FD_SET (sd_list[i], &socks);
	}

      /* Set timeout value */
      timeout.tv_sec = 100;
      timeout.tv_usec = 0;

      /* select */
      readsocks = select (sd_max + 1, 
			  &socks, 
			  (fd_set *) 0, 
			  (fd_set *) 0, 
			  &timeout);
      
      /* select timeouted */
      if (readsocks == 0)
        {
	  printf ("[EE] Select timeouted \n");
	  for (i = 1; i < sd_num; i++) 
	    close (i);
	  
	  exit (-1);
        }

      /* select failed */
      if (readsocks < 0)
        {
	  printf ("[EE] Select failed \n");
	  perror ("select");
	  for (i = 1; i < sd_num; i++) 
	    close (i);
	  
	  exit (-1);
        }
      
      /* handle select break's on ... */
      /* ... input fd */
      if (FD_ISSET (sd_list[0], &socks))
	{
	  /* call readline callback function */
	  rl_callback_read_char();
	}

      /* ... other sockets */
      for (i = 2; i < sd_num; i++)
	{
	  if (sd_list[i] == 0)
	    continue;		// skip deleted connections
	  if (FD_ISSET (sd_list[i], &socks))
	    {
	      
	      printf ("[+] Calling DUMP recv callback [%d]\n", sd_list[i]);
	      data = dump_receive_cb (sd_list[i]);

	      /* connection closed by libdump */
	      if (data == (void *) (-1))
		{
		  printf (" socket %d considered as close \n", sd_list[i]);
		  sd_list[i] = 0;
		  continue;
		}
	      
	      dump_print_pkt ((pkt_t *) data);
	      if (data != NULL)
		{
		  printf ("[+] packet type : %s\n", 
			  (((pkt_t *) data)->type == htons(RR))?
			  "RR":((((pkt_t *) data)->type ==htons(Rr))?
                  "Rr":((((pkt_t *) data)->type ==htons(DATA))?
				"DATA":"Unknown")));
		  dump_free (data);
		}
	      else
		printf ("[+] null (non-data packet)\n");
	      
	    }
	}

      /* ... main socket */
      if (FD_ISSET (sd_list[1], &socks))
        {
          /* try to accept connection */
          cli_len = sizeof (cli_addr);
          new_sd = accept (sd_list[1], 
			   (struct sockaddr *) &cli_addr, 
			   &cli_len);
          if (new_sd > 0)
            {
              printf ("[+] Connection from : %s\n", 
		      inet_ntoa (cli_addr.sin_addr));
              sd_list[sd_num] = new_sd;
              sd_num++;

              /* getsockname ()*/
              getsockname (new_sd, &loc, &lloc);

	      printf ("[+] add new id \n");
	      dump_add_myid (((struct sockaddr_in *) &loc)->sin_addr, new_sd);

	      printf ("[+] added\n");
	      printf ("[+] my new id : %s \n", 
		      inet_ntoa (dump_get_myid (new_sd)));

              printf ("[+] Adding a DUMP neighbor : [%d,%s]\n", 
		      new_sd, inet_ntoa (cli_addr.sin_addr));
              dump_add_neighbor (new_sd, cli_addr.sin_addr);
            }
        }

      
    }

  return 0;
}



int	 main ()
{

  int	main_sd;
  int	input_fd = 0; // =stdin

  /* create the main socket */
  main_sd = main_socket ();  

  if (main_sd == (-1))
    {
      printf ("[EE] error on main socket\n");
      exit (-1);
    }

  // XXX how to get local id ?

  /* get local id */
  
  /* initialize dump */
  dump_init ("NULL");
  printf ("[+] Dump initialized\n");

  /* install readline handler */
  rl_callback_handler_install ("dump ->", lhandler);

  atexit (rl_callback_handler_remove);
	     
  /* loops for ever */
  main_loop (main_sd, input_fd);

  /* remove readline callback handler */
  rl_callback_handler_remove();

  printf ("[EE] DUMP end ???? \n");
    
  return 0;
}
