/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclautolock.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifndef __CMCLAUTOLOCK_H__
#define __CMCLAUTOLOCK_H__

//#include "mclmutex.h"
#include "mclcritsec.h"

namespace ssobjects
{

class CMclAutoLock 
{
  private:
    //CMclMutex *m_pcMutex;
    CMclCritSec *m_pcCritSec;

  public:
    // constructors...
    //CMclAutoLock( CMclMutex & rCMclMutex);
    CMclAutoLock( CMclCritSec & rCMclCritSec);

    // destructor...
    ~CMclAutoLock(void);

  private:
    //unused overloads
    CMclAutoLock(const CMclAutoLock&);  
    CMclAutoLock& operator=(const CMclAutoLock&);
};

};

#endif

