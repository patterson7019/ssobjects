/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       filename :  packetmessageque.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  This que is used to hold incoming messages. Each message
                   contains a pointer to the incoming packet, and a pointer to
                   the socket that input came from. Output will normally be 
                   sent to this socket.
*********************************************************************/

#ifndef PACKETMESSAGEQUE_H
#define PACKETMESSAGEQUE_H

#include "mcl.h"
#include "generalexception.h"
#include "packetbuffer.h"
#include "threadutils.h"
#include "socketinstance.h"
#include "linkedlist.h"

#define throwPacketMessageException(m) (throw PacketMessageException(m,__FILE__,__LINE__))


namespace ssobjects
{

/**
   \brief PacketMessage object errors.
**/ 
class PacketMessageException : public GeneralException
{
  public: // Constructor/destructor
    PacketMessageException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

/**
   \brief Container for a packet and socket.

   The PacketMessage object is created by the SimpleServer object, and is used to
   contain a pointer to the socket and packet data that was just read in. 

   \note
   The unique id is not thread-safe.
**/
class PacketMessage
{
  public:             //constructor / deconstructor
    PacketMessage(SocketInstance* const s,PacketBuffer* const p);
    virtual ~PacketMessage();

  public:             //functions
    PacketBuffer*         packet() const      ///< Pointer to the packet that was read in.
    {return m_pPacket;}
    
    SocketInstance*       socket() const      ///< Pointer to the socket that the packet data was read from.
    {return m_pSocket;}
    
    uint                  id()     const      ///< Unique id of this packet. 
    /** This is rarely used. */
    {return m_nID;}

  private:            //thread data and methods
    static unsigned32 m_nUniqueID;
    static Lock       m_lockID;
    unsigned32        uniqueID();

   private:            //regular data
    unsigned32              m_nID;
    bool                    m_bUsed;
    SocketInstance*         m_pSocket;
    PacketBuffer*           m_pPacket;

  private:
    //unused overloads
    PacketMessage(const PacketMessage&);
    PacketMessage& operator=(const PacketMessage&);
};

#define throwPacketMessageQueException(m) (throw PacketMessageQueException(m,__FILE__,__LINE__))

/**
   \brief PacketMessageQue object errors.
**/ 
class MsgQueException : public GeneralException
{
  public: // Constructor/destructor
    MsgQueException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};


/**
   \brief Thread-safe list of messages. 

   Works like a FIFO stack. Messages are
   added to the end of the list, and retrieved from the head of the list. 

   In the following example, we have added "Aa", "Bb" and "Cc" to the list in that
   order. 

   <PRE>
   ---[head]---
   [    Aa    ]
   [    Bb    ]
   [    Cc    ]
   ---[tail]---
   </PRE>

   When you call the get method, it will retrieve the head item - "Aa" - and
   remove it from the list. The next call will retrieve "Bb" and so on.

   The server uses the message que to store all the messages that have been
   parsed. It then runs through the que and calls your message pump (see
   SimpleServer::processSingleMsg for further details.)
**/
class PacketMessageQue 
{
  private:
    bool            m_bSetEvents; ///< Set if the event should be signaled.
    LinkedList<PacketMessage>   m_listMsgs;   ///< List that contains the messages.
    Lock            m_lock;       ///< Lock to keep things straight in a multi-threaded environment.
    Event           m_event;      ///< Signaled when a message is added to the que.

  public:
    PacketMessageQue(bool bSetEvents=true);
    virtual ~PacketMessageQue();

  public:
    void            add(PacketMessage *);   ///< Adds msg to the end of the list.
    PacketMessage*          get();          ///< Returns the first item, and removes from the list.
    bool            isEmpty();      ///< Checks if there are any messages in the message que.
    void            purge();        ///< Remove all items from the msg list que.
#if 0
    //not tested
    bool            signal();
    bool            reset();
    bool            pulse();
    unsigned32      wait(uint nMilliseconds=INFINITE);
#endif
  private:
    //unused overloads
    PacketMessageQue(const PacketMessageQue&);
    PacketMessageQue& operator=(const PacketMessageQue&);
};

};
#endif
