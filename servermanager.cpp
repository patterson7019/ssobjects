/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       filename :  servermanager.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#include "servermanager.h"
#include "cstr.h"

ServerManager::ServerManager(    
  const SockAddr& saBind,
  const unsigned32 nFreq,
  const unsigned32 nMaxConnections,
  const bool bUseThread)
  : m_sListen(),
    m_saServer(saBind),
    m_listSimpleServers(),
    m_nIdleFrequency(nFreq),
    m_nMaxConnections(nMaxConnections),
    m_bUsingThread(bUseThread),
    m_bPause(false),
    m_serverStartTime(),
    m_serverStartDate()
{
#ifdef WIN32
  WSADATA wsd;
  if(WSAStartup(0x0101,&wsd)!=0)
    throwGeneralException("unable to start windows socket layer");
#endif
  if(m_nIdleFrequency < MIN_FREQ)
  {
    CStr msg;
    msg.format("Server frequency of %d is too fine. Minimum is %d.",nFreq,MIN_FREQ);
    throwSimpleServerException(msg);
  }
}

void
ServerManager::startServer()
{
  if(!bUseThread)
  {
    //start a single threaded simple server
    SimpleServer server(saBind,nFreq,nMaxCon);
    server.startServer(false);
  }
  else
  {
    //Start a threaded server. Simple servers are now treated as 
    //objects that handle one connection at a time. The functionality
    //of listening for connections is turned off, as all listening
    //is handled in this threaded server object.

    m_sListen.create();
    m_sListen.bind(m_saServer);
    m_sListen.listen();
    start();
  }
}

bool
ServerManager::canBind()
{
  try
  {
    m_sListen.create();
    m_sListen.bind(m_saServer);
    m_sListen.listen();
    m_sListen.close();
  }
  catch(GeneralException& e)
  {
    UNUSED_ALWAYS(e);
    return false;
  }
  return true;
}

threadReturn
ServerManager::ThreadHandlerProc(void)
{
  TimeVal timeout(0,500);
  int iReady;
  fd_set rset;

  FD_ZERO(&rset);
  FD_SET(&rset,m_sListen);

  while(running())
  {
    timeout.reset();
    iReady = select(m_sListen+1,&rset,NULL,NULL,&timeout);

    //make sure we are still running after being asleep in select call
    if(running())   
    {
      if(-1 == iReady)
      {
        //TODO: should throw a threaded server exception
        throwGeneralException("Select returned error");
      }

      if(iReady > 0)
        acceptConnection(m_sListen);
    }
  }
}

void
ServerManager::acceptConnection(SocketInstance& sListen)
{
  ServerSocket* pServSocket;
  SocketInstance sClient;
  SockAddr saClient;

  sListen.accept(sClient,saClient);
  pServSocket = new ServerSocket(sClient,saClient,8000,120);
  pServer = SimpleServer::createServer();
//    pSimpleServer = new SimpleServer(pServSocket
//    m_listClients.addTail(pServSocket);

  PacketBuffer* pktNew = new PacketBuffer(PacketBuffer::pcNewConnection);
  PacketMessage* pmsg = new PacketMessage(pServSocket,pktNew);
  m_que.add(pmsg);
}
