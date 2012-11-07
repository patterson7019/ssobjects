/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  November 21, 2000
       filename :  telnetserversocket.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Handles buffered socket operations.

*********************************************************************/

#ifndef TELNETSERVERSOCKET_H
#define TELNETSERVERSOCKET_H


#ifdef _WIN32
#   include <time.h>
#else
#   include <sys/time.h>
#   include <unistd.h>
#endif

#include "packetbuffer.h"
#include "flags.h"
#include "linkedlist.h"
#include "threadutils.h"
#include "packetmessageque.h"

using namespace ssobjects;

#define throwTelnetServerSocketException(m) (throw TelnetServerSocketException(m,__FILE__,__LINE__))

/**
   \brief
   TelnetServerSocket object errors.
**/ 
class TelnetServerSocketException : public GeneralException
{
  public: // Constructor/destructor
    TelnetServerSocketException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};


#define CR 13 ///< Carriage return (0x0D)
#define LF 10 ///< Line feed       (0x0A)
#define NUL 0 ///< Telnet no operation

/**
    \brief
    Socket object that contains both a read and write buffer.

    The Server Socket object contains an incoming buffer for data that is 
    read in and isn't parsed, and an outgoing buffer for data that hasn't 
    been sent. The SimpleServer object uses this object for every connection
    that comes in.
**/
class TelnetServerSocket : public SocketInstance
{
  public:
    enum
    {
      pcFullLine = PacketBuffer::pcUser					///< A full line of text has been read from the socket.
    };

  public:
    TelnetServerSocket(const unsigned32 nBufferSize=18000,const int iTimeout=DEFAULT_SOCKET_TIMEOUT);
    TelnetServerSocket(const SocketInstance& socket,const SockAddr& sa,const unsigned32 nBuffSize,const int iTimeout);
    virtual ~TelnetServerSocket();

    int             sendString(const char* pszString);                      ///< Will add to outgoing buffer, and try sending it.
    int             println(const char* fmt,...);                           ///< Send a line of text, appending CR&LF.
    int             print(const char* fmt,...);                             ///< Send a line of text.
    void            sendBuffer();                                           ///< Will attept to send any data in the outgoing buffer.
    int             readData();                                             ///< Reads in as much data is available on the socket.
    PacketBuffer*   extractPacket();                                        ///< Extracts a packet from the incoming buffer.
    const Flags&    flags() const {return m_flags;}                         ///< Returns the socket flags. (Not currently used.)
    unsigned32      getOutBufferSize() const {return m_nBytesOut;}          ///< Return #bytes in outgoing buffer.
    unsigned32      getOutBufferSizeMax() const {return m_nBufferSizeMax;}  ///< Return the actual size (how much was allocated) of the buffer.
    unsigned32      getInBufferSize() const  {return m_nBytesIn;}           ///< Return #bytes in incoming buffer.
    unsigned32      getBufferSizeMax() const {return m_nBufferSizeMax;}     ///< How big the in/out buffer is.
    SockAddr        getAddr() const {return m_socketAddr;}                  ///< Gets the socket address information of the connected socket.
    int             socketTimeout() const {return m_iTimeout;}              ///< Returns the number of seconds a send/receive operation will wait before throwing.
    char*           getInBuffer() const {return m_pInBuff;}
    char*           getOutBuffer() const {return m_pOutBuff;}
    
  protected:
    void            rotateBuffer(char* pbuffer,unsigned32 nBuffSize,unsigned32 nBytesRotatingOut);
    void            addPacketBuffer(const char* pszString);

  protected:
    SockAddr        m_socketAddr;
    unsigned32      m_nID;
    int             m_iTimeout;
    Flags           m_flags;
    char*           m_pInBuff;      //buffer that contains data that was read from socket
    char*           m_pOutBuff;     //buffer that contains data that will be sent to socket
    char*           m_pOutPtr;      //pointer to first free byte in outgoing buffer
    char*           m_pInPtr;       //pointer to first free byte in incoming buffer
    unsigned32      m_nBytesIn;     //#bytes in the incoming buffer
    unsigned32      m_nBytesOut;    //#bytes in the outgoing buffer
    unsigned32      m_nBufferSizeMax;       //how big the in/out buffer is

  protected:
    //unused overloads
    TelnetServerSocket(const TelnetServerSocket&);
    TelnetServerSocket& operator=(const TelnetServerSocket&);
};

#endif  //TELNETSERVERSOCKET_H
