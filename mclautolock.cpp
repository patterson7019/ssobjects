/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen

       filename :  mclautolock.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#include "mclglobals.h"
#include "mclautolock.h"

using namespace ssobjects;

// constructors...
/*
CMclAutoLock::CMclAutoLock( CMclMutex & rCMclMutex) {
    m_pCritSec = NULL;
    m_pcCritSec = NULL;
    m_hMutexHandle = NULL;
    m_pcMutex = &rCMclMutex;
    m_pcMutex->Wait(INFINITE);
}
*/

CMclAutoLock::CMclAutoLock( CMclCritSec & rCMclCritSec) 
  : m_pcCritSec(&rCMclCritSec)
{
  m_pcCritSec->Enter();
}

// destructor...
CMclAutoLock::~CMclAutoLock(void) 
{

  /*
    BOOL bMutexStatus = TRUE;
    if (m_pcMutex) 
    {
    bMutexStatus = m_pcMutex->Release();
    }
  */
  if (m_pcCritSec) 
  {
    m_pcCritSec->Leave();
  }
  /*
    if (!bMutexStatus) {
    CMclThrowError( ::GetLastError());
    }
  */
}

