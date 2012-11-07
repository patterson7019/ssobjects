/********************************************************************

      Copyright (c) 2006, Lee Patterson & Ant Works Software
      http://ssobjects.sourceforge.net

      date     :  July 4, 2001
      author   :  Lee Patterson (workerant@users.sourceforge.net)

  Demonstraight how simple it is to make a server
  using ssobjects classes.

  This server simple waits for a ping message and
  then sends a pong back. Any thing else is just
  tossed out.

  This source also includes a server (server.cpp)
  to test this client.

  Usage: pingclient [options]

    -h,--help              = Usage
    -p,--port <port>       = Run server on port <port>
    -s,--server <hostname> = Connect to server at <hostname> as client.
                             Can be an ip addr.

*********************************************************************/

#include <stdio.h>
#include <clientconnector.h>

using namespace ssobjects;

void help(const char* pszCommand);
void invalidParam(const char* pszCommand,const char* pszBadParam);

int
main(int argc,char* argv[])
{
  char szHost[255] = "localhost";
  unsigned16 wPort = 9999;
  int count = 1;
  
  puts("Ping Client");
  puts("Copyright (c) 2001, Lee Patterson & Ant Works Software");
  puts("http://ssobjects.sourceforge.net");
  puts("Author: Lee Patterson");

  // Parse command line
  for(int i=1; i<argc; i++)
  {
    char* a = argv[i];
    if(!strcmp(a,"-h") || !strcmp(a,"--help") || !strcmp(a,"/?"))
    {
      help(argv[0]);
    }
    else if(!strcmp(a,"-p") || !strcmp(a,"--port"))
    {
      if(++i>=argc) invalidParam(argv[0],a);
      wPort = atoi(argv[i]);
    }
    else if(!strcmp(a,"-s") || !strcmp(a,"--server"))
    {
      if(++i>=argc) invalidParam(argv[0],a);
      strncpy(szHost,argv[i],sizeof szHost);   //always make sure you don't overflow your storage
      NULL_TERMINATE(szHost,sizeof szHost);   //make sure that the string was null terminated if strncpy truncated. 
    }
    else if(!strcmp(a,"-c") || !strcmp(a,"--count"))
    {
      if(++i>=argc) invalidParam(argv[0],a);
      count = atoi(argv[i]);
    }
  }

  //run the client
  try
  {
    puts("RUNNING AS CLIENT");
    printf("Connecting to ping server at %s:%d\n",szHost,wPort);
    ClientConnector connection(szHost,wPort);
    for(int i=0; i<count; i++) 
    {
      PacketBuffer ping(PacketBuffer::pcPing);
      PacketBuffer pong;
      puts("Sending ping...");
      connection << ping;
      puts("Ping sent. Waiting for pong...");
      connection >> pong;
      assert(pong.getCmd() == PacketBuffer::pcPong);
      puts("Got pong");
		}
    puts("Success");
  }
  catch(GeneralException& e)
  {
    printf("%s error\n\n",e.getErrorMsg());
  }

  return 0;
}


void
help(const char* pszCommand)
{
  printf("\nUsage: %s [options] \n",pszCommand);
  puts  (" -h,--help              = this message");
  puts  (" -p,--port <port>       = run server on port <port>");
  puts  (" -s,--server <hostname> = connect to server at <hostname> as client.");
  puts  ("                          Can be an ip addr.");
  exit(0);
}


void
invalidParam(const char* pszCommand,const char* pszBadParam)
{
  printf("\n\"%s\" is invalid.\n",pszBadParam);
  help(pszCommand);
}




