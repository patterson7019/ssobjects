ssobjects Version 1.01.01 September 25, 2001
Copyright (c) 2006, Lee Patterson & Ant Works Software

Web   : http://ssobjects.sourceforge.net
Email : workerant@users.sourceforge.net

See LICENSE for details on using this source.
See CHANGELOG.txt for list of changes.


C O N T E N T S:

1. What is ssobjects?
1.1 Status of ssobjects
2. File structure
3. Building
  3.1 Under Linux
  3.2 Under Win32
  3.3 Linking
    3.3.1 Linking to the Libraries Linux
    3.3.2 Linking to the Libraries Win32
  3.4 Pthreads
4. Using ssobjects
  4.1 Examples
  4.2 Tutorials




1. What is ssobjects?
Simple Server Objects (ssobjects) are a set of classes meant to take the 
chore out of writing server and client applications that use TCP/IP for 
communicating. It provides a consistent and stable interface. All of the grunt 
work of polling or selecting sockets, deciding what socket sent what, calling 
send() again if all data didn't get sent etc, is all taken care of by ssobjects. 
ssobjects was designed to be cross platform compatible. You can currently compile 
ssobjects on Linux and on Windows. This makes developing client apps on windows and 
server apps on Linux a breeze.  The goal of ssobjects is to provide a consistent, 
and easy to use interface, to be well documented, and widely used. 

1.1 Status of ssobjects
ssobjects is currently stable, and in use with minor modifications at a number of 
companies. This is the first public release of ssobjects. No major overhauls are 
planed for this version. The source is fairly tidy and easy to understand at 
this point, but house cleaning is underway. Only fixes and house cleaning will 
done to this version. 



2. File Structure
ssobjects     Contains all source and makefile
 |
 +-docs      Html & man page documentation
 |
 +-obj       Once libraries have been built, 
 |           all object files are placed in 
 |           here
 +-win32     Contains project files for 
 |           building with MS Dev Studio
 +-examples  Contains examples that use ssobjects


3. Building
ssobjects currently compiles on Mandrake Linux 7.1 & 8.0, Microsoft VC 5.0 & 6.0. 
Every attempt has been made to keep the source exactly the same for both 
operating systems, but some differences had to be resolved with "ifdef _WIN32" 
blocks. SEE PTHREAD SECTION NOTE FOR WINDOWS.

 
3.1 Building under Linux
Change to the ssobjects directory. Type:

1) % make clean
2) % make cleanall
3) % make tidy
4) % make debug
5) % make release
6) % make all
7) % make install

#1 will remove all lib files and object files but will leave the dependency 
file.

#2 will remove everything in 1 plus the dependency file.

#3 will remove object files.

#4 & 5 will build a debug and release version of the libs. Debug libs have gdb 
info, and are named like "ccsocket_dbg.a".

#6 Builds both the debug and release

#7 Builds both debug and release and installs by default to /usr/local/develop. 
To change this location, modify the INSTALL_DIRECTORY location in the make file. 
THIS DIRECTORY MUST EXIST BEFORE INSTALLING.


3.2 Building under Windows
Start developer studio. Open the dsp file ssobjects/win32 directory. Easiest 
thing to do is compile batch mode for release and debug.

3.3 Linking
3.3.1 Linking to the Libraries Linux
Link to the ssobjects library as well as pthread.

3.3.2 Linking to the Libraries Win32
Link to the ssobjects library as well as wsock32.lib, winmm.lib pthreadVCE.lib (or 
depending on the type of exception handling you have chosen. Also be sure the 
pthread dll is in your path. See the readme file that comes with pthreads for more 
information on choosing exception handling.

3.4 Pthreads
Note that threads were implemented using PThreads for both Linux and Windows. 
You can download PThreads for win32 at 

  http://sources.redhat.com/pthreads-win32/



4. Using ssobjects
This section is a little sparse at the moment. See the examples section for 
samples on using the libraries. Tutorials are planned shortly after the 
libraries have been documented properly.

4.1 Examples
See README.txt in the examples folder.

4.2 Tutorials
Tutorials are planned once ssobjects is properly documented.

