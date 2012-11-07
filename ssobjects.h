/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  3/10/2000
        filename :  ssobjects.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Include all standard lib headers.
*********************************************************************/

/**
   \mainpage ssobjects Documentation

   <a href="http://ssobjects.sourceforge.net">Home Page</a><P>

   <a href="http://sourceforge.net/projects/ssobjects">SourceForge Project
   Summary</a><P>

   Simple Server Objects (ssobjects) are a set of classes meant to take the
   chore out of writing server and client applications that use TCP/IP for
   communicating. It provides a consistent and stable interface. All of the grunt
   work of polling or selecting sockets, desiding what socket sent what, calling
   send() again if all data didn't get sent etc, is all taken care of by ssobjects. 

   ssobjects was designed to be cross plateform compatible. You can compile ssobjects on
   Linux and on Windows. This makes developing client apps on windows and server
   apps on Linux a breeze.
   
   The goal of ssobjects is to provide a consistant, and easy to use interface, to be
   well documented, and widely used.
**/

#ifndef SSOBJECTS_H
#define SSOBJECTS_H

#include "defs.h"
#include "generalexception.h"
#include "cstr.h"
#include "linkedlist.h"
#include "file.h"

#endif


