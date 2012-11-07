/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclevent.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifndef __CMCLEVENT_H__
#define __CMCLEVENT_H__

#include <pthread.h>
#include "msdefs.h"

namespace ssobjects
{

class CMclEvent
{
  private:
    enum
      {
      ONEMILLION = 1000000,
      ONEBILLION = 1000000000,
      };

  private:
    pthread_cond_t  m_condEvent;
    pthread_mutex_t m_mutexEvent;
    pthread_mutex_t m_mutexSignal;
    bool m_bSignaled;
    bool m_bAutoSignal;   //does the signal get set back to unsignaled once wait has completed?

  public:
    CMclEvent(bool bAutoSignal=true);
    virtual ~CMclEvent();

    // operations on event object...
    DWORD Wait(DWORD dwTime=INFINITE);      //time in milliseconds
    BOOL Set(void);
    BOOL Reset(void);
    BOOL Pulse(void);

  private:
    void autoResetSignal();
};

};

#endif  //__CMCLEVENT_H__
