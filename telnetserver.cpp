/********************************************************************
       Copyright (c) 2006, Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  December 3, 2001
       filename :  telnetserver.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)

*********************************************************************/

#include "telnetserver.h"
#include "telnetserversocket.h"
#include "serverhandler.h"
#include "stopwatch.h"

using namespace ssobjects;

/**
   This constructs the server object to run as a single threaded server. 

   [Win32] WSAStartup is called at this point. You should construct the server
   before calling canBind method.

   \param saBind Port and protocal address information you want to bind to. See
   TelnetServer::canBind for more details on using saBind.

   \param nFreq How often idle method will be called in milli-seconds.

   \param nMaxCon Reserved for future expansion.

   \throw TelnetServerException If nFreq is out of range.
   \throw GeneralException [Win32] If WSAStartup call failes.

   \todo
   Test passing in the protocol address.
**/
TelnetServer::TelnetServer(
  const SockAddr& saBind,
  const unsigned32 nFreq,
  const unsigned32 nMaxCon)
  : m_sListen(),
    m_saServer(saBind),
    m_listClients(),
    m_que(),
    m_nIdleFrequency(nFreq),
    m_nMaxCon(nMaxCon),
    m_bUsingThread(false),
    m_bPause(false),
    m_nSleepTime(0),
    m_tvServerStarted(),
    m_rset(),
    m_wset()
{
#ifdef _WIN32
  WSADATA wsd;
  if(WSAStartup(0x0101,&wsd)!=0)
    throwGeneralException("unable to start windows socket layer");
#endif
  if(m_nIdleFrequency < MIN_FREQ)
  {
    CStr msg;
    msg.format("Server frequency of %d is too fine. Minimum is %d.",nFreq,MIN_FREQ);
    throwTelnetServerException(msg);
  }
}

/**
   Creating a simple server as a handler for a socket. This constructor is used
   in the simple manager. The socket is already attached. This instance of a simple
   server is meant to be run multi-threaded. 

   \param psocket Client socket that is connected.

   \param nFreq idle frequency. (See note below.)

   \Todo
   Finish explaining how threading and idling is accomplished.
**/
TelnetServer::TelnetServer(TelnetServerSocket* const psocket,const unsigned32 nFreq)
  : m_sListen(),
    m_saServer(),
    m_listClients(),
    m_que(),
    m_nIdleFrequency(nFreq),
    m_nMaxCon(0),
    m_bUsingThread(true),
    m_bPause(false),
    m_nSleepTime(0),
    m_tvServerStarted(),
    m_rset(),
    m_wset()
{
  m_listClients.addTail(psocket);
}

/**
   Destroys the server object, closes the listening socket, removes all msg's
   in the message que, removes all client socket connects and closes them. 

   [Win32] Calls WSACleanup()
**/
TelnetServer::~TelnetServer()
{
  m_que.purge();              //remove all items in the list, and delete any packets that where in the que
  m_listClients.purge();      //remove all clients from the list, and delete the sockets
  m_sListen.close();          //close the listening socket
#ifdef _WIN32
  WSACleanup();
#endif
}

/**
   Creates a listen socket and tries binding it using the port information
   passed in. Once the connection has been bound, the connection is put into
   listen mode, and is then closed. Good for when you want to start the server
   in single threaded mode, but need to make sure that you were able to bind.

   When using the protocol address in saBind, this will bind this server to a specific IP
   address. Useful when the machine this server is running on has more then one IP address
   assigned to it. See the manpage on bind for more details.

   \param saBind Port and protocol address information you want to bind to. 
**/
bool
TelnetServer::canBind(SockAddr& saBind)
{
  SocketInstance s;
  try
  {
    s.create();
    s.bind(saBind);
    s.listen();
    s.close();
  }
  catch(GeneralException& e)
  {
    UNUSED_ALWAYS(e);
    s.cleanup();
    return false;
  }
  return true;
}

/**
   Creates the listen socket, binds to the address passed in at construction,
   and puts the listen socket into listen mode. 

   This method does not return until the server has stopped. The message pump
   and idler are your only access points after this call. You would typically
   do any setup you need to do before this is called. Once this returns, the
   server object can be destroyed.
**/
void
TelnetServer::startServer()
{
  gettimeofday(&m_tvServerStarted,NULL);

  m_sListen.create();
  m_sListen.bind(m_saServer);
  m_sListen.listen();

  setRunning(true);
  ThreadHandlerProc();    //call the main handler directly non-threaded
  setRunning(false);
}

void 
TelnetServer::pauseIncomingConnections()
{
  m_bPause=true;
}

