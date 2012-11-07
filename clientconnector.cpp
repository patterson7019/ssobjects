/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        filename :  clientconnector.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  See header file.
*********************************************************************/


#include <ctype.h>
#include "clientconnector.h"

using namespace ssobjects;

#ifdef _WIN32
bool ClientConnector::m_bWSAStarted = false;
#endif

/**
   Default constructor. 
   [Windows only] If ClientConnector hasn't yet called WSAStartup, it will be called now.

   \throw ClientConnectorException [Windows only] If WSAStartup failed.

   \todo Instead of using a flag, a reference count should be used so if there
   are multiple instances of ClientConnector, the windows socket layer won't be closed
   down.
**/
ClientConnector::ClientConnector(const unsigned32 bufferSize)
    : m_nBufferSize(bufferSize),m_wPort(0),m_strHost(),m_pSocket(0)
{
#ifdef _WIN32
  if(!m_bWSAStarted)
  {
    LOG("wsastartup");
    if(WSAStartup(0x0101,&m_wsd)!=0)
      throwClientConnectorException("unable to start windows socket layer");
    m_bWSAStarted = true;
  }
#endif
}

/**
   Constructs a ClientConnector object and connects to the host.
   [Win32] If the WSAStartup hasn't been called, it will be called at this
   point. Upon destruction, WSACleanup will be called.

   \param pszHost [in] Describe what host to connect to.
   \param wPort   [in] Describe what port to connect to on pszHost

   \note [Win32] Currently, if you create and use more then one at the same
   time, ClientConnector will call WSACleanup when the first ClientConnector goes out of
   scape. If you try socket operations with the section, it will fail.

   \throw ClientConnectorException           If Windows WSAStartup failed (Windows only).
   \throw SocketInstanceException   If there is a socket or connect error.
**/
ClientConnector::ClientConnector(const char* pszHost,word wPort,const unsigned32 bufferSize)
    : m_nBufferSize(bufferSize),m_wPort(wPort),m_strHost(pszHost),m_pSocket(0)
{
#ifdef _WIN32
  if(!m_bWSAStarted)
  {
    LOG("wsastartup");
    if(WSAStartup(0x0101,&m_wsd)!=0)
      throwClientConnectorException("unable to start windows socket layer");
    m_bWSAStarted = true;
  }
#endif
  connect();
}

/**
   Closes connection.

   [Win32] Also calls WSACleanup. 

   \note [Win32] Currently, if you create and use more then one at the same
   time, ClientConnector will call WSACleanup when the first ClientConnector goes out of
   scape. If you try socket operations with the section, it will fail.
**/
ClientConnector::~ClientConnector()
{
	cleanup();
#ifdef _WIN32
  if(m_bWSAStarted)
  {
    LOG("wsacleanup");
    WSACleanup();
    m_bWSAStarted=false;
  }
#endif
}

/** 
    Closes down any existing connection, and tries to connect to the server by
    calling connect() method.

    \throw See ClientConnector::connect for details.
**/
void ClientConnector::reconnect()
{
  cleanup();  //make sure old connection is gone
  connect();
}

/**
   \param szHost  [in] Describe what host to connect to.
   \param wPort   [in] Describe what port to connect to on szHost.

   Stores the information you pass in, and then tries to connect to that
   server.

   \throw See ClientConnector::connect for details.
**/
void
ClientConnector::connect(const char* szHost,const unsigned wPort)
{
  m_wPort = wPort;
  m_strHost = szHost;
  connect();
}

