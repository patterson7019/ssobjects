/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  07/31/2000 02:00pm
       filename :  tsleep.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  puts the calling process or thread to sleep for 
                   specified number of milliseconds.
*********************************************************************/

#ifdef _WIN32
# include <time.h>
#else
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
#endif

#include "tsleep.h"

/**
   Sleeps for the specified number of milliseconds. Note that the amount of time
   slept is within about 10 milliseconds, usually under. 

   So if you request 100ms, the system may only sleep for 91ms. 

   There are 1000 milliseconds in a second.

   \param nMilliSeconds The number of milliseconds you want to sleep. 
**/
void tsleep(unsigned32 nMilliSeconds)
{
#ifdef _WIN32
  ::Sleep(nMilliSeconds);
#else
  struct timeval tv;
  tv.tv_sec=0;
  tv.tv_usec=nMilliSeconds*1000;  //convert to microseconds
  select(0,NULL,NULL,NULL,&tv);
#endif
}
