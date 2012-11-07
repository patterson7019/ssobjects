/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       filename :  servermanager.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "ssobjects.h"
#include "threadutils.h"
#include "simpleserver.h"
#include "linkedlist.h"
#include "simpletime.h"
#include "SimpleDate.h"

#ifdef _WIN32
# include <time.h>
#else
# include <sys/time.h>
# include <unistd.h>
#endif

namespace ssobjects
{

class ServerManager : public ThreadHandler
{
  public:
    enum {MIN_FREQ=10};
  protected:
    SocketInstance          m_sListen;
    SockAddr              m_saServer;             //what interface and port to bind to
    LinkedList<SimpleServer>    m_listSimpleServers;    //list of the handlers that process messages for a connection
    unsigned32              m_nIdleFrequency;       //how often idle will be called
    unsigned32              m_nMaxConnections;      //how many connections are allowed
    bool                    m_bUsingThread;
    bool                    m_bPause;      
    //    CSimpleTime             m_serverStartTime;      //when the server started
    //    CSimpleDate             m_serverStartDate;      //when the server started

  public:
    ServerManager(
      const SockAddr& saBind,
      const unsigned32 nFreq,
      const unsigned32 nMaxConnections,
      const bool bUseThread);
    virtual ~ServerManager();

    void startServer();
    bool canBind();
    virtual threadReturn ThreadHandlerProc(void);
    void acceptConnection(SocketInstance& sListen);

    //pure vurtial method in order to create an instance of a simple server
    virtual SimpleServer* createServerInstance(
      ServerSocket* const psocket,
      const SockAddr& saClient,
      const unsigned32 nFreq) = 0;

};

};

#endif //SERVERMANAGER_H

