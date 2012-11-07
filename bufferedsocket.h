/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  1999 
        filename :  bufferedsocket.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Socket class provides a convienent way to send 
                    and recieve packets of data.
*********************************************************************/

#ifndef BUFFEREDSOCKET_H
#define BUFFEREDSOCKET_H

#include "packetbuffer.h"
#include "generalexception.h"
#include "socketinstance.h"
#include <ctype.h>

namespace ssobjects
{

/**
    Base class for sending and receiving data send as PacketBuffers.

    When data is sent, not all data is garenteed to be sent in the one call. 
    (See man page for send(3) for details.)

    When data is read, or received, not all the data is garenteed to be 
    read in one recv call. (See man page for recv(3) for details.)

    To make life easier, the functionality for sending and receiving an 
    entire PacketBuffer object, without multiple calls to send()/recv() is 
    wrapped up in this object. 

    Also, if 3 packets were sent, but only 2 1/2 were read, BufferedSocket will
    parse out the two, and keep the data from 3 till the next call.

    \todo Verify it's send(3) and not send(2).
**/
class BufferedSocket : public SocketInstance
{
  public: 
    enum {DEFAULT_BUFFER_SIZE = 3000};  // about 3 packets
    
  public:
    BufferedSocket(unsigned32 bufferSize = DEFAULT_BUFFER_SIZE);                                 ///< Construct the socket.
    ~BufferedSocket();                                ///< Destroys the socket, and closes the connection.
    typedef enum {autoRelease,noRelease} ReleaseFlag;
    int sendPacket(PacketBuffer* pPacket,const ReleaseFlag release=noRelease);  ///< Transmits the packet.
    int sendPacket(PacketBuffer& packet);                                       ///< Transmits the packet.
    PacketBuffer* recvPacket();                                                 ///< Reads and waits for a full PacketBuffer object.
    PacketBuffer* recvPacket(PacketBuffer& packet);                                 ///< Reads and waits for a full PacketBuffer object.
    int setTimeout(const int iTimeOut);                                     ///< Sets how long to wait for incoming data before throwing.
    void operator << (PacketBuffer& packet);                                    ///< Transmits the packet.
    void operator << (PacketBuffer* ppacket);                                   ///< Transmits the packet.
    void operator >> (PacketBuffer& packet);                                    ///< Reads and waits for a full PacketBuffer object.

  private:
    PacketBuffer* readFullPacket();
    PacketBuffer* parsForPacket();
    void    resetBuffer();

  private:
    enum{nBufferSize=1024};
    char* m_pbuffer;        //buffer to hold temp info
    char* m_ptr;            //pointer into the buffer
    unsigned32 m_nBytesInBuf; //size of data in buffer
    unsigned32 m_nBufferSize; //size of the buffer
    int   m_iTimeout;
  private:
    //unused overloads
    BufferedSocket(const BufferedSocket&);
    BufferedSocket& operator=(const BufferedSocket&);
};

#define throwBufferedSocketException(m) (throw BufferedSocketException(m,__FILE__,__LINE__))

class BufferedSocketException : public GeneralException
{
  public: // Constructor/destructor
    BufferedSocketException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

};

#endif //BUFFEREDSOCKET_H
