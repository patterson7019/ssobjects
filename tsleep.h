/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  07/31/2000 02:00pm
       filename :  tsleep.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  puts the calling process or thread to sleep for 
                   specified number of milliseconds.

       notes    : this function is acurate to about 5 milliseconds.
*********************************************************************/

#include "defs.h"

void tsleep(unsigned32 nMilliSeconds);  ///< Sleep a specified number of milliseconds.
