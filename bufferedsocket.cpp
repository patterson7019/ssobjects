/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  1999 
        filename :  bufferedsocket.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  see header file for details
*********************************************************************/


#ifdef WIN32
#include "stdafx.h"
#endif

#include <stdlib.h>
#include <stdlib.h>
#ifndef OS_BSD
//#include <malloc.h>
#endif
#include <string.h>     //for memmove (linux)
#include "socketinstance.h"
#include "bufferedsocket.h"
#include "logs.h"

using namespace ssobjects;

/**
    Construct a buffered socket, with a default max buffer size of 
    DEFAULT_BUFFER_SIZE. The buffer will not grown any larger then that
    If there is more data to read, it will be left unread until there 
    is room in the buffer.
 
    \param bufferSize Max size of the socket buffer. 
    \throw  BufferedSocketException If memory for the buffer was not available.
**/
BufferedSocket::BufferedSocket(unsigned32 bufferSize)
  : m_pbuffer(),
    m_ptr(0),
    m_nBytesInBuf(0),
    m_nBufferSize(bufferSize),
    m_iTimeout(DEFAULT_SOCKET_TIMEOUT)
{
  m_pbuffer = (char*)calloc(m_nBufferSize,1);
  if(!m_pbuffer)
    throwBufferedSocketException("BufferedSocket::BufferedSocket() -- could not allocate buffer memory");
  m_ptr = m_pbuffer;
  m_nBytesInBuf = 0;
}

BufferedSocket::~BufferedSocket()
{
  if(m_pbuffer) free(m_pbuffer);
}

/**
    Sets how many seconds a read or send operation will wait 
    before throwing an exception.
**/ 
int BufferedSocket::setTimeout(int iTimeout)
{
  int ioldTimeout = m_iTimeout;
  m_iTimeout = iTimeout;
  return ioldTimeout;
}

/**
    Reads data into the referenced packet. This isn't as effiecent as using 
    recvPacket(). You are gerented to receive a full PacketBuffer object.

  
    \throw SocketInstanceException  If there is a read error, or a connection was closed.
    \throw BufferedSocketException          If a bad packet was read.
**/
PacketBuffer* BufferedSocket::recvPacket(PacketBuffer& packet)
{
  PacketBuffer* pPkt = recvPacket();
  packet = *pPkt;
  delete pPkt;
  return &packet;
}

/**
    Reads data and returns a pointer to a new PacketBuffer object. You are responsible for 
    deleting the returned packet when you are finished with it. 
    
    You are gerented to receive a full PacketBuffer object.

    When data is read in, some verification is done to ensure that the packet header is 
    valid. There is no way to validate the buffer portion of the packet. A test is done
    on the header to see if the cookie (just a constant value passed all the time) and
    to see the the command looks like it might be valid. There is no way to tell other then 
    if the command is zero (PacketBuffer::pcInvalid). Note that a common problem with getting 
    PacketBuffer::pcInvalid as a command is not setting the command in a packet before sending
    it.

    If a packet is sent with a size greater then 3000 bytes, the read operation will
    reset the internal buffer, and throw an exception. This is done to prevent bogus packets
    overflowing the buffer.
  
    \throw SocketInstanceException  If there is a read error, or a connection was closed.
    \throw BufferedSocketException          If a bad packet was read.
**/
PacketBuffer* BufferedSocket::recvPacket()
{
  PacketBuffer* pPacket = readFullPacket();

  if(pPacket->cookie()!=PacketBuffer::pkCookie || pPacket->getCmd()==PacketBuffer::pcInvalid)
  {
#ifdef DEBUG
    logs::dump(pPacket,PacketBuffer::getHeaderSize());
    logs::dump(pPacket->m_Buffer,pPacket->getBufferSize());
#endif
    resetBuffer();
    CStr msg;
    msg.format("bad packet received %s",
               pPacket->getCmd() ? "(invalid command)":"(invalid cookie)"); 
    throwBufferedSocketException(msg);
  }

  return pPacket;
}

//
//  put the buffer back to an empty state
//
void BufferedSocket::resetBuffer()
{
  m_ptr = m_pbuffer;
  m_nBytesInBuf = 0;
#ifdef DEBUG
  //make the reset data very visable
  memset(m_pbuffer,0xFF,nBufferSize);
#endif
}

