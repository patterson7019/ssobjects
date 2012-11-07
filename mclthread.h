/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclcritsec.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifndef MCLTHREAD_H
#define MCLTHREAD_H

#include "msdefs.h"

#include <pthread.h>

namespace ssobjects
{

typedef void*  threadReturn;
 

// forward declaration for CMclThreadHandler...
class CMclThread;

// CMclThreadHandler encapsulates the thread procedure for a 
// CMclThread object, each instantiation of a thread handler can
// be used by multiple threads at a time but the base CMclThreadHandler
// class does NOT provide any internal synchronization for multiple threads
// using a single instance simultaneously. A derived class could provide
// this internal synchronization however...
class CMclThreadHandler 
{
  public:
    // destructor does nothing, it is simply a placeholder for
    // ensure the destructors of derived classes are virtual...
    virtual ~CMclThreadHandler();

    // This is a pure virtual function with no implementation
    // it must be implemented in a derived class.
    // The "this" object
    // inside ThreadHandlerProc() will be the CMclThreadHandler derived
    // object itself.
    // The procedure should return the exit code of the thread when finished...
    virtual threadReturn ThreadHandlerProc(void) = 0;
};

class CMclThread 
{
  protected:
    CMclThreadHandler *m_pcThreadHandler;
    pthread_t m_Thread;
    DWORD m_dwStatus;

  public:

    // only the thread handler reference needs to 
    // be supplied since the other arguments have default values...
    CMclThread( CMclThreadHandler *pcThreadHandler);
    
    void Wait(DWORD dwMilliSeconds=INFINITE);
    
    //return the pthread 
    pthread_t GetThread() {return m_Thread;}

    // suspend the thread...
    DWORD Suspend(void);

    // resume the thread...
    DWORD Resume(void);

    // terminate the thread...
    BOOL Terminate( DWORD dwExitCode);

    // read a thread's exit code...
    BOOL GetExitCode( DWORD *pdwExitCode);

    // set a thread's priority...
    BOOL SetPriority( int nPriority);

    // read a thread's priority...
    int GetPriority(void);

    // get the internal thread id...
    DWORD GetThreadId(void);

  private:
    // this is a static function used to kick-start the thread handler...
    static void* CallThreadHandlerProc(void *pThreadHandler);
    CMclThread(const CMclThread&);
    CMclThread& operator=(const CMclThread&);
};

}; // namespace

#endif  //MCLTHREAD_H
