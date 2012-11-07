/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  December 17, 2000
       filename :  simplemanager.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
       purpose  :  Abstract class that Manages SimpleServer objects.
                   Management is done so you can have multi threaded 
                   or single threaded servers.

*********************************************************************/

#ifndef SIMPLEMANAGER_H
#define SIMPLEMANAGER_H

#include "ssobjects.h"
#include "simpleserver.h"
#include "serversocket.h"
#include "linkedlist.h"

#ifdef _WIN32
# include <time.h>
#else
# include <sys/time.h>
# include <unistd.h>
#endif

namespace ssobjects
{

class SimpleServer;

class SimpleManager : public ThreadHandler
{
  protected:
    //attributes
    LinkedList<SimpleServer>  m_listSimpleServers;    //list of the handlers that process messages for a connection
    LinkedList<ServerSocket>  m_listClientSockets;    //list of the clients sockets that are connected to server
    SocketInstance          m_sListen;
    SockAddr                m_saBind;
    unsigned32              m_nIdleFrequency;       //how often idle will be called
    unsigned32              m_nMaxConnections;
    bool                    m_bUsingThreads;
    bool                    m_bPause;      
    //    CSimpleTime             m_serverStartTime;
    //    CSimpleDate             m_serverStartDate;

  public:
    //construction
    SimpleManager(    
      const SockAddr& saBind,
      const bool bUseThread,
      const unsigned32 nFreq,
      const unsigned32 nMaxConnections);
    virtual ~SimpleManager();

    //methods
    virtual void    startServer();
    virtual void    idle() {};         //called every frequency
    void            waitFinish();              //sits here till the server is finished
    bool            isThreaded() { return m_bUsingThreads; }
    unsigned32      idleFrequency() const { return m_nIdleFrequency; }
    unsigned32      maxConnections() const { return m_nMaxConnections; }
    //    CSimpleTime&    startTime() { return m_serverStartTime; }
    //    CSimpleDate&    startDate() { return m_serverStartDate; }
    SockAddr&       bindAddr() { return m_saBind; }

  protected:
    threadReturn    ThreadHandlerProc(void);
    void            acceptConnection();

    //pure virtual user overridable 
    virtual SimpleServer* createServerInstance(ServerSocket* const psocket) = 0;
};

};

#endif  //SIMPLEMANAGER_H

