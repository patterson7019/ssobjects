/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        filename :  asyncclientconnector.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Asynchronus client class. This class uses a 
                    non-blocking polling method to check for and 
                    process data.

*********************************************************************/

#include <ctype.h>
#ifndef _WIN32
#   include <unistd.h>
#   include <fcntl.h>
#endif

#include "asyncclientconnector.h"

using namespace ssobjects;

AsyncClientConnector::AsyncClientConnector()
  : m_wPort(0),m_strHost(),m_pSocket(0),m_que()
{
}

/**
   Constructs a AsyncClientConnector object and connects to host
   pointed to by pszHost on port pointed to by wPort.

   \throw AsyncClientConnectorException [Windows only] If Windows WSAStartup failed 
**/
AsyncClientConnector::AsyncClientConnector(const char* pszHost,word wPort)
        : m_wPort(wPort),m_strHost(pszHost),m_pSocket(0),m_que()
{
#ifdef WIN32
	if(WSAStartup(0x0101,&m_wsd)!=0)
		throwAsyncClientConnectorException("unable to start windows socket layer");
#endif
  connect();
}

AsyncClientConnector::~AsyncClientConnector()
{
	cleanup();
#ifdef WIN32
	WSACleanup();
#endif
}

/** 
    Closes down any existing connection, and tries to connect to the server.

    \throw See AsyncClientConnector::connect for details.
**/
void AsyncClientConnector::reconnect()
{
  cleanup();  //make sure old connection is gone
  connect();
}

/**
   Stores the information you pass in, and then tries to connect to that
   server.

   \throw See AsyncClientConnector::connect for details.
**/
void
AsyncClientConnector::connect(const char* pszHost,const unsigned16 wPort)
{
  m_strHost = pszHost;
  m_wPort   = wPort;
  connect();
}

/**
   Attempts to connect to the server. Once connected, the socket 
   is put into non-blocking mode.
   
   \throw SocketInstanceException If unable to make a connection or socket creation 
   failed.
**/
void AsyncClientConnector::connect()
{
  if(m_pSocket)
    throwAsyncClientConnectorException("connection already established");

  try
  {
    SockAddr saServer(m_strHost,m_wPort);
    //TODO: interestingly enough, there is no operator [] in cstr class, yet this
    //TODO works here, but not in redhat. Wierd!?!?!
    if(isalpha(m_strHost[0]))
      saServer = SocketInstance::getHostByName(m_strHost,m_wPort);
    m_pSocket = new ServerSocket();
    m_pSocket->create();
    m_pSocket->connect(saServer);
    setNonBlocking(*m_pSocket);
  }
  catch(SocketInstanceException& e)
  {
    UNUSED_ALWAYS(e);
    if(m_pSocket)
    {
      m_pSocket->cleanup();
      DELETE_NULL(m_pSocket);
    }
    throw;
  }
}

void AsyncClientConnector::cleanup()
{
  if(!m_pSocket)
    return;

  m_pSocket->cleanup();
  DELETE_NULL(m_pSocket);
}

/** 
    \throw AsyncClientConnectorException If socket was not connected.
    \throw SocketInstanceException If there was a socket error during close operation.
**/
void AsyncClientConnector::close()
{
	try
	{
		if(!m_pSocket)
			throwAsyncClientConnectorException("socket was not connected");

		m_pSocket->close();
		DELETE_NULL(m_pSocket);
	}
	catch(SocketInstanceException& e)
	{
    UNUSED_ALWAYS(e);
		m_pSocket->cleanup();
		DELETE_NULL(m_pSocket);
    throw;
	}
}

