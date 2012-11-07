/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  November 13, 2000
       filename :  serverhandler.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Sample of how to write a server using communicable
                   classes.

                   Server handler is an object that handles one type
                   of session. For instance, in a server where connections
                   could come from a client or from another server,
                   the way it is handled may be radically different.

*********************************************************************/

#include <stdio.h>

#include "packetmessageque.h"
#include "serverhandler.h"

using namespace ssobjects;

ServerHandler::ServerHandler(ServerSocket* psocket,SimpleServer* pserver) 
  : m_psocket(psocket),m_pserver(pserver)
{
#ifdef DEBUG
  puts("server handler created");
#endif
}

ServerHandler::~ServerHandler()
{
#ifdef DEBUG
  puts("server handler destroyed");
#endif
}


