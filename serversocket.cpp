/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  November 21, 2000
       filename :  serversocket.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Handles buffered socket operations.

*********************************************************************/

#include "serversocket.h"
#include "serverhandler.h"

using namespace ssobjects;

ServerSocket::ServerSocket(
  const SocketInstance& socket,
  const SockAddr& sa,
  const unsigned32 nBufferSize,
  const int iTimeout)
  : SocketInstance(socket),
    m_pserverHandler(NULL),
    m_socketAddr(sa),
    m_nID(0),
    m_iTimeout(iTimeout),
    m_flags(),
    m_pInBuff(NULL),
    m_pOutBuff(NULL),
    m_pOutPtr(NULL),
    m_pInPtr(NULL),
    m_nBytesIn(0),
    m_nBytesOut(0),
    m_nBufferSizeMax(nBufferSize)
{
  m_pInBuff = new char[nBufferSize];
  m_pOutBuff = new char[nBufferSize];
  m_pOutPtr = m_pOutBuff;
  m_pInPtr = m_pInBuff;

  assert(m_pInBuff);
  assert(m_pOutBuff);
}

ServerSocket::ServerSocket(const unsigned32 nBufferSize,const int iTimeout)
  : m_pserverHandler(NULL),
    m_socketAddr(),
    m_nID(0),
    m_iTimeout(iTimeout),
    m_flags(),
    m_pInBuff(NULL),
    m_pOutBuff(NULL),
    m_pOutPtr(NULL),
    m_pInPtr(NULL),
    m_nBytesIn(0),
    m_nBytesOut(0),
    m_nBufferSizeMax(nBufferSize)
{
  m_pInBuff = new char[nBufferSize];
  m_pOutBuff = new char[nBufferSize];
  m_pOutPtr = m_pOutBuff;
  m_pInPtr = m_pInBuff;

  assert(m_pInBuff);
  assert(m_pOutBuff);
}

/** 
    Deletes the in buffer, out buffer, and the server handler you assigned it.
**/
ServerSocket::~ServerSocket()
{
  delete [] m_pInBuff;
  delete [] m_pOutBuff;
  delete m_pserverHandler;
}

void
ServerSocket::sendPacket(const PacketBuffer& packet)
{
  sendPacket(&packet);
}

void
ServerSocket::sendPacket(const PacketBuffer* const ppacket)
{
#   ifdef DEBUG
  assert(ppacket);    //passing in NULL packet is not allowed
  assert(!(ppacket->getBufferSize()+getOutBufferSize() > getOutBufferSizeMax()));     //the output buffer  doesn't have any more room left to put passed in new data 
#   else
  if(ppacket->getBufferSize()+getOutBufferSize() > getOutBufferSizeMax())
  {
    CStr msg = "buffer too small or can't fit packet";
#       ifdef DEBUG
    msg.format("buffer too small or can't fit packet (max%03d size%03d pack%03d)",
               getOutBufferSizeMax(),getOutBufferSize(),ppacket->getBufferSize());
#       endif        
    throwServerSocketException(msg);
  }
#endif
  addPacketBuffer(ppacket);
  sendBuffer();
}

void
ServerSocket::addPacketBuffer(const PacketBuffer* const ppacket)
{
  assert(ppacket);

  //TODO: should somehow make a copy of the header, and memcpy entire
  //TODO  header at once instead of each member

  unsigned32 nBufferSize = ppacket->getBufferSize();
  unsigned16 wCookie = ppacket->cookie();
  unsigned16 wCommand = ppacket->getCmd();

  nBufferSize = htonl(nBufferSize);
  wCookie     = htons(wCookie);
  wCommand    = htons(wCommand);

  memcpy(m_pOutPtr,&nBufferSize,sizeof(nBufferSize));
  m_pOutPtr += sizeof(nBufferSize);
  memcpy(m_pOutPtr,&wCookie,sizeof(wCookie));
  m_pOutPtr += sizeof(wCookie);
  memcpy(m_pOutPtr,&wCommand,sizeof(wCommand));
  m_pOutPtr += sizeof(wCommand);

  memcpy(m_pOutPtr,ppacket->getBuffer(),ppacket->getBufferSize());
  m_pOutPtr += ppacket->getBufferSize();

  m_nBytesOut += ppacket->getBufferSize() + sizeof(nBufferSize)+sizeof(wCookie)+sizeof(wCommand);
}

