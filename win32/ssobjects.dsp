# Microsoft Developer Studio Project File - Name="ssobjects" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ssobjects - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ssobjects.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ssobjects.mak" CFG="ssobjects - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ssobjects - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ssobjects - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ssobjects"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ssobjects - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "DEBUG" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "PtW32NoCatchWarn" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\ssobjects.lib"

!ELSEIF  "$(CFG)" == "ssobjects - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "DEBUG" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "PtW32NoCatchWarn" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\ssobjects_dbg.lib"

!ENDIF 

# Begin Target

# Name "ssobjects - Win32 Release"
# Name "ssobjects - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\asyncclientconnector.cpp
# End Source File
# Begin Source File

SOURCE=..\bufferedsocket.cpp
# End Source File
# Begin Source File

SOURCE=..\clientconnector.cpp
# End Source File
# Begin Source File

SOURCE=..\cstr.cpp
# End Source File
# Begin Source File

SOURCE=..\file.cpp
# End Source File
# Begin Source File

SOURCE=..\flags.cpp
# End Source File
# Begin Source File

SOURCE=..\generalexception.cpp
# End Source File
# Begin Source File

SOURCE=..\gettimeofday.cpp
# End Source File
# Begin Source File

SOURCE=..\logs.cpp
# End Source File
# Begin Source File

SOURCE=..\mclautolock.cpp
# End Source File
# Begin Source File

SOURCE=..\mclcritsec.cpp
# End Source File
# Begin Source File

SOURCE=..\mclevent.cpp
# End Source File
# Begin Source File

SOURCE=..\mclthread.cpp
# End Source File
# Begin Source File

SOURCE=..\packetbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\packetmessageque.cpp
# End Source File
# Begin Source File

SOURCE=..\Parseit.cpp
# End Source File
# Begin Source File

SOURCE=..\serverhandler.cpp
# End Source File
# Begin Source File

SOURCE=..\serversocket.cpp
# End Source File
# Begin Source File

SOURCE=..\simplemanager.cpp
# End Source File
# Begin Source File

SOURCE=..\simpleserver.cpp
# End Source File
# Begin Source File

SOURCE=..\socketinstance.cpp
# End Source File
# Begin Source File

SOURCE=..\stopwatch.cpp
# End Source File
# Begin Source File

SOURCE=..\threadutils.cpp
# End Source File
# Begin Source File

SOURCE=..\tsleep.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\asyncclientconnector.h
# End Source File
# Begin Source File

SOURCE=..\bufferedsocket.h
# End Source File
# Begin Source File

SOURCE=..\clientconnector.h
# End Source File
# Begin Source File

SOURCE=..\cstr.h
# End Source File
# Begin Source File

SOURCE=..\defs.h
# End Source File
# Begin Source File

SOURCE=..\file.h
# End Source File
# Begin Source File

SOURCE=..\flags.h
# End Source File
# Begin Source File

SOURCE=..\generalexception.h
# End Source File
# Begin Source File

SOURCE=..\gettimeofday.h
# End Source File
# Begin Source File

SOURCE=..\linkedlist.h
# End Source File
# Begin Source File

SOURCE=..\logs.h
# End Source File
# Begin Source File

SOURCE=..\mcl.h
# End Source File
# Begin Source File

SOURCE=..\mclautolock.h
# End Source File
# Begin Source File

SOURCE=..\mclcritsec.h
# End Source File
# Begin Source File

SOURCE=..\mclevent.h
# End Source File
# Begin Source File

SOURCE=..\mclglobals.h
# End Source File
# Begin Source File

SOURCE=..\mclmutex.h
# End Source File
# Begin Source File

SOURCE=..\mclthread.h
# End Source File
# Begin Source File

SOURCE=..\msdefs.h
# End Source File
# Begin Source File

SOURCE=..\packetbuffer.h
# End Source File
# Begin Source File

SOURCE=..\packetmessageque.h
# End Source File
# Begin Source File

SOURCE=..\Parseit.h
# End Source File
# Begin Source File

SOURCE=..\serverhandler.h
# End Source File
# Begin Source File

SOURCE=..\serversocket.h
# End Source File
# Begin Source File

SOURCE=..\simplemanager.h
# End Source File
# Begin Source File

SOURCE=..\simpleserver.h
# End Source File
# Begin Source File

SOURCE=..\socketinstance.h
# End Source File
# Begin Source File

SOURCE=..\ssobjects.h
# End Source File
# Begin Source File

SOURCE=..\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\stopwatch.h
# End Source File
# Begin Source File

SOURCE=..\threadutils.h
# End Source File
# Begin Source File

SOURCE=..\timeval.h
# End Source File
# Begin Source File

SOURCE=..\tsleep.h
# End Source File
# End Group
# End Target
# End Project