void
TelnetServer::killServer()
{
  m_bRun = false;
  if(m_bUsingThread)
    stop();
}

/**
   \return (windows) number of ticks since windows started.
   (linux)  number of ticks since server app started
**/
unsigned32 
TelnetServer::getTicks()
{
#ifdef _WIN32
    
  //just return the multimedia timer value
  return timeGetTime();
    
#else
   
  //calculate how many ticks it has been since the server started
  struct timeval tv;
  unsigned32 nTicks;
  unsigned32 nSec,nMicroSec;

  gettimeofday(&tv,NULL);
  nSec = tv.tv_sec - m_tvServerStarted.tv_sec;
  nMicroSec = tv.tv_usec - m_tvServerStarted.tv_usec;
    
  //figure out how many microseconds, then convert to milliseconds
  nTicks = (nSec * 1000000 + nMicroSec)/1000;
    
  return nTicks;
    
#endif
}

//
//finds the highest socket number and sets the rset and wset bits
//from our list of clients and our listening socket. 
//
int 
TelnetServer::getMaxFD()
{
  int imax = m_sListen;
  TelnetServerSocket* s;

  FD_ZERO(&m_wset);
  FD_ZERO(&m_rset);
  if(!m_bUsingThread)
  {
    //when we are using threads, this simple server object is only 
    //paying attention to one socket. Other simple server objects
    //will pay attention to other sockets. Listen socket is taken
    //care of in the manager.
    FD_SET(m_sListen,&m_rset);      //select on listening socket
  }

  for(s = m_listClients.getHead(); s; s = m_listClients.getNext())
  {
    FD_SET(*s,&m_rset);          //select for readability

    if(s->getOutBufferSize())
      FD_SET(*s,&m_wset);      //need to send data out, select for writability

    if((int)(*s) > imax)
      imax = *s;
  }
  return imax;
}

//
//main worker function. called directly when in single thread, or 
//runs as a thread in multi threaded mode.
//
//PORTING NOTE: On Linux, tv is modified to reflect the amount of 
//time not slept; most other implementations do not do this. See help
//for more information
//
//TODO: need to double check that in a multi threaded environment, TelnetServer::idle 
//TODO  isn't called. Looks like it isn't, so why not?
threadReturn
TelnetServer::ThreadHandlerProc(void)
{
  struct timeval tv;
  int iReady;
  StopWatch timer;

  idle();                     //give idle a chance to go as soon as server is running
  tv.tv_sec = 1;
  tv.tv_usec = 0;//getSleepTime();
  timer.start();

  while(running())
  {
    int iMaxFD = getMaxFD();
    iReady = select(iMaxFD+1,&m_rset,NULL/*&m_wset*/,NULL,&tv);     //see porting note above for select behaviour
    if(running())
    {
      if(-1 == iReady)
        processSelectError(); 
      else if(iReady > 0)
      {
        if(processSockets(iReady))     //check for incoming data,socket closers,socket errors,new connections
          processMessages();
      }

      if(timer.milliseconds() >= (m_nIdleFrequency-MIN_FREQ) && running())     //20ms error range as a timeout isn't percise
      {
        idle();
        //tv.tv_sec = 0;
        //tv.tv_usec = getSleepTime();
        timer.start();
      }
    }
  }
  return 0;
}

long 
TelnetServer::getSleepTime()
{
  m_nSleepTime = m_nIdleFrequency * 1000;
  return m_nSleepTime;
}

void
TelnetServer::processSelectError()
{
  throwTelnetServerException(strerror(errno));
}

//
//
//runs through all messages, calling on processSingleMsg for each msg
//
void
TelnetServer::processMessages()
{
  PacketMessage* m;
  PacketBuffer* p;
  m = m_que.get();
  while(m)
  {
    processSingleMsg(m);
    p = m->packet();
//    delete p;
    delete m;                       //we are finished with the message
    m = m_que.get();
  }
}

