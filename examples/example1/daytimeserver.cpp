/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        filename :  daytimeserver.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
*********************************************************************/

//BUILD:
//g++ -DOS_LINUX -I../.. -L../.. -W -Wall -g -o daytime1 daytimeserver.cpp -lssobjects

#include <stdio.h>
#include <time.h>
#include <socketinstance.h>

using namespace ssobjects;

#define MAXLINE     4096
#define DEF_PORT    3333

int
main(int argc,char* argv[])
{
  SocketInstance sockListen,sockClient;
  SockAddr saClient;
  time_t ticks;
  char buff[MAXLINE];
  USHORT wPortNum = DEF_PORT;

  if(argc>1)
    wPortNum=atoi(argv[1]);

  try
  {
    sockListen.create();
    sockListen.bind(SockAddr(INADDR_ANY,wPortNum));
    sockListen.listen();

    printf("server listening on port %u\n",wPortNum);
    for(;;)
    {
      sockListen.accept(sockClient,saClient);
      printf("Got a connection on socket %d\n",(int)sockClient);

      ticks = time(NULL);
      snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
      sockClient.write(buff,strlen(buff));
      sockClient.close();
      puts("connection closed");
    }
  }
  catch(SocketInstanceException& e)
  {
    printf("Error %s\n",e.getErrorMsg());
    sockClient.cleanup();
    sockListen.cleanup();
  }

  return 0;
}
