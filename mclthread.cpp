/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclthread.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#include "ssobjects.h"
#include "mclglobals.h"
#include "mclthread.h"

using namespace ssobjects;

// destructor does nothing, it is simply a placeholder for
// ensure the destructors of derived classes are virtual...
CMclThreadHandler::~CMclThreadHandler() 
{
  return;
}

// constructor creates a thread C++ object and
// creates the kernel thread object which begins executing
// at the ThreadHandlerProc of the cThreadHandler object...
CMclThread::CMclThread( CMclThreadHandler *pcThreadHandler) : m_pcThreadHandler(pcThreadHandler),m_Thread(),m_dwStatus(0)
{
  // create the thread, call the class static function to forward
  // the thread on to the thread handler...
  if(pthread_create(&m_Thread,NULL,CallThreadHandlerProc,m_pcThreadHandler))
  {
    m_dwStatus = NO_ERROR;
    pthread_detach(m_Thread);
  }
  else 
  {
    // throw thread creation error...
    m_dwStatus = ERROR;
    //ThrowError(m_dwStatus);
  }      
}

threadReturn 
CMclThread::CallThreadHandlerProc(void *pThreadHandler) 
{
  // the constructor calls us here to start the thread, we need to
  // use the argument and convert it to a CMclThreadHandler so that
  // we can call the CMclThreadHandler::ThreadHandlerProc() as a member
  // function...
  CMclThreadHandler *pcHandler = static_cast<CMclThreadHandler *>(pThreadHandler);
    
  // call the handler procedure and return the exit code...
  threadReturn retval = pcHandler->ThreadHandlerProc();    
  //removed the delete, as the original mcl code does not have it
  //(see page 172 fo win32 multithreaded programming
  //delete pcHandler;
  return retval;
}
            
//wait for the thread to finish, the return
//  new: dw doesn't have any meaning
//  this is a replacement for a kernal object Wait call
void CMclThread::Wait(DWORD dw)
{
  UNUSED_ALWAYS(dw);
  //TODO: joining a thread won't work if you have detached it as I am in constructor
  pthread_join(m_Thread,NULL);
}

// suspend the thread...
DWORD CMclThread::Suspend(void) 
{
  return 0;//::SuspendThread( m_hHandle);
}

// resume the thread...
DWORD CMclThread::Resume(void) 
{
  return 0;//::ResumeThread( m_hHandle);
}

// terminate the thread...
BOOL CMclThread::Terminate( DWORD dwExitCode) 
{
  UNUSED_ALWAYS(dwExitCode);
  return 0;//::TerminateThread( m_hHandle, dwExitCode);
}

// read a thread's exit code...
BOOL CMclThread::GetExitCode( DWORD *pdwExitCode) 
{
  UNUSED_ALWAYS(pdwExitCode);
  return 0;//::GetExitCodeThread( m_hHandle, pdwExitCode);
}

// set a thread's priority...
BOOL CMclThread::SetPriority( int nPriority) 
{
  UNUSED_ALWAYS(nPriority);
  return 0;//::SetThreadPriority( m_hHandle, nPriority);
}

// read a thread's priority...
int CMclThread::GetPriority( void) 
{
  return 0;//::GetThreadPriority( m_hHandle);
}

// return the thread's identifier...
// no longer implimented
DWORD CMclThread::GetThreadId(void) 
{
  return 0;
}