//
//Read as much data as we can, up till we find 
//we have enough for a full packet. 
//
//We might read more then a full packet, and if
//so, this data is left in the buffer and used
//to create a packet on another call.
//
PacketBuffer* BufferedSocket::readFullPacket()
{
  PacketBuffer* pPacket=NULL;
  int iBytesIn;
  do
  {
    pPacket = parsForPacket();  //check if there is already enough for a packet
    if(!pPacket)
    {
      iBytesIn = recv(m_ptr,m_nBufferSize-m_nBytesInBuf,m_iTimeout);
      if(!iBytesIn)
      {
        resetBuffer();
        throwBufferedSocketException("connecton closed");
      }
      m_nBytesInBuf += iBytesIn;
      m_ptr += iBytesIn;
      pPacket = parsForPacket();
    }
  } while(!pPacket);
    
  return pPacket;
}

//
//We do two checks  to see that we have enough data to make 
//a full packet. One check is to see that we have enough data
//the make a header. The second is to see if there is enough
//to make a packet with a header plus it's data.
//We return NULL if there wasn't
//enough data to make a full packet. If there is enough data
//we return _one_ complete packet, and make sure the buffer
//is ready to extract more packets, and recieve additional
//data.
//
//A full packet is the size of the packet header (all packets
//_must_ have full header, plus the size of any data. There 
//doesn't always have to be any data with the packet.
//
PacketBuffer* BufferedSocket::parsForPacket()
{
  PacketBuffer* pPacket = NULL;
  char* phead = m_pbuffer;
  char* ptail = phead+m_nBytesInBuf;
  unsigned32 nSize;

  if(m_nBytesInBuf<PacketBuffer::getHeaderSize())
    return NULL;

  nSize = ntohl(*(unsigned32*)phead)+PacketBuffer::getHeaderSize();
  if((unsigned32)(ptail-phead) < nSize)
    return NULL;

  //copy in the header, then copy in packet data, less the header data
  pPacket = new PacketBuffer(0,nSize-PacketBuffer::getHeaderSize());    //new packet
  memcpy(pPacket->getHeader(),phead,PacketBuffer::getHeaderSize());
  pPacket->makeHostReady(true);       //convert header from network byte order
  if(nSize>PacketBuffer::getHeaderSize()) //if we have data after the header
    memcpy(pPacket->getBuffer(),phead+PacketBuffer::getHeaderSize(),pPacket->getBufferSize());

  phead += nSize;
  m_nBytesInBuf -= nSize;

  if(phead>=ptail)
  {
    //there is no more data in the buffer
    m_ptr = m_pbuffer;
    m_nBytesInBuf = 0;
  }
  else
  {
    //we need to move the unparsed data to the beginning of the buffer
    memmove(m_pbuffer,phead,m_nBytesInBuf);
    m_ptr = m_pbuffer+m_nBytesInBuf;
  }

  return pPacket;
}

/**
    Sends the data pointed to by pPacket, and optionaly destroys the memory pointed to by pPacket 
    once the data is sent. All data is garented to be sent.

    \param pPacket  Packet you are sending.
    \param release  When set to autoRelease, pPacket will be deleted once the data has been sent. 
                    When set to noRelease, pPacket is not deleted.

    \throw  SocketInstanceException If there was a problem during the send operation.
**/
int BufferedSocket::sendPacket(PacketBuffer* pPacket,const ReleaseFlag release)
{
  int ret;
  unsigned32 nBufferSize,nHeaderSize;

  nHeaderSize = pPacket->getHeaderSize();
  nBufferSize = pPacket->getBufferSize();
  pPacket->makeNetworkReady();

  ret  = write((char*)pPacket->getHeader(),nHeaderSize,m_iTimeout);
  if(nBufferSize)
    ret += write((char*)pPacket->getBuffer(),nBufferSize,m_iTimeout);

  if(autoRelease==release)
    DELETE_NULL(pPacket);

  return ret;
}

int BufferedSocket::sendPacket(PacketBuffer& packet)
{
  int ret = sendPacket(&packet,noRelease);
  return ret;
}

//send the packet
void BufferedSocket::operator << (PacketBuffer& packet)
{
  packet.transmit(this);
}

//send a packet pointer;
void BufferedSocket::operator << (PacketBuffer* ppacket)
{
  ppacket->transmit(this);
}

//receive packet
//this is not as efficient as using recvPacket, as it requires a copy after the packet is received
void BufferedSocket::operator >> (PacketBuffer& packet)
{
  packet.receive(this);
}