/**
   You call idle as often as you are able to. When idle is called, it will check
   for incoming data. If there is data on the connected socket, it will be read,
   and parsed into packets. If the connection is closed gracefully on the other
   end, PacketBuffer::pcClosed is posted to the message que. If the connection is
   not closed gracefully, and exeption is generated.

   \throw AsyncClientConnectorException If there was any read or select errors.

   \return  
        true if there was packets parsed. 
        false otherwise.
**/
bool
AsyncClientConnector::idle()
{
  if(!m_pSocket)
    return false;

  fd_set rset,wset,*pwset=NULL;
  FD_ZERO(&rset);
  FD_ZERO(&wset);

  FD_SET(*m_pSocket,&rset);
  if(m_pSocket->getOutBufferSize())
  {
    DLOG("socket has %d bytes to send",m_pSocket->getOutBufferSize());
    FD_ZERO(&wset);
    FD_SET(*m_pSocket,&wset);
    pwset = &wset;  //select will not check for writability
  }

  int iReady;
  struct timeval tv = {0,0}; //don't sleep, just poll

  iReady = select((*m_pSocket)+1,&rset,pwset,NULL,&tv);
  if(!iReady)
    return !m_que.isEmpty();

  if(-1 == iReady)
  {
    CStr msg;
    msg.format("select error %d %s",errno,strerror(errno));
    LOG((const char*)msg);
    throwAsyncClientConnectorException(msg);
  }

  //check for any data
  if(FD_ISSET(*m_pSocket,&rset))
  {
    PacketMessage* pmsg;
    int iBytesRead = m_pSocket->readData();
    if(!iBytesRead)
    {
      //socket was closed
      PacketBuffer* pktRemove = new PacketBuffer(PacketBuffer::pcClosed);
      *pktRemove << (signed32) *m_pSocket;
      *pktRemove << m_pSocket->getAddr().dottedDecimal();
      pktRemove->rewind();
      pmsg = new PacketMessage(m_pSocket,pktRemove);         //socket is no longer valid
      m_que.add(pmsg);
      DELETE_NULL(m_pSocket);
    }
    else
    {
      PacketBuffer* ppacket;
      while((ppacket = m_pSocket->extractPacket()))
      {
        pmsg = new PacketMessage(m_pSocket,ppacket);
        m_que.add(pmsg);
      }       
    }
  }

  if(m_pSocket)
    if(FD_ISSET(*m_pSocket,&wset))
    {
      m_pSocket->sendBuffer();
    }

  return !m_que.isEmpty();         //return false if the que is empty instead of isEmpty()'s true 
}

/**
   Attempts to send the packet pointed to by pPkt.

   Asserts if pPkt is NULL.

   \throw AsyncClientConnectorException If the socket is not connected.
**/
void 
AsyncClientConnector::sendPacket(PacketBuffer* pPkt)
{
  assert(pPkt);

	if(!m_pSocket) 
		throwAsyncClientConnectorException("socket is not connected");
	m_pSocket->sendPacket(pPkt);
}

/**
   Attempts to send the packet referenced by packet.

   \throw AsyncClientConnectorException If the socket is not connected.
**/
void AsyncClientConnector::sendPacket(PacketBuffer& packet)
{
	if(!m_pSocket) 
		throwAsyncClientConnectorException("socket is not connected");
	sendPacket(&packet);
}

/** 
   \throw AsyncClientConnectorException If the socket is not connected.
**/
//send the packet
void AsyncClientConnector::operator << (PacketBuffer& packet)
{
  if(!m_pSocket)
    throwAsyncClientConnectorException("socket is not connected");
  sendPacket(&packet);
}

/** 
   \throw AsyncClientConnectorException If the socket is not connected.
**/
//send a packet pointer;
void AsyncClientConnector::operator << (PacketBuffer* ppacket)
{
  if(!m_pSocket)
    throwAsyncClientConnectorException("socket is not connected");
  sendPacket(ppacket);
}

ServerSocket* AsyncClientConnector::getSocket()
{
  if(!m_pSocket)
    throwAsyncClientConnectorException("socket is not connected");
  return m_pSocket;
}

/**
   Sets the socket to non-blocking mode.
   \throw AsyncClientConnectorException If unable to set socket to non-blocking mode.
   \note [Windows only] When compiled for windows, this is an empty function. 
   Socket is not put into non-blocking mode. 
**/
//TODO: windows needs to set non blocking with ioctlsocket() 
void
AsyncClientConnector::setNonBlocking(SOCKET sockfd)
{
#ifndef _WIN32
  int val;
  int er;
  CStr msg;

  val = fcntl(sockfd,F_GETFL,0);
  er = fcntl(sockfd,F_SETFL,val | O_NONBLOCK);    
  if(-1 == er)
  {
    msg.format("*** ERROR *** %s setting nonblock on socket %d",
               strerror(errno),sockfd);
    LOG((const char*)msg);
    throwAsyncClientConnectorException(msg);
  }
#endif
}

/**
   Gets the next message, and removes it from the message queue.

   \return 
   NULL if there are no messages. 
   A pointer to the next message otherwise.
**/
PacketMessage* 
AsyncClientConnector::getMsg()
{
  //TODO: need a better way to make sure that messages are deleted when 
  //TODO  finished with.
  return m_que.get();
}
