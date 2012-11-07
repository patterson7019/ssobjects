/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  gettimeofday.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifdef _WIN32
#include <windows.h>
#include <time.h>

int gettimeofday(struct timeval* ptv,int)
{
  unsigned today = timeGetTime();
  ptv->tv_sec = today/1000;
  //find the number of seconds
  ptv->tv_usec = (today - (ptv->tv_sec*1000)) * 1000;     //milliseconds leftover to microseconds
  return 0;
}
#endif
