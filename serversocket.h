/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  November 21, 2000
       filename :  serversocket.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Handles buffered socket operations.

*********************************************************************/

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#ifdef _WIN32
#   include <time.h>
#else
#   include <sys/time.h>
#   include <unistd.h>
#endif

#include "packetbuffer.h"
#include "flags.h"
#include "linkedlist.h"
#include "bufferedsocket.h"
#include "threadutils.h"
#include "packetmessageque.h"

namespace ssobjects
{

#define throwServerSocketException(m) (throw ServerSocketException(m,__FILE__,__LINE__))
/**
   \brief
   ServerSocket object errors.
**/ 
class ServerSocketException : public GeneralException
{
  public: // Constructor/destructor
    ServerSocketException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

class ServerHandler;

/**
   \brief
   Socket object that contains both a read and write buffer.

    The Server Socket object contains an incoming buffer for data that is 
    read in and isn't parsed, and an outgoing buffer for data that hasn't 
    been sent. The SimpleServer object uses this object for every connection
    that comes in.

    A ServerHandler object can be stored here for easy access to it from a server
    object that has a number of server socket objects.
**/
class ServerSocket : public SocketInstance
{
  public:   //enums
    /// Not used
    enum
      {
      modeInvalid         = 0,

      modeWriteOkay = 1,    //it is okay for the write thread to write to this socket
      modeSendOkay  = 2,    //it is okay to send more packets to this socket (send
      modeReadOkay  = 4,    //it is okay to read data from this socket
      modeClosing   = 8,    //socket should be closed next call to CleanSockets 
      modeNotified  = 16,   //a message to have this socket removed has been sent to server
      modeClosed    = 32,   //this socket has been closed, and is now free

      modeValid=modeSendOkay|modeWriteOkay|modeReadOkay,        //socket is connected, and everything is okay

      modeAll=0xFFFFFFFF
      };

  public:
    ServerSocket(const unsigned32 nBufferSize=18000,const int iTimeout=DEFAULT_SOCKET_TIMEOUT);
    ServerSocket(const SocketInstance& socket,const SockAddr& sa,const unsigned32 nBuffSize,const int iTimeout);
    virtual ~ServerSocket();

    void            sendPacket(const PacketBuffer& packet);             ///< Will add to outgoing buffer, and try sending it.
    void            sendPacket(const PacketBuffer* const ppacket);      ///< Will add to outgoing buffer, and try sending it.
    void            sendBuffer();                                   ///< Will attept to send any data in the outgoing buffer.
    int             readData();                                     ///< Reads in as much data is available on the socket.
    PacketBuffer*       extractPacket();                                ///< Extracts a packet from the incoming buffer.
    const Flags&    flags() const {return m_flags;}                         ///< Returns the socket flags. (Not currently used.)
    unsigned32      getOutBufferSize() const {return m_nBytesOut;}          ///< Return #bytes in outgoing buffer.
    unsigned32      getOutBufferSizeMax() const {return m_nBufferSizeMax;}  ///< Return the actual size (how much was allocated) of the buffer.
    unsigned32      getInBufferSize() const  {return m_nBytesIn;}           ///< Return #bytes in incoming buffer.
    unsigned32      getBufferSizeMax() const {return m_nBufferSizeMax;}     ///< How big the in/out buffer is.
    SockAddr        getAddr() const {return m_socketAddr;}                  ///< Gets the socket address information of the connected socket.
    int             socketTimeout() const {return m_iTimeout;}              ///< Returns the number of seconds a send/receive operation will wait before throwing.
    ServerHandler* serverHandler() const {return m_pserverHandler;}             ///< Returns the ServerHandler object that was assigned to this server socket object.
    void            setServerHandler(ServerHandler* s) {m_pserverHandler = s;}  ///< Sets the ServerHandler object that this server socket will use.

  protected:
    char*           getOutBuffer() const {return m_pOutBuff;}
    char*           getInBuffer() const {return m_pInBuff;}
    void            rotateBuffer(char* pbuffer,unsigned32 nBuffSize,unsigned32 nBytesRotatingOut);
    void            addPacketBuffer(const PacketBuffer* const ppacket);

  protected:
    ServerHandler*  m_pserverHandler;
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
    ServerSocket(const ServerSocket&);
    ServerSocket& operator=(const ServerSocket&);
};

};

#endif  //SERVERSOCKET_H
