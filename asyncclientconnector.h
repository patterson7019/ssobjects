/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  
        filename :  asyncclientconnector.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  
*********************************************************************/

#ifndef ASYNCCLIENTCONNECTOR_H
#define ASYNCCLIENTCONNECTOR_H

#include "ssobjects.h"
#include "serversocket.h"
#include "packetmessageque.h"

namespace ssobjects
{

#define throwAsyncClientConnectorException(m) (throw AsyncClientConnectorException(m,__FILE__,__LINE__))

/**
   \brief
   AsyncClient object throws exceptions of this type.
**/
class AsyncClientConnectorException : public GeneralException
{
  public: // Constructor/destructor
    AsyncClientConnectorException(char* pchMessage,const char* pFname,const int iLine) 
			: GeneralException(pchMessage,pFname,iLine){};
};

/**
   \brief
   Non-blocking client class that a client app uses to connect to a server.

   This is an asynchronis client class used so you can poll for incoming
   packets. If there are no packets available, your program will not block. If
   there is incoming data, it will be read and parsed into packets and posted to
   the message queue if possible.

   You would use this class if you didn't want to use threads. Note however,
   since this class uses the PacketMessageQue class, you must still compile using
   threading options.
**/
class AsyncClientConnector
{

  protected:  //attributes
    unsigned16      m_wPort;      ///< Port number of the server you wish to connect to.
    CStr            m_strHost;    ///< Host name of the server you wish to connect to.
    ServerSocket*   m_pSocket;    ///< Socket connected to the server.
    PacketMessageQue        m_que;        ///< Message que that contains any messages that have been parsed from read socket data.
#ifdef _WIN32
    WSADATA         m_wsd;        ///< WSA startup information (Windows only)
#endif

  protected:
    void        cleanup();        ///< Closes the connection without throwing any exeptions on errors during socket closer. Deletes the socket.
    void        setNonBlocking(SOCKET sockfd);  

  private:    //construction
    AsyncClientConnector();

  public:     //overridables
    void handleException(GeneralException& e);///< Not currently used.
    void sendPacket(PacketBuffer* pPkt);          ///< Sends the packet.
    void sendPacket(PacketBuffer& pPkt);          ///< Sends the packet.
    void operator << (PacketBuffer&);             ///< Sends the packet.
    void operator << (PacketBuffer*);             ///< Sends the packet.
    PacketMessage* getMsg();                          ///< Retrieves the next message from the message queue.

//TODO: add setTimeout() method

  public:     //construction
    AsyncClientConnector(const char* pszHost,word wPort);    ///< Constructs and connects to host.
    ~AsyncClientConnector();                                 ///< Destroys and closes any connections.

  public:     //methods
    const char* getServer()     {return m_strHost;}                   ///< Returns the server host name you assigned this client object.
    const word  getPort()       {return m_wPort;}                     ///< Returns the port you assigned this client object.
    bool        isConnected()   {return m_pSocket ? true:false;}      ///< Returns if this client is connected or not. 
    ServerSocket* getSocket();                                        ///< Returns the ServerSocket of this client object.

    void        reconnect();                                          ///< Tries to reconnect to the server.
    void        connect(const char* pszHost,const unsigned16 wPort);  ///< Stores server info passed in, and attempts to connect to the server.
    void        connect();                                            ///< Connects to the server.
    void        close();                                              ///< Closes the connection to the server.
    bool        idle();                                               ///< You call this so AsyncClientConnector can check for incoming data.

  private:
    //unused overloads
    AsyncClientConnector(const AsyncClientConnector&);
    AsyncClientConnector& operator=(const AsyncClientConnector&);
};

};

#endif  //ASYNCCLIENTCONNECTOR_H
