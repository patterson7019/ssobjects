/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclcritsec.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Supplies functionality found in MclThread win32 libs. Not all
                   functionality has been brought across. The critical section
                   is impliemented as a mutex as Linux doesn't have a kernel
                   CRITICAL section. This is to maintain compatibility with
                   existing code.
*********************************************************************/

#ifndef __CMCLCRITSEC_H__
#define __CMCLCRITSEC_H__

//#include "msdefs.h"
# include <pthread.h>

namespace ssobjects
{

class CMclCritSec 
{
  public:
    // constructor creates a CRITICAL_SECTION inside
    // the C++ object...
    CMclCritSec();
    
    // destructor...
    virtual ~CMclCritSec();

    // enter the critical section...
    void Enter(void);

    // leave the critical section...
    void Leave(void);

  private:
    pthread_mutex_t m_mutexCritSec;
};

};

#endif

