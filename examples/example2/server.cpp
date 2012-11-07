/********************************************************************

      Copyright (c) 2006, Lee Patterson & Ant Works Software
      http://ssobjects.sourceforge.net

      date     :  July 4, 2001
      author   :  Lee Patterson (workerant@users.sourceforge.net)

  Demonstraight how simple it is to make a server
  using sssobjects classes.

  This server simplly waits for a ping message and
  then sends a pong back. Any thing else is just
  tossed out. This should not be confused with 
  the ping command that uses ICMP messages. This is 
  more of a ping like that used in an IRC server. 
  It's just a small message that is send, and another 
  that is sent back as a pong (reply).

  This source also includes a client client.cpp
  to test with this server.

  Usage: pingserver [options]

    -h,--help              = Usage
    -p,--port <port>       = Run server on port <port>
    -d,--daemonize         = Daemonize server (not available in win32)

*********************************************************************/

#include <stdio.h>
#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif 

#include <simpleserver.h>
#include <tsleep.h>

#define TITLE "Ping Server"
#define SERVER_VERSION "1.0"

void help(const char* pszCommand);
void invalidParam(const char* pszCommand,const char* pszBadParam);
void initDaemon();

using namespace ssobjects;

/** 
    The most simplest server there is. 
**/
class PingServer : public SimpleServer
{
  public:
    enum {FREQUENCY = 5000};  // controls how often idle will be called
    PingServer(SockAddr& saBind) : SimpleServer(saBind,FREQUENCY){};
    StopWatch m_timer;

  protected:
    // This is the only method you must impliment
    virtual void processSingleMsg(PacketMessage* pmsg)    
    {   
      ServerSocket* psocket = (ServerSocket*)pmsg->socket();
      PacketBuffer* ppacket = pmsg->packet();
      if(ppacket->getCmd() == PacketBuffer::pcPing)
      {
        logs::logln("Got ping, sending pong");
        sendPacket(psocket,PacketBuffer(PacketBuffer::pcPong));
      }
      else if(ppacket->getCmd() == PacketBuffer::pcLogin)
      {
        logs::logln("got login packet");
        char name[100];
        char pass[100];
        *ppacket >> name;
        *ppacket >> pass;
        if(strcmp(name,"someusername")==0 && strcmp(pass,"somepassword")==0)
        {
            logs::logln("Got login %s:%s",(CONSTCHAR*)name,(CONSTCHAR*)pass);
            PacketBuffer loginReply(PacketBuffer::pcLoginOkay);
            loginReply << "Lee";            //users first name
            loginReply << "Patterson";      //users last name
            loginReply << (unsigned32)4321;           //user id
            sendPacket(psocket,loginReply);
        }
        else
        {
            PacketBuffer loginReply(PacketBuffer::pcLoginBad);
            loginReply << "Invalid username or password.";
            sendPacket(psocket, loginReply);
        }
      }
      DELETE_NULL(ppacket);
    }

    // SimpleServer calls this every FREQUENCY milliseconds.
    // This is optional, you don't need to impliment it.
    virtual void idle(unsigned32)
    {
    	//simulate taking a long time to do something. Idle should still be called every FREQUENCY millisecionds.
      logs::logln("Timer from ping server, elapsed time %u",m_timer.milliseconds());
      m_timer.start();
    	tsleep(300);
    }
};


int
main(int argc,char* argv[])
{
	unsigned16 wPort = 9999;
  bool bDaemonize = false;

  puts(TITLE" Version "SERVER_VERSION); // a cute way to combine 3 strings 
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
    else if(!strcmp(a,"-d") || !strcmp(a,"--daemonize"))
    {
      bDaemonize=true;
    }
  }

  // Note that this SockAddr is okay here, as we are not specifying an ip address.
  // You should first construct the server before using SockAddr 
  // like "SockAddr("www.something.com",80)".
  SockAddr saBind((ULONG)INADDR_ANY,wPort);
  try
  {
    // All server output is routed through logs object. That way we can have output
    // sent to a file, console or both. Especially useful when you are running
    // in daemon mode, when you don't want any output on the console.
#ifndef _WIN32      
    if(bDaemonize)
    {
      // Enable log file if we are running as a daemon.
      // Output goes to a file and to console by default.
      // Filename will be "pingserer-MMDD.log".
      logs::init("pingserver");
      logs::enable(true);
    }
#endif

    // In win32 it's important to construct the server BEFORE calling canBind, or any
    // other socket operation, as the server constructor contains a call to WSAStartup,
    // the required call to initialize windows socket layer.
    // Linux is okay either way.
    PingServer server(saBind);          
    if(!SimpleServer::canBind(saBind))  // check if we can bind to this port
      throwGeneralException("Can't bind");       // should not throw from main after server constructed
    else
    {

      logs::log("Server on port %d\n",wPort);
      logs::log("Run 'pingclient' in another terminal to connect and test.\n");

#ifndef _WIN32
      if(bDaemonize)
      {
        logs::logln("Switching to daemon process...");
        logs::set(logs::L_FILE,logs::L_ALL);  // output only log to a file now
        initDaemon();
      }
#endif            
      server.startServer();               // server will now listen for connections

      logs::logln("server is finished.");
    }
  }
  catch(GeneralException& e)
  {
    // all errors use exceptions instead of return codes
    LOG("caught exception [%s]",e.getErrorMsg());
  }
}


void
help(const char* pszCommand)
{
  printf("\nUsage: %s [options] \n",pszCommand);
  puts  (" -h,--help              = this message");
  puts  (" -p,--port <port>       = run server on port <port>");
  puts  (" -d,--daemonize         = daemonize server (not available in win32)");
  exit(0);
}


void
invalidParam(const char* pszCommand,const char* pszBadParam)
{
  printf("\n\"%s\" is invalid.\n",pszBadParam);
  help(pszCommand);
}


#ifndef _WIN32

//From page 336 of 
//Unix Network Programming Network API's: Sockets and XTI by
//W. Richard Stevens:
//
//The purpose of the second fork is to guarantee that the daemon cannnot
//automatically aquire a controlling terminal should it open a terminal
//device in the future. Under SVR4, when a session leader without a
//controlling terminal opens a terminal device (that is not currently
//some other session's controlling terminal), the terminal becomes the controlling 
///terminal of the session leader. But by calling fork a second time,
//we guarantee that the second child is no longer a session leader, so it
//cannot acquire a contolling terminal. We must ignore SIGHUP because
//when a session leader terminates (the first child), all processes in the 
//session (our second child) are sent the SIGHUP signal. 

void 
initDaemon()
{
  pid_t pid;

//  close(0);
//  close(1);
//  close(2);

  if((pid = fork())!=0)
    exit(0);    //parent terminates

  //1st child continues
  setsid();
  signal(SIGHUP,SIG_IGN);
    
  //see comment above for second for explaination
  if((pid = fork())!=0)
    exit(0);    //1st child terminates

  //2nd child continues
  umask(0);       //clear our file mode creation flag

}

#endif