//
//RETURNS true if there are any messages in the msg que (msg's will have been just added)
//
bool
TelnetServer::processSockets(int iReady)
{
//  FUNCLOG("TelnetServer::processSockets");

  //check for a new connection
  if(FD_ISSET(m_sListen,&m_rset))
  {
    acceptConnection();
    iReady--;
  }

  //loop through all sockets or until there are no more ready connections
  TelnetServerSocket* s;
  PacketMessage* pmsg;
  for(s=m_listClients.getHead(); s && iReady; s=m_listClients.getNext())
  {
    try
    {
      if(FD_ISSET(*s,&m_rset))
      {
        s->readData();     //reads into a buffer the serv socket has
        PacketBuffer* ppacket;
        while((ppacket = s->extractPacket()))
        {
          pmsg = new PacketMessage(s,ppacket);
          m_que.add(pmsg);
        }       
      }
      if(s)
      {
        if(FD_ISSET(*s,&m_wset))
          s->sendBuffer();
      }
    }
    catch(SocketInstanceException& e)
    {
      // There was a socket error, this guy should be shut down.
      UNUSED_ALWAYS(e);
      queClosedMessage(s);
      s->cleanup();
      m_listClients.removeCurrent(listPREV);
      DELETE_NULL(s);
    }
    catch(TelnetServerSocketException& e)
    {
      // An invalid packet was detected, this socket should be shut down.
      UNUSED_ALWAYS(e);
      queClosedMessage(s);
      s->cleanup();
      m_listClients.removeCurrent(listPREV);
      DELETE_NULL(s);
    }
  }
  return !m_que.isEmpty();         //return false if the que is empty instead of isEmpty()'s true 
}

void
TelnetServer::acceptConnection()
{
  TelnetServerSocket* pServSocket;
  SocketInstance sClient;
  SockAddr saClient;

  //TODO: put in a try/catch block
  m_sListen.accept(sClient,saClient);
  pServSocket = new TelnetServerSocket(sClient,saClient,8000,120);
  m_listClients.addTail(pServSocket);

  PacketBuffer* pktNew = new PacketBuffer(PacketBuffer::pcNewConnection);
  PacketMessage* pmsg = new PacketMessage(pServSocket,pktNew);
  m_que.add(pmsg);
}

/**
   Use this when you want to send a packet to all client connections.

   \param packet Packet you wish to send.

    \throw SocketInstanceException if the send operation had an error.
**/
void
TelnetServer::send2All(const char* pszString)
{
  TelnetServerSocket* s;
  for(s=m_listClients.getHead(); s; s=m_listClients.getNext())
  {
    sendString(s,pszString);
  }
}

/** 
    \param s Socket you are sending this packet to.
    \param ppacket Packet containing the data you are sending.

    \throw SocketInstanceException if the send operation had an error.
**/
void
TelnetServer::sendString(TelnetServerSocket* s,const char* pszString)
{
  if(s)
    try
    {
      s->sendString(pszString);
    }
    catch(SocketInstanceException& e)
    {
      //there was a send error
      UNUSED_ALWAYS(e);
      removeSocket(s);
      throw e;
    }
#ifdef DEBUG
  else
  {
    DLOG("WARNING: ignorning NULL socket");
  }
#endif
}

/**
   Removes this socket from our client list, deletes the object, and posts a
   message to say that this guy is gone. Once this function returns, the socket
   passed in can no longer be used. Undefined behavior will occur If you attempt
   to use the socket after a call to this function.

   You will receive a pcClosed message in a call to processSingleMsg. Included
   in the message is the socket number, and dotted decimal IP address that was
   attached to the socket. You should no longer use the socket number, or the
   socket object. 

   Typically, you store the socket number in a list somewhere so when a socket
   is closed, you will be able to find it and remove it from your user data.

   \param psocketRemoving Pointer to the socket object that should be removed.
**/
//
//TODO: make psocket const?
//
void
TelnetServer::removeSocket(TelnetServerSocket* psocketRemoving)
{
  //socket was closed, post remove msg to server, and remove from our server
  queClosedMessage(psocketRemoving);

  TelnetServerSocket* s;
  for(s=m_listClients.getHead(); s; s=m_listClients.getNext())
  {
    if(s == psocketRemoving)
    {
      m_listClients.removeCurrent(listPREV);
      s->cleanup();
      DELETE_NULL(s);
      return;
    }
  }
}


/**
   Creates and posts a PacketBuffer::pcClosed message to the server message
   que. The message contains the socket handle, and the ip address in dotted
   decimal format. The socket you pass in is the socket that you are
   removing. The socket should still be valid (not yet deleted) when this method
   is called.

   \param s Pointer to the socket that is being removed.
**/
void
TelnetServer::queClosedMessage(TelnetServerSocket* s)
{
  PacketBuffer* pktRemove = new PacketBuffer(PacketBuffer::pcClosed);
  *pktRemove << (signed32)   s->getSocket();
  *pktRemove << s->getAddr().dottedDecimal();
  pktRemove->rewind();
  PacketMessage* pmsg = new PacketMessage(s,pktRemove);         //socket is no longer valid
  m_que.add(pmsg);
}


//--------------------


