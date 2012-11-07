Copyright (c) 2001, Lee Patterson & Ant Works Software
http://ssobjects.sourceforge.net

See LICENSE for details on using this source.


Shows a compairison between using ssobjects and low level socket calls. 
Notice that the ssobjects version (daytimeserver.cpp) doesn't have to check
for errors after every socket call. Instead it is all handled in once
place in the catch.

A daytime server is something to connect to via telnet and it returns
the date and time on that system. 

COMPILING:
(Unix)
From the command line, type (assuming you have the ssobjects compiled in "../.."

     $ gcc -DOS_LINUX -I../.. -L../.. -W -Wall -o daytime1 daytimeserver.cpp -lssobjects
     $ gcc -o daytime2 daytimeserver.c
     
RUNNING:

     $ ./daytime1 (or) ./daytime2
     server listening on port 3333

Then to connect to the server, in another window type: 

     $ telnet localhost 3333
     Trying 127.0.0.1...
     Connected to localhost.localdomain.
     Escape character is '^]'.
-->  Fri Jul 20 20:31:12 2001
     Connection closed by foreign host.

Highlighted line shows the output from the daytime server.
