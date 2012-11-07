/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  December <19, 2000
       filename :  simpleserver.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
       purpose  :  Simple server is a class that you dirive from to
                   create a fully functional server. The simple server
                   class handles creating a listen socket, reading a full
                   packet from a client connection, idling the server.

                   You only need to override processSingleMsg() in order 
                   to process packets that come from a client socket.
                   See the section "user overrideables" below for what 
                   methods you would tipically call.

                   Override idle() method if you want to do things on a 
                   regular basis without waiting for a packet from a client.

                   The two ways of constructing a simple server are for a 
                   single threaded server, and a multi threaded server.

                   NOTE: Any shared data in your dirived classes should be 
                   in the SimpleManager and passed into the simple server. 
                   If you idle in the manager, you should idle the same way
                   in the server. The simple server however should check if 
                   it is running multi threaded, and only idle shared data
                   if it is running single threaded.

*********************************************************************/

#ifndef SIMPLESERVER_H
#define SIMPLESERVER_H

#ifdef _WIN32
# include <time.h>
#else
# include <sys/time.h>
# include <unistd.h>
#endif

#include "packetbuffer.h"
#include "flags.h"
#include "linkedlist.h"
#include "bufferedsocket.h"
#include "threadutils.h"
#include "packetmessageque.h"
#include "serversocket.h"
#include "simplemanager.h"
#include "stopwatch.h"

namespace ssobjects
{

#define throwSimpleServerException(m) (throw SimpleServerException(m,__FILE__,__LINE__))

/** 
    \brief SimpleServer errors.
**/
class SimpleServerException : public GeneralException
{
  public: // Constructor/destructor
    SimpleServerException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

class ServerSocket;
class ServerHandler;
class SimpleManager;

/**
   \brief Simple Server object used for creating a full fledged server. 

   <B> Overview </B> <br>
   Simple server is a class that you dirive from to create a fully functional
   server. The simple server class handles creating a listen socket, reading 
   full packets from a client connection, and idling the server.

   You only need to override processSingleMsg() in order to process packets that
   come from a client socket.  idle() is overridden if you want to have an idle
   task. 

   The two ways of constructing a simple server are for a single-threaded
   server, and a multi-threaded server.


   <B> User overridables </B> <br>namespace ssobjects
   processSingleMsg() <br>
   idle() <br>



   <B> Components </B> <br>
   o Message Pump <br>
   o Idler        <br>


   <B> Message Pump </B> <br>
   The message pump is the way the server talks to your code. When data comes
   into the server, it is first parsed into packets, and a PacketMessage is created that
   references the packet and the socket that it was sent from. It is then posted
   to the message que. Once there, processSingleMsg is called for every packet
   that was parsed from the incoming data. 

   This is where your code comes into play. You check what the packet command
   was, then extract the data from the packet if appropreate, perform some kind
   of operation, and perhaps send a reply back. This is the process of
   processing a clients request.

   Care should be taken to make sure that when processSingleMsg is called, your
   operations don't take very long. In a single threaded environment, all other
   packets - and there-for all the clients - will waiting for you to process
   thier requests.


   <B> Idler </B> <br>
   Idling is the process of repeating tasks at a regular interval, like having a
   main loop, or more accuratly, like using a WM_TIMER message in Windows. The
   frequency is controlled by the m_nFrequency attribute. 
   
   Override idle() method if you want to do things on a regular basis without
   waiting for a packet from a client. This is prefered over putting the server
   into a busy loop. If the server is in a busy loop, it will not process
   additional packets in a single-threaded environment. You can set how often
   this is called via the frequency attribute.


   <B>Threading</B> <br>
   It is possible to write the server in such a way that it will operate as a
   single-threaded server or a multi-threaded server, and this decision can be
   made at run time. It simply requires a little for-planning, and knowing where
   to put shared data. 

   The reason you would want to have a server capable of running as a single
   or threaded server is for debugging purposes under Linux. Single threading is
   easier to debug. Multi-threaded can give better performance, and is the best
   way to handle lengthy operations.

   Threading requires the use of the SimpleManager object. It's a little
   tricky to setup.


   \note 
   When running multi-threaded, any shared data in your dirived classes should
   be in the SimpleManager and passed into the simple server.  If you idle in
   the manager, you should idle the same way in the server. The simple server
   however should check if it is running multi threaded, and only idle shared
   data if it is running single threaded.
 **/
class SimpleServer : public ThreadHandler
{
    friend class SimpleManager;

  public:
    enum 
      {
      MIN_FREQ=10   ///< Used internally when calculating how much time has been spent waiting for data.
      };

