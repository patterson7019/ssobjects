/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclevent.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#include <pthread.h>
#include <errno.h>

#ifndef _WIN32
# include <sys/time.h>
# include <unistd.h>
#endif

#include "mclglobals.h"
#include "mclevent.h"
#include "gettimeofday.h"

using namespace ssobjects;

// constructor creates an event object...
CMclEvent::CMclEvent(bool bAutoSignal)
  : m_condEvent(),
    m_mutexEvent(),
    m_mutexSignal(),
    m_bSignaled(false),
    m_bAutoSignal(bAutoSignal)
{
  pthread_mutex_init(&m_mutexSignal,NULL);
  pthread_mutex_init(&m_mutexEvent,NULL);
  pthread_cond_init(&m_condEvent,NULL);
}

CMclEvent::~CMclEvent()
{
  pthread_mutex_destroy(&m_mutexEvent);
  pthread_mutex_destroy(&m_mutexSignal);
  pthread_cond_destroy(&m_condEvent);
}

// operations on event object...

//  Will wait for specified number of milliseconds.
//
//  RETURNS: 
//
//      WAIT_TIMEOUT        - the event was not signaled within the specified time
//      WAIT_OBJECT_0       - the event was signaled 
//
DWORD CMclEvent::Wait(DWORD dwTime)
{
  DWORD dwReturn = WAIT_OBJECT_0;
    
  pthread_mutex_lock(&m_mutexEvent);
  pthread_mutex_lock(&m_mutexSignal);
  bool bSignaled = m_bSignaled;
  pthread_mutex_unlock(&m_mutexSignal);

  if(!bSignaled)
  {
    if(INFINITE == dwTime)
    {
      pthread_cond_wait(&m_condEvent,&m_mutexEvent);
    }
    else
    {
      struct timeval now;
      struct timespec timeout;
      int nanosec,sec;
      int retcode;
        
      gettimeofday(&now,NULL);

      //calculate the number of seconds and nanoseconds 
      sec = dwTime/1000;
      timeout.tv_sec = now.tv_sec + sec;      //milliseconds to seconds

      //calculate the remaining time less the number of seconds
      nanosec = (dwTime - sec*1000) * ONEMILLION;
      timeout.tv_nsec = now.tv_usec * 1000 + nanosec;
      if(timeout.tv_nsec >= ONEBILLION)
      {
        timeout.tv_sec++;
        timeout.tv_nsec -= ONEBILLION;
      }
      retcode = pthread_cond_timedwait(&m_condEvent,&m_mutexEvent,&timeout);
      if(ETIMEDOUT == retcode)
        dwReturn = WAIT_TIMEOUT;
    }
  }

  autoResetSignal();
  pthread_mutex_unlock(&m_mutexEvent);

  return dwReturn;
}

//we reset the signal if the signal should be automatically reset
void CMclEvent::autoResetSignal()
{
  pthread_mutex_lock(&m_mutexSignal);
  if(m_bAutoSignal)
    m_bSignaled=false;
  pthread_mutex_unlock(&m_mutexSignal);
}

BOOL CMclEvent::Set(void) 
{
  pthread_mutex_lock(&m_mutexSignal);
  m_bSignaled=true;
  pthread_cond_signal(&m_condEvent);
  pthread_mutex_unlock(&m_mutexSignal);
  return TRUE;
}

BOOL CMclEvent::Reset(void) 
{
  
  pthread_mutex_lock(&m_mutexSignal);
  m_bSignaled=false;
  pthread_mutex_unlock(&m_mutexSignal);
  return TRUE;
}

BOOL CMclEvent::Pulse(void) 
{
  return FALSE;
}

