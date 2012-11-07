/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  November 21, 2000
       filename :  serversocket.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Handles buffered socket operations.

*********************************************************************/

#include <stdio.h>
#include "telnetserversocket.h"

using namespace ssobjects;

TelnetServerSocket::TelnetServerSocket(
  const SocketInstance& socket,
  const SockAddr& sa,
  const unsigned32 nBufferSize,
  const int iTimeout)
  : SocketInstance(socket),
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

#ifdef _WIN32
//TODO: figure out why it was needed for win32, and why not working on Linux
  char opt = 1;
  setsockopt(m_hSocket,SOL_SOCKET,TCP_NODELAY,&opt,sizeof opt);
#endif
  assert(m_pInBuff);
  assert(m_pOutBuff);
}

TelnetServerSocket::TelnetServerSocket(const unsigned32 nBufferSize,const int iTimeout)
  : m_socketAddr(),
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
TelnetServerSocket::~TelnetServerSocket()
{
  delete [] m_pInBuff;
  delete [] m_pOutBuff;
}


int
TelnetServerSocket::sendString(const char* pszString)
{
# ifdef DEBUG
  assert(pszString);    //passing in NULL string is not allowed
  assert(strlen(pszString) + getOutBufferSize() < getOutBufferSizeMax());
# endif
  addPacketBuffer(pszString);
  sendBuffer();
  return strlen(pszString);
}


void
TelnetServerSocket::addPacketBuffer(const char* pszString)
{
  assert(pszString);

  unsigned32 nSize = strlen(pszString);
  memcpy(m_pOutPtr,pszString,nSize);
  m_pOutPtr   += nSize;
  m_nBytesOut += nSize;
}

void
TelnetServerSocket::sendBuffer()
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
//      DLOG("had %d bytes to send for socket %d",m_nBytesOut,m_hSocket);
    }
  }
//  DLOG("sendbufffer: %d bytes remain unsent for socket %d",getOutBufferSize(),m_hSocket);
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
TelnetServerSocket::readData()
{
  //TODO: do a non blocked recv
  int iBytesRead=0;
  iBytesRead = recv(m_pInPtr,getBufferSizeMax()-getInBufferSize(),socketTimeout());
    
  if(iBytesRead > 0)
  {
    DLOG("Read %d bytes",iBytesRead);
    DUMP(m_pInPtr,iBytesRead);
    m_pInPtr   += iBytesRead;
    m_nBytesIn += iBytesRead;
  }
  return iBytesRead;
}

void
TelnetServerSocket::rotateBuffer(char* pbuffer,unsigned32 nBuffSize,unsigned32 nBytesRotatingOut)
{
  assert(pbuffer && nBuffSize && nBytesRotatingOut);
  memmove(pbuffer,pbuffer+nBytesRotatingOut,nBuffSize-nBytesRotatingOut);
}

/**
   Parses out a single line of data, and puts it into a packet buffer with the
   packet command of TelnetServerSocket::pcFullLine
    
   A single line of data is terminated with a CR LF combination to indicate the
   end-of-line. This is not complient to a telnet server. This will be fixed at
   a later date.

   See section 3.3.1 of RFC 1123 for information about End-of-Line
   convention. See RFC 854 for the Telnet Protocol Specification.

   \return NULL if there isn't enough data to return a full packet.
           A pointer to a newly created PacketBuffer object if there is.

   \throw TelnetServerSocketException If a packet with an invalid size is read.
**/
PacketBuffer* 
TelnetServerSocket::extractPacket()
{
  if(!m_nBytesIn)
    return NULL;

  PacketBuffer*   pPacket = NULL;
  char*       phead   = m_pInBuff;
  char*       ptail   = phead+m_nBytesIn;
  char*       p       = phead;
  unsigned32  nSize;

  while(p<ptail && *p != LF)
    p++;
  if(p==ptail)
    return NULL;    // not a CRLF terminated line yet.

  p++;  //move past the LF
  nSize = p-phead;
  pPacket = new PacketBuffer(pcFullLine,nSize-1);
  p = (char*)pPacket->getBuffer();
# ifdef DEBUG
  memset(p,0xff,nSize);
# endif
  memcpy(p,phead,nSize-2);       // don't copy the CRLF pair
  p[nSize-2] = '\0';            // NULL terminate the string

  rotateBuffer(getInBuffer(),getInBufferSize(),nSize);
  m_nBytesIn -= nSize;
  m_pInPtr   -= nSize;

  return pPacket;
}


/**
   Writes a line of text to the socket, and addes CR & LF.

   \param fmt printf formatting string.
   \param ... printf parameters.

   \return The number of bytes sent.

   \throw SocketInstanceException If there is a socket error.
**/
int TelnetServerSocket::println(const char* fmt,...)
{
  char buffer[1024];
  va_list marker;

  va_start(marker,fmt);
  vsnprintf(buffer,sizeof buffer,fmt,marker);
  NULL_TERMINATE(buffer,sizeof buffer);

  int iBytes;
  strcat(buffer,"\r\n");
  iBytes = sendString(buffer);
  return iBytes;
}

/**
   Writes a line of text to socket.

   \param fmt printf formatting string.
   \param ... printf parameters.

   \return The number of bytes sent.

   \throw SocketInstanceException If there is a socket error.
**/
int TelnetServerSocket::print(const char* fmt,...)
{
  char buffer[1024];
  va_list marker;

  va_start(marker,fmt);
  vsnprintf(buffer,sizeof buffer,fmt,marker);
  NULL_TERMINATE(buffer,sizeof buffer);

  return sendString(buffer);
}
