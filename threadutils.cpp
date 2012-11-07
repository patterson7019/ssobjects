/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  2/15/2000
        filename :  threadutils.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Wrapper class for threads
*********************************************************************/

#include <stdio.h>

#include "threadutils.h"

using namespace ssobjects;

ThreadHandler::ThreadHandler(bool bStart) : m_bRun(false),m_pThread(NULL)
{
  if(bStart)
    start();
}

ThreadHandler::~ThreadHandler()
{
  stop();
}

void ThreadHandler::start()
{
  m_bRun=true;
  m_pThread=new CMclThread(this);
}

void ThreadHandler::stop()
{
  if(m_pThread)
  {
    m_bRun=false;
    //m_pThread->Wait();    
    DELETE_NULL(m_pThread);
  }
}


RWLock::RWLock() : m_nReadersReading(0),m_nWriterWriting(0),m_mutex(),m_condLockFree()
{
  pthread_mutex_init(&m_mutex,NULL);
  pthread_cond_init(&m_condLockFree,NULL);
}

RWLock::~RWLock()
{
  pthread_cond_signal(&m_condLockFree);
  pthread_mutex_unlock(&m_mutex);

  //linux actually does nothing except checking that the mutex is unlocked.
  //see man page pthread_mutex(3) for details
  pthread_mutex_destroy(&m_mutex);
  pthread_cond_destroy(&m_condLockFree);
}

void
RWLock::rlock()
{
  pthread_mutex_lock(&m_mutex);
  while(m_nWriterWriting)
  {
    pthread_cond_wait(&m_condLockFree,&m_mutex);
  }
  m_nReadersReading++;
  pthread_mutex_unlock(&m_mutex);
}

void 
RWLock::runlock()
{
  pthread_mutex_lock(&m_mutex);
  if(!m_nReadersReading) 
    pthread_mutex_unlock(&m_mutex);
  else 
  {
    m_nReadersReading--;
    if (!m_nReadersReading) 
    {
      pthread_cond_signal(&m_condLockFree);
    }
    pthread_mutex_unlock(&m_mutex);
  }
}

void
RWLock::wlock()
{
  pthread_mutex_lock(&m_mutex);
  while(m_nReadersReading || m_nWriterWriting)
  {
    pthread_cond_wait(&m_condLockFree,&m_mutex);
  }
  m_nWriterWriting++;
  pthread_mutex_unlock(&m_mutex);
  
}

void
RWLock::wunlock()
{
  pthread_mutex_lock(&m_mutex);
  if(!m_nWriterWriting) 
  {
    pthread_mutex_unlock(&m_mutex);
  }
  else 
  {
    m_nWriterWriting = 0;
    pthread_cond_broadcast(&m_condLockFree);
    pthread_mutex_unlock(&m_mutex);
  }
}


AutoLock::AutoLock(Lock& lock) : m_pcLock(&lock)
{
  m_pcLock->lock();
}

AutoLock::~AutoLock()
{
  if(m_pcLock)
    m_pcLock->unlock();
}