  public:     //construction
    SimpleServer(const SockAddr& saBind,
                 const unsigned32 nFreq=1000,
                 const unsigned32 nMaxConnections=100);   ///< Single-threaded constructor.
    SimpleServer(ServerSocket* const psocket,
                 const unsigned32 nFreq);                 ///< Multi-threaded constructor
    virtual ~SimpleServer();
        
  protected:  //user overridables
    /** 
        \brief Override this to process packets that come in from clients. 

        The server will call this method when there was enough data to have
        parsed it into a packet and needs to have the packet processed. A PacketMessage
        object is created to contain both the packet and client socket that send
        the packet. You will typically send your reply to this socket. In
        certain circumstances you will send the reply to all client connections
        such as in the case of a chat message.

        When you override this method, your function will typically look
        something like: 

        <PRE>
        ...
        void
        YourServer::processSingleMsg(PacketMessage* pmsg)
        {
          ServerSocket* psocket = (ServerSocket*)pmsg->socket();
          PacketBuffer* ppacket = pmsg->packet();
          switch(ppacket->getCmd())
          {
            //One way to handle the message. Process and reply within the switch.
            case PacketBuffer::pcPing:
              //do something importaint
              sendPacket(psocket,PacketBuffer(PacketBuffer::pcPong)); //send a reply pong
              break;

            //The prefered way to handle the message. Keeps the switch clean.
            case PacketBuffer::pcGetVersion: onGetVersion(pmsg); break;
          }
          DELETE_NULL(ppacket);   //IMPORTANT! The packet is no longer needed. You must delete it.
        }
        ...
        void
        YourServer::onGetVersion(PacketMessage* pmsg)
        {
          ServerSocket* psocket = (ServerSocket*)pmsg->socket();
          PacketBuffer version(PacketBuffer::pcVersion);  //create the reply packet
          version << "Version: 1.0";              //insert the version information into the packet
          sendPacket(psocket,version);            //send the packet to the client socket
        }
        ...
        </PRE>

        For more information see the Message Pump section in the Detailed
        Description section. 

        \param pmsg Pointer to the message object that contains the packet and
        cilent socket that sent the packet. 
    **/
    virtual void    processSingleMsg(PacketMessage* pmsg) = 0;

    /// Override to have a timer in your server.
    virtual void    idle(unsigned32 nTimer=0) {UNUSED_ALWAYS(nTimer);}
      

  public:     //server controls
    virtual void    startServer();                                              ///< Starts up the server, makes ready for connections
    virtual void    killServer();                                               ///< Stops server, and closes all connections
    virtual void    pauseIncomingConnections();                                 ///< (Not currently in use.) Server will no longer accept connections
    virtual void    sendPacket(ServerSocket* psocket,const PacketBuffer& packet);   ///< Sends a packet over a socket.
    virtual void    sendPacket(ServerSocket* psocket,const PacketBuffer* ppacket);  ///< Sends a packet over a socket.
    virtual void    send2All(const PacketBuffer& packet);                           ///< Sends a packet to all client sockets connected to the server.
    virtual void    send2All(const PacketBuffer* ppacket);                          ///< Sends a packet to all client sockets connected to the server.
    virtual void    removeSocket(ServerSocket* psocket);                        ///< Removes a socket from the server.

  public:
    static bool     canBind(SockAddr& saBind);                                ///< Checks if we are able to bind. good for when you are about to start the server
    unsigned32      getTicks();                                                 ///< System timer ticks.
    bool            isThreaded() { return m_bUsingThread; }                     ///< Check if server is in single or multi-threaded mode.

    //nothing servicable beyond this point
  protected:
    threadReturn    ThreadHandlerProc(void);                //main handler routine
    void            processMessages();
    bool            processSockets(int iReady);
    void            processSelectError();
    void            acceptConnection();
    int             getMaxFD();                             //return highest connected sock fd
    long            getSleepTime();                         //how many usecs to sleep according to idle frequency
    void            addMsg(PacketMessage* pmsg) { m_que.add(pmsg); }
    void            queClosedMessage(ServerSocket* psocketRemoving);
    void            calcSleepTime(struct timeval* tv,const unsigned32 sleepTime,const StopWatch& timer);   ///< Figure out how much time we should sleep based on how much we have already slept

  private:    //unused overrides
    SimpleServer(const SimpleServer& server);
    SimpleServer& operator=(const SimpleServer& server);
        
  protected:
    SocketInstance          m_sListen;
    SockAddr                m_saServer;
    LinkedList<ServerSocket>    m_listClients;
    PacketMessageQue        m_que;
    unsigned32              m_nIdleFrequency;       //how often idle will be called
    unsigned32              m_nMaxCon;
    bool                    m_bUsingThread;
    bool                    m_bPause;      
    unsigned32              m_nSleepTime;
    struct timeval          m_tvServerStarted;
    fd_set                  m_rset,m_wset;
};

};

#endif //SIMPLESERVER_H
