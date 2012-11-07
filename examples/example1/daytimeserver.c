/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        filename :  daytimeserver.c
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
*********************************************************************/

//BUILD:
//gcc -o daytime2 daytimeserver.c
  
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
  
#define SA struct sockaddr
#define MAXLINE     4096
#define DEF_PORT    3333

void
err_quit(const char* pszMsg)
{
  puts(pszMsg);
  exit(0);
}

int
main(int argc, char* argv[])
{
  int     listenfd,connfd;
  struct  sockaddr_in servaddr;
  char    buff[MAXLINE];
  time_t  ticks;
  unsigned short wPortNum = DEF_PORT;

  if(argc>1)
    wPortNum=atoi(argv[1]);
    
  if(-1==(listenfd = socket(AF_INET,SOCK_STREAM,0)))
    err_quit("socket error");
    
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(wPortNum);

  if(-1==bind(listenfd,(SA*)&servaddr,sizeof(servaddr)))
    err_quit("bind error");
  if(-1==listen(listenfd,5))
    err_quit("listen error");

  printf("server listening on port %u\n",wPortNum);
  for(;;)
  {
    connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd)
      err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);

    ticks = time(NULL);
    snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
    write(connfd,buff,strlen(buff));
    close(connfd);
    puts("connection closed");
  }
}
