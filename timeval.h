/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  
       filename :  timeval.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  wrapper for time val structure. Also stores it's 
                   original value, so it can be reset without having 
                   to set the values again.

       notes    :  
*********************************************************************/

#ifndef TIMEVAL_H
#define TIMEVAL_H

#ifdef _WIN32
# include <time.h>
#else
# include <sys/time.h>
# include <unistd.h>
#endif

#include <sys/types.h>

#include "ssobjects.h"

namespace ssobjects
{

typedef struct timeval* PTIMEVAL;

class TimeVal : public timeval
{
  protected:
    timeval m_timeval;  //stored time val

  public:
    TimeVal() /*: m_timeval()*/
    {
      tv_sec=0;
      tv_usec=0;
    }

    TimeVal(signed32 seconds,signed32 milliseconds)
      : m_timeval()
    { 
      tv_sec=0;
      tv_usec=0;
      setTime(seconds,milliseconds); 
    }
        
    void setTime(signed32 seconds,signed32 milliseconds)
    { 
      tv_sec  = seconds;
      tv_usec = milliseconds*1000;
      m_timeval = *this;
    }

    void reset()
    {
      tv_sec  = m_timeval.tv_sec; 
      tv_usec = m_timeval.tv_usec; 
    }

    PTIMEVAL timevalptr() { return (PTIMEVAL)&m_timeval; }
    operator PTIMEVAL() { return (PTIMEVAL)this; }
};

};

#endif

