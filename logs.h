/********************************************************************
    Copyright (c) 2006, Lee Patterson & Ant Works Software
    http://ssobjects.sourceforge.net

    created  :      9/21/1999
    filename :      logs.h
    author   :      Lee Patterson (workerant@users.sourceforge.net)
    
    purpose  :      Enables the logging of a function being enter, and when 
                    the function returns, no matter how it returns.
              
                    Just make a call to         logs:Init("appname.log");
              
                    The define at the top of    logs func("myfunc");
                    your function like this:    
              
              
                    Example:
              
                        void myfunc()
                        {
                            logs func("myfunc");    //a logs contructor
                            //do something
                        }
              
                        void main()
                        {
                            logs::Init("mylog.log");            //set the log filename, init memory
                            logs::log("starting program");
                            myfunc();
                            logs::log("ending");
                            logs::Deinit();                     //free memory
                        }
              
              
                    This example will generate the following in the log file:
              
                        starting program
                        myfunc ENTER
                        myfunc RETURNING
                        ending

*********************************************************************/

#ifndef LOGS_H
#define LOGS_H

#include "msdefs.h"

namespace ssobjects
{
//TODO: need to add "ssobjects::" to defines

#define LOG ssobjects::logs::logln

#ifdef DEBUG
#define ENABLELOG ssobjects::logs::enable
#define INITLOG ssobjects::logs::init
#define DLOG ssobjects::logs::logln
#define DUMP ssobjects::logs::dump
#define FUNCLOG ssobjects::logs func
#else
#define DLOG 1 ? (void)0 : ssobjects::logs::log
#define FUNCLOG(x)
#define INITLOG(x)
#define ENABLELOG 1 ? (void)0 : ssobjects::logs::enable 
#define DUMP 1 ? (void)0 : ssobjects::logs::dump
#endif

class logs
{
  public:
    enum {L_FILE=1,L_CONSOLE=2,L_DEFAULT=3,L_NONE=0,L_ALL=0xFFFFFFFF};
    logs(const char *pFuncName);
    ~logs();

  public:
    static void log(const char* fmt,...);
    static void logln(const char* fmt,...);
    static void dump(void* pMemory,DWORD dwNumBytes);
    static void init(const char* pszLogFileName);
    static void deinit();
    static void enable(bool bEnable=true);
    static void set(unsigned nSet,unsigned nReset=0);

    enum{max_funcs=100,max_func_len=80};

  public:
    static bool m_bActive;
    static unsigned m_nFlags;
    static bool m_bInited;

  private:
    static bool isPrintable(int c);
    static unsigned m_nTail;
    static char *m_pszFileName;
    static char m_szFuncName[max_funcs][max_func_len];
};

};

#endif