/**
   Attempts to connect to the server. 

   [Win32] If ClientConnector hasn't yet called WSAStartup, it will be called now.
   
   \throw SocketInstanceException If unable to make a connection or socket creation failed.
   \throw ClientConnectorException If the socket is already connected.
**/
void ClientConnector::connect()
{
  if(m_pSocket)
    throwClientConnectorException("connection already established");

#ifdef _WIN32
  if(!m_bWSAStarted)
  {
    LOG("wsastartup");
    if(WSAStartup(0x0101,&m_wsd)!=0)
      throwClientConnectorException("unable to start windows socket layer");
    m_bWSAStarted = true;
  }
#endif

  try
  {
    SockAddr saServer(m_strHost,m_wPort);
    m_pSocket = new BufferedSocket(m_nBufferSize);
    m_pSocket->create();
    m_pSocket->connect(saServer);
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

/**
   Closes the socket and deletes the memory accociated with the socket. It will
   not throw any exceptions on error. Use this when you are in a catch block,
   and need to close the connection.
**/
void ClientConnector::cleanup()
{
  if(!m_pSocket)
    return;

  m_pSocket->cleanup();
  DELETE_NULL(m_pSocket);
}

/** 
    Closes the connection and deletes the memory accociated with the socket.

    \throw ClientConnectorException If socket was not connected.
    \throw SocketInstanceException If there was a socket error during close operation.
**/
void ClientConnector::close()
{
  try
  {
    if(!m_pSocket)
      throwClientConnectorException("socket was not connected");

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
   \throw ClientConnectorException If socket is not connected. 
   \throw SocketInstance If there is a socket error during the send operation.
**/
void ClientConnector::sendPacket(PacketBuffer* pPkt)
{
  assert(pPkt);

  if(!m_pSocket) 
    throwClientConnectorException("socket is not connected");
  m_pSocket->sendPacket(pPkt);
}

/**
   \throw ClientConnectorException If socket is not connected. 
   \throw SocketInstance If there is a socket error during the send operation.
**/
void ClientConnector::sendPacket(PacketBuffer& packet)
{
  if(!m_pSocket) 
    throwClientConnectorException("socket is not connected");
  m_pSocket->sendPacket(packet);
}

/**
   Reads a full packet from the socket.

   \return 
   A PacketBuffer pointer that points to the data that was just read. This
   packet was created when the data was read in. You are responsible for
   destrying the memory once you are finished with the packet.

   \throw ClientConnectorException If the socket is not connected.
   \throw SocketInstanceException If there is a read error or socket gets closed.

   \note
   This is more effiecent then using the ">>" operator.
**/
PacketBuffer* ClientConnector::recvPacket()
{
  if(!m_pSocket) 
    throwClientConnectorException("socket is not connected");
  return m_pSocket->recvPacket();
}

/**
   Reads a full packet from the socket.

   \throw ClientConnectorException If the socket is not connected.
   \throw SocketInstanceException If there is a read error or socket gets closed.

   \note
   This is not as efficient as using ClientConnector::recvPacket, as ssobjects creates a new
   packet, then copies this to the reference you passed in. 
   It does however let you make cleaner C++ code.
**/
void ClientConnector::operator >> (PacketBuffer& packet)
{
  if(!m_pSocket)
    throwClientConnectorException("socket is not connected");
  packet.receive(m_pSocket);
}

/** 
    Send the data referenced by packet.

    \throw  ClientConnectorException If socket is not connected.
    \throw  SocketInstanceException If there was a socket error during send operation.
**/
void ClientConnector::operator << (PacketBuffer& packet)
{
  if(!m_pSocket)
    throwClientConnectorException("socket is not connected");
  packet.transmit(m_pSocket);
}

/** 
    Send the data pointed to by ppacket.

    \throw  ClientConnectorException If socket is not connected.
    \throw  SocketInstanceException If there was a socket error during send operation.
**/
void ClientConnector::operator << (PacketBuffer* ppacket)
{
  if(!m_pSocket)
    throwClientConnectorException("socket is not connected");
  ppacket->transmit(m_pSocket);
}

/**
   \return The socket this client object is connected to.

   \throw ClientConnectorException   If the socket isn't connected to anything.
**/
BufferedSocket* ClientConnector::getSocket()
{
  if(!m_pSocket)
    throwClientConnectorException("socket is not connected");
  return m_pSocket;
}
