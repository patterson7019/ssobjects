/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  December 17, 2000
       filename :  simplemanager.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
       purpose  :  See header file

*********************************************************************/

//system includes

//library includes
#include "cstr.h"
#include "timeval.h"
#include "tsleep.h"
#include "stopwatch.h"

#include "simplemanager.h"

using namespace ssobjects;

SimpleManager::SimpleManager(    
  const SockAddr& saBind,
  const bool bUseThreads,
  const unsigned32 nFreq,
  const unsigned32 nMaxConnections)
  : m_listSimpleServers(),
    m_listClientSockets(),
    m_sListen(),
    m_saBind(saBind),
    m_nIdleFrequency(nFreq),
    m_nMaxConnections(nMaxConnections),
    m_bUsingThreads(bUseThreads),
    m_bPause(false)
    //    m_serverStartTime(),
    //    m_serverStartDate()
{
#ifdef _WIN32
  WSADATA wsd;
  if(WSAStartup(0x0101,&wsd)!=0)
    throwGeneralException("unable to start windows socket layer");
#endif
  if(m_nIdleFrequency < SimpleServer::MIN_FREQ)
  {
    CStr msg;
    msg.format("Server frequency of %d is too fine. Minimum is %d.",nFreq,SimpleServer::MIN_FREQ);
    //TODO: should throw a simple manager exception
    throwGeneralException(msg);
  }
}

SimpleManager::~SimpleManager()
{
  //TODO: check for memory leaks
  m_listSimpleServers.purge();
  m_listClientSockets.purge();
}

void
SimpleManager::startServer()
{
  if(!isThreaded())
  {
    //single threaded servers should override startServer
    //and create your simple server dirived object, and call
    //startServer() from your overidden startServer

    assert(isThreaded()); //see comments above
    throwGeneralException("single threaded implimentations need to override startServer method");
  }

  //Start a threaded server. Simple servers are now treated as 
  //objects that handle one connection at a time. The functionality
  //of listening for connections is turned off, as all listening
  //is handled as a threaded server object.

  m_sListen.create();
  m_sListen.bind(m_saBind);
  m_sListen.listen();
  start();
}

threadReturn
SimpleManager::ThreadHandlerProc(void)
{
  TimeVal     timeout(0,idleFrequency());
  StopWatch   timer;
  int         iReady;
  fd_set      rset;

  idle();
  timer.start();
  while(running())
  {
    int fd = m_sListen;
    FD_ZERO(&rset);
    FD_SET(fd,&rset);

    iReady = select(fd+1,&rset,NULL,NULL,&timeout);

    //make sure we are still running since we've been asleep in the select call
    if(running())   
    {
      if(-1 == iReady)
      {
        //TODO: should throw a simple manager exception
        throwGeneralException("Select returned error");
      }

      if(iReady > 0)
        acceptConnection();

      if(timer.milliseconds() >= (idleFrequency()-SimpleServer::MIN_FREQ) && running())     //20ms error range as a timeout isn't percise
        idle();

      if(!iReady)
      {
        timeout.reset();
        timer.start();
      }
    }
  }
  return 0;
}

void
SimpleManager::acceptConnection()
{
  SocketInstance sClient;
  SockAddr saClient;
  m_sListen.accept(sClient,saClient);

  ServerSocket*   psocket = new ServerSocket(sClient,saClient,8000,120);
  SimpleServer* pserver = createServerInstance(psocket);

  m_listSimpleServers.addTail(pserver);       //keep for later clean up
  m_listClientSockets.addTail(psocket);       //keep for later clean up
  pserver->start();      //start the thread

  PacketBuffer* pktNew = new PacketBuffer(PacketBuffer::pcNewConnection);
  PacketMessage*    pmsg   = new PacketMessage(psocket,pktNew);
  pserver->addMsg(pmsg);
}

//
//keep sleeping till the this manager is finished
//
void
SimpleManager::waitFinish()
{
  //TODO: should use an event and have the event signaled when 
  //TODO  server is finished.
  while(running())
    tsleep(1000);
}
