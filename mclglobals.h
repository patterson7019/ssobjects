/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from Win32 Multithreaded Programming
       Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring

       filename :  mclglobals.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifndef MCLGLOBALS_H
#define MCLGLOBALS_H

# ifndef _WIN32

#  ifndef NO_ERROR
#   define NO_ERROR    0
#  endif

#  ifndef ERROR
#   define ERROR       1
#  endif

#  ifndef INFINITE
#   define INFINITE    0
#  endif

#  define WAIT_OBJECT_0  1
#  define WAIT_FAILED    2
#  define WAIT_TIMEOUT   3
#  define WAIT_ABANDONED 4

# endif

#endif