void
ServerSocket::sendBuffer()
{
  //send only if there is data in the outgoing buffer
  if(getOutBufferSize())
  {
    int iBytesSent = send(getOutBuffer(),getOutBufferSize(),m_iTimeout);
    if(getOutBufferSize() == (unsigned32)iBytesSent)
    {
      m_pOutPtr = getOutBuffer();
      m_nBytesOut = 0;
    }
    else
    {
      rotateBuffer(getOutBuffer(),getOutBufferSize(),iBytesSent);
      m_nBytesOut -= iBytesSent;
      m_pOutPtr -= iBytesSent;
      DLOG("had %d bytes to send for socket %d",m_nBytesOut,m_hSocket);
    }
  }
}

/**
   Reads data from the socket. Will only read as many bytes as will fit in our
   input buffer. If 0 bytes are available in the buffer, then recv will read 
   0 bytes. When this happens, a SocketInstanceException is thrown. This is 
   desirable as if the buffer is full, and hasn't been processed, then it is
   considered an error.

   \return The number of bytes read.

   \throw SocketInstanceException If there was a read error, or the input
   buffer was full.
**/
int
ServerSocket::readData()
{
  //TODO: do a non blocked recv
  int iBytesRead=0;
  iBytesRead = recv(m_pInPtr,getBufferSizeMax()-getInBufferSize(),socketTimeout());
    
  if(iBytesRead > 0)
  {
    m_pInPtr   += iBytesRead;
    m_nBytesIn += iBytesRead;
  }
  return iBytesRead;
}

void
ServerSocket::rotateBuffer(char* pbuffer,unsigned32 nBuffSize,unsigned32 nBytesRotatingOut)
{
  assert(pbuffer && nBuffSize && nBytesRotatingOut);
  memmove(pbuffer,pbuffer+nBytesRotatingOut,nBuffSize-nBytesRotatingOut);
}

/**
    We do two checks  to see that we have enough data to make 
    a full packet. One check is to see that we have enough data
    the make a header. The second is to see if there is enough
    to make a packet with a header plus it's data.
    
    We return NULL if there wasn't enough data to make a full packet. 
    If there is enough data we return _one_ complete packet, and make 
    sure the buffer is ready to extract more packets, and recieve 
    additional data.
    
    A full packet is the size of the packet header (all packets
    _must_ have full header) plus the size of any data. There 
    doesn't always have to be any data with the packet.

    If the size that is read in is invalid, extractPacket throws an 
    exception. This indicates that the data read in was not valid. 
    
    \return NULL if there isn't enough data to return a full packet.
            A pointer to a newly created PacketBuffer object if there is.

    \throw ServerSocketException If a packet with an invalid size is read.
**/
PacketBuffer* 
ServerSocket::extractPacket()
{
  PacketBuffer*   pPacket = NULL;
  char*       phead   = m_pInBuff;
  char*       ptail   = phead+m_nBytesIn;
  unsigned32  nSize;

  //TODO: would like to have only one return, change logic
  if(m_nBytesIn<PacketBuffer::getHeaderSize())
    return NULL;


  nSize = ntohl(*(unsigned32*)phead);
  if(nSize > getBufferSizeMax() || nSize >= 0xFFFFFFFF-PacketBuffer::getHeaderSize())
    throwServerSocketException("Packet size invalid");

  nSize += PacketBuffer::getHeaderSize();
  if((unsigned32)(ptail-phead) < nSize)
    return NULL;

  //copy in the header, then copy in packet data, less the header data
  pPacket = new PacketBuffer(0,nSize-PacketBuffer::getHeaderSize());    //new packet
  memcpy(pPacket->getHeader(),phead,PacketBuffer::getHeaderSize());
  pPacket->makeHostReady(true);       //convert header from network byte order
  if(nSize>PacketBuffer::getHeaderSize()) //if we have data after the header
    memcpy(pPacket->getBuffer(),phead+PacketBuffer::getHeaderSize(),pPacket->getBufferSize());

  phead += nSize;
  m_nBytesIn -= nSize;

  if(phead>=ptail)
  {
    //there is no more data in the buffer
    m_pInPtr = m_pInBuff;
    m_nBytesIn = 0;
  }
  else
  {
    //we need to move the unparsed data to the beginning of the buffer
    memmove(m_pInBuff,phead,m_nBytesIn);
    m_pInPtr = m_pInBuff+m_nBytesIn;
  }

  return pPacket;
}
