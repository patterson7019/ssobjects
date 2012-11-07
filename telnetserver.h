/********************************************************************
       Copyright (c) 2006, Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  December 3, 2001
       filename :  telnetserver.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

*********************************************************************/

#ifndef TELNETSERVER_H
#define TELNETSERVER_H

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

using namespace ssobjects;

/**
   \brief Server object used to create a telnet server.

   Works much like the TelnetServer object, except data is all text, and is parsed
   into lines. 
**/

#define throwTelnetServerException(m) (throw TelnetServerException(m,__FILE__,__LINE__))

class TelnetServerException : public GeneralException
{
  public: // Constructor/destructor
    TelnetServerException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

class TelnetServerSocket;
class SimpleManager;

/**
   \brief Telnet Server object used for creating a full fledged telnet server.


   <B> Overview </B> <br>

   <I>(Documentation modified from SimpleServer)</I>

   Very similar to the SimpleServer class, except data that is read is lines of
   text. Telnet server is a class that you derive from to create a fully
   functional server. The telnet server class handles creating a listen socket,
   reading full lines of text from a client connection, and idling the server.

   You only need to override processSingleMsg() in order to process packets that
   come from a client socket.  idle() is overridden if you want to have an idle
   task. 

   The two ways of constructing a telnet server are for a single-threaded
   server, and a multi-threaded server.


   <B> User overridables </B> <br>namespace ssobjects
   processSingleMsg() <br>
   idle() <br>



   <B> Components </B> <br>
   o Message Pump <br>
   o Idler        <br>


   <B> Message Pump </B> <br> 

   The message pump is the way the server talks to your code. When data comes
   into the server, it is first parsed into lines of data (searching for "\r\n"
   sequences, and a PacketMessage is created that references the data and the
   socket that it was sent from. It is then posted to the message que. Once
   there, processSingleMsg is called for every line of text that was parsed from
   the incoming data. This is similar to the SimpleServer, except that the read
   data is a string. It is however still put into a PacketBuffer to make working
   with the message pump the same as the SimpleServer. 

   This is where your code comes into play. processSingleMsg will be called with
   a packet command of:

     PacketBuffer::pcNewConnection when a new connection is made to your server.

     PacketBuffer::pcClosed when the connection is terminated.

     TelnetServerSocket::pcFullLine when a full line of text has been read.

   You check what the packet command was, and in the case of a
   TelnetServerSocket::pcFullLine you can then get the zero terminated string
   that was read in with:

     char* pszString = (char*)pmsg->packet()->getBuffer();

   You should not modify the contents of the string. If you need to modify it,
   make a copy and modify the copy. You would then perform some kind of
   operation, and perhaps send a reply back with
   TelnetServerSocket::print/println. This is the process of processing a
   clients request.

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
   When running multi-threaded, any shared data in your derived classes should
   be in the SimpleManager and passed into the simple server.  If you idle in
   the manager, you should idle the same way in the server. The simple server
   however should check if it is running multi threaded, and only idle shared
   data if it is running single threaded.
 **/
class TelnetServer : public ThreadHandler
{
  public:
    enum 
      {
      MIN_FREQ=10   ///< Used internally when calculating how much time has been spent waiting for data.
      };

  public:     //construction
    TelnetServer(const SockAddr& saBind,
                 const unsigned32 nFreq=1000,
                 const unsigned32 nMaxConnections=100);   ///< Single-threaded constructor.
    TelnetServer(TelnetServerSocket* const psocket,
                 const unsigned32 nFreq);                 ///< Multi-threaded constructor
    virtual ~TelnetServer();
        
  protected:  //user overridables
    /** 
        \brief Override this to process data that come in from clients. 

        The server will call this method when there was enough data to have
        parsed it into a full line and needs to have the data processed. A PacketMessage
        object is created to contain both the line read and client socket that send
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
          TelnetServerSocket* psocket = (TelnetServerSocket*)pmsg->socket();
          PacketBuffer* ppacket = pmsg->packet();
          switch(ppacket->getCmd())
          {
            //One way to handle the message. Process and reply within the switch.
            case PacketBuffer::pcNewConnection:   onNewConnection(pmsg);   break;
            case PacketBuffer::pcClosed:          onClosed(pmsg);          break;
            case TelnetServerSocket::pcFullLine:  onFullLine(pmsg);        break;
          }
          DELETE_NULL(ppacket);   //IMPORTANT! The packet is no longer needed. You must delete it.
        }
        ...
        void
        YourServer::onFullLine(PacketMessage* pmsg)
        {
          TelnetServerSocket* psocket = (TelnetServerSocket*)pmsg->socket();
          char* pszString = (char*)pmsg->packet()->getBuffer();
          processCmd(psocket,pszString);
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
    virtual void    sendString(TelnetServerSocket* psocket,const char* pszString);  ///< Sends a packet over a socket.
    virtual void    send2All(const char* pszString);                             ///< Sends a packet to all client sockets connected to the server.
    virtual void    removeSocket(TelnetServerSocket* psocket);                   ///< Closes and removes a socket from the server.

  public:
    static bool     canBind(SockAddr& saBind);                                  ///< Checks if we are able to bind. good for when you are about to start the server
    unsigned32      getTicks();                                                 ///< System timer ticks.
    bool            isThreaded() { return m_bUsingThread; }                     ///< Check if server is in single or multi-threaded mode.

    //nothing servicable beyond this point
  protected:
    threadReturn    ThreadHandlerProc(void);                //main handler routine
    void            processMessages();
    bool            processSockets(int iReady);
    void            processSelectError();
    virtual void    acceptConnection();
    int             getMaxFD();                             //return highest connected sock fd
    long            getSleepTime();                         //how many usecs to sleep according to idle frequency
    void            addMsg(PacketMessage* pmsg) { m_que.add(pmsg); }
    void            queClosedMessage(TelnetServerSocket* psocketRemoving);

  private:    //unused overrides
    TelnetServer(const TelnetServer& server);
    TelnetServer& operator=(const TelnetServer& server);
        
  protected:
    SocketInstance          m_sListen;
    SockAddr                m_saServer;
    LinkedList<TelnetServerSocket>    m_listClients;
    PacketMessageQue        m_que;
    unsigned32              m_nIdleFrequency;       //how often idle will be called
    unsigned32              m_nMaxCon;
    bool                    m_bUsingThread;
    bool                    m_bPause;      
    unsigned32              m_nSleepTime;
    struct timeval          m_tvServerStarted;
    fd_set                  m_rset,m_wset;
};

#endif //TELNETSERVER_H
