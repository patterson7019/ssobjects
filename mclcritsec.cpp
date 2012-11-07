/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclcritsec.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#include "mclglobals.h"
#include "mclcritsec.h"

using namespace ssobjects;

// destructor...
CMclCritSec::~CMclCritSec() 
{
  pthread_mutex_destroy(&m_mutexCritSec);
}

// constructor creates a CRITICAL_SECTION inside
// the C++ object...
CMclCritSec::CMclCritSec()
  : m_mutexCritSec()
{
  pthread_mutex_init(&m_mutexCritSec,NULL);
}


// enter the critical section...
void CMclCritSec::Enter(void) 
{
  pthread_mutex_lock(&m_mutexCritSec);
}

// leave the critical section...
void CMclCritSec::Leave(void) 
{
  pthread_mutex_unlock(&m_mutexCritSec);
}


