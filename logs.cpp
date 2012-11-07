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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "ssobjects.h"
#include "logs.h"

using namespace ssobjects;

char* logs::m_pszFileName = NULL;
char logs::m_szFuncName[max_funcs][max_func_len];
bool  logs::m_bActive = false;
bool logs::m_bInited = false;    
unsigned logs::m_nTail = 0;
unsigned logs::m_nFlags = L_CONSOLE;
//CMclCritSec logs::m_cs;

//LBP (09/21/99)
//
//  Get the logs class ready to use. Sets a filename to 
//  use for logging.
//
void logs::init(const char *pszLogFileName)
{
  //CMclAutoLock lock(m_cs);
  assert(strlen(pszLogFileName) < _MAX_PATH-9); //need enough space to append "-mmdd.log"

  m_bInited=true;
  m_pszFileName = new char[_MAX_PATH];
  memset(m_szFuncName,0,sizeof m_szFuncName);
  strncpy(m_pszFileName,pszLogFileName,_MAX_PATH-9);  //leave us enough room to append date and ".log"
  m_pszFileName[_MAX_PATH-9] = '\0';  //make sure we are null terminated
  m_nTail = 0;
#ifdef DEBUG
  m_bActive = true;   //default to turning on the logs
#endif
}

//LBP (09/21/99)
//
//  Frees memory.
//
void logs::deinit()
{
  //CMclAutoLock lock(m_cs);
  delete m_pszFileName;
  m_pszFileName = NULL;
  memset(m_szFuncName,0,sizeof m_szFuncName);
  m_nTail = 0;
  m_bActive = false;
  m_bInited=false;
  m_nFlags = 0;
}

//LBP (09/21/99)
//
//  Turns ALL logging off or on.
//
void logs::enable(bool bEnable)
{
  UNUSED_ALWAYS(bEnable);
  set(L_FILE | L_CONSOLE);
}

void logs::set(unsigned nSet,unsigned nReset)
{
//  acAutoLock l(m_cs);
  m_nFlags&=~nReset;
  m_nFlags|=nSet;
}

//LBP (09/21/99)
//
//  Logs to the console screen and a log file if one has been defined.
//  You can log to the console and not have a file name okay.
//
void logs::log(const char* fmt,...)
{
  //CMclAutoLock lock(m_cs);
  if(m_nFlags)
  {
    char buffer[1024];
    char timebuf[23]; //room only for "09/00/00 21:00\0"
    char filename[_MAX_PATH];
    va_list marker;
    time_t mytime;
    mytime = time(NULL);
    struct tm *today;
    today = localtime(&mytime);

    va_start(marker,fmt);
    vsnprintf(buffer,sizeof buffer,fmt,marker);
#ifdef _WIN32
    NULL_TERMINATE(buffer,sizeof buffer);
#endif
    strftime(timebuf,sizeof timebuf,"%m%d",today);
    sprintf(filename,"%s-%s.log",m_pszFileName,timebuf);
    NULL_TERMINATE(filename,sizeof filename);
    strftime(timebuf,sizeof timebuf,"%m/%d %H:%M:%S",today);
    if(m_nFlags&L_CONSOLE)
    {
      printf("%s: %s",timebuf,buffer);
    }
    if(m_nFlags&L_FILE)
    {
      if(m_pszFileName)
      {
        FILE* fp = fopen(filename,"a");
        if(fp)
        {
          fprintf(fp,"%s: %s",timebuf,buffer);
          fclose(fp);
        }
      }
    }
  }
}

void logs::logln(const char* fmt,...)
{
  //CMclAutoLock lock(m_cs);
  if(m_nFlags)
  {
    char buffer[1024];
    char timebuf[23]; //room only for "09/00/00 21:00\0"
    char filename[_MAX_PATH];
    va_list marker;
    time_t mytime;
    mytime = time(NULL);
    struct tm *today;
    today = localtime(&mytime);

    va_start(marker,fmt);
    vsnprintf(buffer,sizeof buffer,fmt,marker);
#ifdef _WIN32
    NULL_TERMINATE(buffer,sizeof buffer);
#endif
    strftime(timebuf,sizeof timebuf,"%m%d",today);
    sprintf(filename,"%s-%s.log",m_pszFileName,timebuf);
    NULL_TERMINATE(filename,sizeof filename);
    strftime(timebuf,sizeof timebuf,"%m/%d %H:%M:%S",today);
    if(m_nFlags&L_CONSOLE)
    {
      printf("%s: %s\n",timebuf,buffer);
    }
    if(m_nFlags&L_FILE)
    {
      if(m_pszFileName)
      {
        FILE* fp = fopen(filename,"a");
        if(fp)
        {
          fprintf(fp,"%s: %s\n",timebuf,buffer);
          fclose(fp);
        }
      }
    }
  }
}

//LBP (09/21/99)
//
//  Constructor will log the entry of the function it is declaired in.
//
logs::logs(const char *pFuncName)
{
  //CMclAutoLock lock(m_cs);
  if(m_szFuncName && m_bActive)
  {
    strncpy(m_szFuncName[m_nTail],pFuncName,sizeof(m_szFuncName[m_nTail]));
    NULL_TERMINATE(m_szFuncName[m_nTail],sizeof(m_szFuncName[m_nTail]));

    logln("%s ENTER",m_szFuncName[m_nTail]);
    if(++m_nTail>=max_funcs)
    {
      logln("MAX FUNC DEPTH REATCHED!");
      m_nTail=max_funcs;
    }
  }
}

void logs::dump(void* pMemory,unsigned32 nNumBytes)
{
  //CMclAutoLock lock(m_cs);
  unsigned char* p=(unsigned char*)pMemory;
  unsigned char* phold=p;
  char buffer[160];
  char output[160];
  unsigned nCol=0;
  memset(output,0,sizeof output);
  while(nNumBytes)
  {
    sprintf(buffer,"%02X",*p);
    strcat(output,buffer);
    if(++nCol<16)
    {
      strcat(output," ");
    }
    else
    {
      strcat(output,"  ");
      for( ;phold <= p; phold++)
      {
        if(isPrintable(*phold))
        {
          if('%' == *phold)
            strcpy(buffer,"%%");
          else
            snprintf(buffer,sizeof buffer,"%c",*phold);
        }
        else
          strcpy(buffer,".");

        strcat(output,buffer);
      }
      logln(output);
      nCol=0;
      memset(output,0,sizeof output);
    }
    nNumBytes--;
    p++;
  }
  if(nCol)
  {
    //finish off the line
    while(++nCol<16)
      strcat(output,"   ");
    strcat(output,"    ");
    for( ;phold < p; phold++)
    {
      if(isPrintable(*phold))
      {
        if('%' == *phold)
          strcpy(buffer,"%%");
        else
          snprintf(buffer,sizeof buffer,"%c",*phold);
      }
      else
        strcpy(buffer,".");

      strcat(output,buffer);
    }
    logln(output);
  }
}

//
// if this char is printable, meaning alpha numberic or printable 
// symbol, return true
//
bool
logs::isPrintable(int c)
{
  if(c >= 0x20 && c <= 0x7E)
    return true;
  return false;
}

//LBP (09/21/99)
//
//  Destructor will log the returning of the function it is declaired in.
//  This will happen no matter how the function is returned from.
//
logs::~logs()
{
  if(m_szFuncName && m_bActive)
  {
    logln("%s RETURNING",m_szFuncName[--m_nTail]);
  }
}
