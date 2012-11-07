/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  gettimeofday.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       Win32 implimentation of gettimeofday(). If not running Windows, the unix version 
       of gettimeofday is used.

       \param ptv Time val structure to store the current time into.
       \param notused This parameter is not used, and is present to maintain compatibility with unix gettimeofday().

       \return Zero is always returned. Zero on unix means success, -1 for error.
*********************************************************************/

#ifdef _WIN32
#ifndef GETTIMEOFDAY_H
#define GETTIMEOFDAY_H

int gettimeofday(struct timeval* ptv,int);

#endif  //GETTIMEOFDAY_H
#endif  //_WIN32
