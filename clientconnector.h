/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        filename :  clientconnector.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  A client uses this to connect to a server.
                    Provides abilty to direct packets via << & >>
                    operators.
*********************************************************************/

#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include "ssobjects.h"
#include "bufferedsocket.h"

namespace ssobjects
{

#define throwClientConnectorException(m) (throw ClientConnectorException(m,__FILE__,__LINE__))

/**
   \brief 
   ClientConnector object throws exeptions of this type.
**/
class ClientConnectorException : public GeneralException
{
  public: // Constructor/destructor
    ClientConnectorException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

/**
   \brief 
   Blocking client class that a client app uses to connect to a server.

   Use this class when you want to connect to a server that uses ssobjects. ClientConnector
   provides methods and operators for sending a receiving packets to/from the 
   server you are connected to. 
**/
class ClientConnector
{

#ifdef _WIN32
  public:
    WSADATA             m_wsd;              ///< WSA startup information (Windows only)
    static bool         m_bWSAStarted;      ///< Flag indicating if we have started socket layer.
#endif

  protected:  //attributes
    unsigned32  m_nBufferSize;		    ///< How large we want to make our buffered socket objects
    unsigned16  m_wPort;                    ///< Port number of the server you wish to connect to.
    CStr        m_strHost;                  ///< Host name of the server you wish to connect to.
    BufferedSocket*   m_pSocket;                  ///< Socket connected to the server.

  protected:
    void        cleanup();                  ///< Closes the connection without throwing any exeptions on errors during socket closer. Deletes the socket.

  public:     //overridables
    void handleException(GeneralException& e);   ///< Not currently used.
    void sendPacket(PacketBuffer* pPkt);        ///< Sends the packet.
    void sendPacket(PacketBuffer& pPkt);        ///< Sends the packet.
    PacketBuffer* recvPacket();                 ///< Receives a packet.
    void operator << (PacketBuffer&);           ///< Sends the packet.
    void operator << (PacketBuffer*);           ///< Sends the packet.
    void operator >> (PacketBuffer&);           ///< Receives a packet.

//TODO: add setTimeout() method

  public:     //construction
    ClientConnector(const unsigned32 bufferSize = BufferedSocket::DEFAULT_BUFFER_SIZE);
    ClientConnector(const char* pszHost,word wPort,const unsigned32 bufferSize = BufferedSocket::DEFAULT_BUFFER_SIZE); ///< Constructs and connects to host.
    ~ClientConnector();                              ///< Destroys and closes any connections.

  public:     //methods
    CONSTCHAR*  getServer()     {return m_strHost;}                 ///< Returns the server host name you assigned this client object.
    word        getPort()       {return m_wPort;}                   ///< Returns the port you assigned this client object.
    bool        isConnected()   {return m_pSocket ? true:false;}    ///< Returns if this client is connected or not. 
    BufferedSocket*   getSocket();

    void        reconnect();                                        ///< Tries to reconnect to the server.
    void        connect();                                          ///< Connects to the server.
    void        connect(const char* szHost,const unsigned wPort);   ///< Stores server info passed in, and attempts to connect to the server.
    void        close();                                            ///< Closes the connection to the server.

  private:
    //unused overloads
    ClientConnector(const ClientConnector&);
    ClientConnector& operator=(const ClientConnector&);
};

};

#endif  //CLIENTCONNECTOR_H
