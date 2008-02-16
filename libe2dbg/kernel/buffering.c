/**
 * @file libe2dbg/kernel/buffering.c
 *
 *
 */

#include "ke2dbg.h"

int infobuffer; /* =DESACTIVADO; */
int bufferindex=0;
int insertbufferindex=0;
char buff_cmd[BUFFERMAXCMD][80];
int insertcmdindex;
int cmdindex;
char buffering[BUFFERMAXLINES][80];       /* 200 lines and 80 chars */


/**
 * nettoie le buffer de log                        
 */
void ke2dbg_logbuf_clean(void)
{
  int x,y=0;
  char *ptr;
  for (x=0;x<BUFFERMAXLINES;x++)
    {
      ptr=&buffering[x][0];
      for (y=0;y<80;y++)
	{*ptr=' ';ptr++;};
           
    }
}


/**
 * insere une commande dans l historique           
 */
void ke2dbg_command_insert(char *ptr)
{
  insertcmdindex = insertcmdindex % BUFFERMAXCMD;
  strcpy(&buff_cmd[insertcmdindex][0], ptr ); 
  insertcmdindex = (insertcmdindex+1) % BUFFERMAXCMD;
  cmdindex =insertcmdindex;
}

/**
 *
 * @param decal
 */
void ke2dbg_command_recall(int decal)
{
  unsigned int len,i;

  cmdindex = (cmdindex + decal + BUFFERMAXCMD) % BUFFERMAXCMD;	
  cmdindex = cmdindex % BUFFERMAXCMD;

  ke2dbg_command_clear();

  len = ke2dbg_strlen(&buff_cmd[cmdindex][0]);
  len = len>50?50:len;

  for (i=0;i<len;i++)
    ke2dbg_cmdkey_insert(buff_cmd[cmdindex][i]);

}


/**
 * insere une ligne dans la file du buffer de log  
 * @param ptr
 */
void ke2dbg_logbuf_insert(char *ptr)
{
  unsigned int i;
  char buf[]="                                                                                ";


  if (insertbufferindex>=BUFFERMAXLINES) 
    {
      for (i=0;i<BUFFERMAXLINES-1;i++)
	  ke2dbg_memcpy(&buffering[i][0], &buffering[i+1][0], 78);

      insertbufferindex-=1;
     
      for (;i<BUFFERMAXLINES;i++)
	  ke2dbg_memcpy(&buffering[i][0],&buf[0], 78);
     
    }

  strcpy(&buffering[insertbufferindex][0], ptr); 
  insertbufferindex++;
  
  bufferindex = (unsigned int)insertbufferindex<buff_win_dim?0 : insertbufferindex-buff_win_dim;
}


/**
 * affiche le bufer de log dans la fenetre de log  
 *
 */
void ke2dbg_logbuf_print()
{
  unsigned int x;
  //  for (x=buff_win_y; x < buff_win_y + buff_win_dim;x++)
    //ke2dbg_memcpy(&table[x][1], &buffering[bufferindex+x-buff_win_y][0], 78);
  for (x=0; x < buff_win_dim;x++)
    ke2dbg_print_AW("                                                                              ", x);


  for (x=0; x < buff_win_dim;x++)
    ke2dbg_print_AW(&buffering[bufferindex+x][0], x);

}

/**
 * descend d'une ligne dans le buffer de log       
 */
void ke2dbg_logbuf_downline(void) 
{
  ke2dbg_Refresh_AW();
  if ((bufferindex+buff_win_dim)<BUFFERMAXLINES) bufferindex++;

}

/**
 * remonte d'une ligne dans le buffer de log       
 */
void ke2dbg_logbuf_upline(void)
{
  ke2dbg_Refresh_AW();
  if (bufferindex>0) bufferindex--;

}

/**
 * descend de 10 ligne dans le buffer de log       
 */
void ke2dbg_logbuf_downlines(void) 
{
  int i;
  for (i=0;i<10;i++) 
    ke2dbg_logbuf_downline();
}

/**
 * remonte de 10 ligne dans le buffer de log       
 */
void ke2dbg_logbuf_uplines(void)
{
  int i;
  for (i=0;i<10;i++) 
    ke2dbg_logbuf_upline();
}

/**
 *
 */
void ke2dbg_logbuf_start(void)
{
  ke2dbg_ClearAW();
  ke2dbg_logbuf_clean();

  bufferindex=0;
  insertbufferindex=0;
}

/**
 *
 */
void ke2dbg_logbuf_end(void)
{

}

/**
 *
 */
void ke2dbg_logbuf_restore(void)
{

}
