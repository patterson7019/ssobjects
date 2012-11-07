/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  2/15/2000
        filename :  threadutils.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Wrapper class for threads

*********************************************************************/

/*
impliments:
        ThreadHandler
        RWLock
        Lock
        AutoLock
        Event
*/

#ifndef THREADUTILS_H
#define THREADUTILS_H

#include "mcl.h"
#include "defs.h"

namespace ssobjects
{

//TODO: need a way to wait for this thread to finish. using detach currently, so I can't join
//TODO  to do synchronizing. Should be able to just call something like wait() and have a time-
//TODO  out
class ThreadHandler : public CMclThreadHandler
{
  public:
    ThreadHandler(bool bStart=false);
    void start();
    void stop();
    void setRunning(bool bRunning) {m_bRun=bRunning;}
    bool running() {return m_bRun;}
    threadReturn ThreadHandlerProc(void)=0;
    virtual ~ThreadHandler();

  protected:
    bool m_bRun;
    CMclThread* m_pThread;

  protected:
    //unused overloads
    ThreadHandler(const ThreadHandler&);
    ThreadHandler& operator=(const ThreadHandler&);
};

class Lock : public CMclCritSec
{
  public:
    void lock() {Enter();}
    void unlock() {Leave();}
};

class RWLock
{
  protected:
    unsigned32 m_nReadersReading;
    unsigned32 m_nWriterWriting;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_condLockFree;

  public:
    int  isLocked(){return m_nReadersReading||m_nWriterWriting;};
    int  isRLocked(){return m_nReadersReading;};
    int  isWLocked(){return m_nWriterWriting;};
    RWLock();
    ~RWLock();

    void rlock();
    void wlock();
    void wunlock();
    void runlock();
};

class AutoLock
{
  public:
    AutoLock(Lock& lock);
    ~AutoLock();
  protected:
    Lock* m_pcLock;
  private:
    //unused overloads
    AutoLock(const AutoLock&);
    AutoLock& operator=(const AutoLock&);
};

class Event : public CMclEvent
{
	public:
		Event(bool bAutoSignal=true) : CMclEvent(bAutoSignal){};
    virtual ~Event(){return;};
		bool signal() {return Set() ? true:false;}
		bool reset() {return Reset() ? true:false;}
		bool wait(unsigned int nMilliSeconds=INFINITE) {return Wait(nMilliSeconds)?true:false;}
};

};

#endif //THREADUTILS_H
