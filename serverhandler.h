/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  November 13, 2000
       filename :  serverhandler.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Sample of how to write a server using communicable
                   classes.

                   Server handler is an object that handles one type
                   of session. For instance, in a server where connections
                   could come from a client or from another server,
                   the way it is handled may be radically different.

*********************************************************************/

#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

namespace ssobjects
{

class PacketMessage;
class SimpleServer;
class ServerSocket;

//
// abstract class to impliment a server handler
//
class ServerHandler 
{
  protected:  //attributes
    ServerSocket*   m_psocket;
    SimpleServer* m_pserver;

  public:     //user overridables
    virtual void processMsg(PacketMessage* pmsg) = 0;

  public:     //methods
    ServerSocket*   mysocket() {return m_psocket;}
    SimpleServer* server() {return m_pserver;}

  public:     //construction
    ServerHandler(ServerSocket* psocket,SimpleServer* pserver);
    virtual ~ServerHandler();

  private:
    //only copies the pointers, as we don't create servsockets or 
    //simple servers here.
    ServerHandler(const ServerHandler& handler)
      : m_psocket(handler.m_psocket),m_pserver(handler.m_pserver) {}
    ServerHandler& operator=(const ServerHandler& handler)
    {
      if(&handler==this)
        return *this;
      m_psocket = handler.m_psocket;
      m_pserver = handler.m_pserver;
      return *this;
    }
};

};

#endif
