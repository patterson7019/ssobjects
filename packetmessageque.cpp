/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       filename :  packetmessageque.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#include "stdafx.h"
#include "packetmessageque.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace ssobjects;

unsigned32 PacketMessage::m_nUniqueID=0;
Lock       PacketMessage::m_lockID;

/**
   Returns a thread-safe unique id. Id's start at 0 and go to 0XFFFFFFFF
   at which point they wrap back to 0. 

   \return A unique id.
**/
unsigned32 
PacketMessage::uniqueID()
{
  AutoLock l(m_lockID);
  return m_nUniqueID++;
}


/**
   Constructs a message that will be used to place into the message que. 

   \param s Pointer to the socket that the data in p was read from.
   \param p Pointer to the packet object that was created from the data read from socket s. 
**/
PacketMessage::PacketMessage(SocketInstance* const s,PacketBuffer* const p)
  : m_nID(0),m_bUsed(false),m_pSocket(s),m_pPacket(p)
{
  m_nID = uniqueID();
}

/**
   Copy constructor.
**/
PacketMessage::PacketMessage(const PacketMessage& p)
  : m_nID(0),m_bUsed(p.m_bUsed),m_pSocket(p.m_pSocket),m_pPacket(p.m_pPacket)
{
  m_nID = uniqueID();
}

/**
   Destructs. Does not delete objects that were passed in. You are responsible
   for that. 
**/
PacketMessage::~PacketMessage()
{
}


/**
   Equals operator. If assigning to self, nothing is changed, but if 
   copying another packet, the unique id is updated to a new unique id.
**/
PacketMessage&
PacketMessage::operator=(const PacketMessage& p)
{
  if(&p != this)
  {
    m_nID = uniqueID();
    m_pPacket = p.m_pPacket;
    m_pSocket = p.m_pSocket;
    m_bUsed   = p.m_bUsed;
  }
  return *this;
}

/** 
    Constructs a message que. When events are set, whenever a message is added
    to the que, the event will fire. This event however is not in use at the
    moment. 

    \param bSetEvents Tells the message que to signal it's internal event when
    data is added to the que.
**/
PacketMessageQue::PacketMessageQue(bool bSetEvents)
  : m_bSetEvents(bSetEvents),m_listMsgs(),m_lock(),m_event()
{
  m_bSetEvents = bSetEvents;
}

/**
   Deletes all messages in this message que.
**/
PacketMessageQue::~PacketMessageQue()
{
  purge();
}

/**
   Add a message to the tail end of the message que. 

   If the event flag is set, the event will be signaled.

   \param pmsg The message object containing the socket and packet information
   you wish to add to the message que. 
**/
void PacketMessageQue::add(PacketMessage* pmsg)
{
  AutoLock l(m_lock);
  m_listMsgs.addTail(pmsg);
  if(m_bSetEvents)
    m_event.signal();
}

/**
   Checks if there are any messages in the que and returns head (first) message
   if there is one. If there is a message pointer to be returned, it is removed
   from the message que, so subsiquent calls will return the next one. Works
   like a FIFO stack.

   \return NULL if there are no messages in the que, a pointer to the PacketMessage
   object otherwise.
**/
PacketMessage* PacketMessageQue::get()
{
  AutoLock l(m_lock);
  PacketMessage* pmsg;
    
  pmsg = m_listMsgs.getHead();
  if(!pmsg)
    return NULL;

  m_listMsgs.removeHead();

  return pmsg;
}

/**
   \return true if there are any messages in the message que, false otherwise. 
**/
bool PacketMessageQue::isEmpty()
{
  AutoLock l(m_lock);
  if(m_listMsgs.getHead())
    return false;
  return true;
}

void PacketMessageQue::purge()
{
  m_listMsgs.purge();
}

#if 0
//not tested
unsigned32cMsgQue::wait(uint nMilliseconds)
{
  return m_event.wait(nMilliseconds);
}

bool PacketMessageQue::signal()
{
  AutoLock l(m_lock);
  if(m_event.signal())
    return true;
  return false;
}

bool PacketMessageQue::reset()
{
  AutoLock l(m_lock);
  if(m_event.reset())
    return true;
  return false;
}
#endif
