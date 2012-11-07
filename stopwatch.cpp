/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  September 4, 2001
        filename :  stopwatch.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)

*********************************************************************/


#include "stopwatch.h"

using namespace ssobjects;

StopWatch::StopWatch()
  : m_bRunning(false),
    m_tvStart(),
    m_tvStop()
{
  reset();
}


/**
   Starts the stopwatch timer. Clears any stored laps.
**/
void
StopWatch::start()
{
  gettimeofday(&m_tvStart,NULL);
  m_tvStop = m_tvStart;
  m_bRunning = true;
}


/**
   Stops the stop watch timer. Notes the time the timer was stopped.
**/
void 
StopWatch::stop()
{
  if(m_bRunning)
  {
    gettimeofday(&m_tvStop,NULL);
    m_bRunning = false;
  }
}


/**
   Checks if the stop watch is currently running. 

   \return true if the stop watch is currently active, false otherwise.
**/
bool
StopWatch::isRunning() const
{
  return m_bRunning;
}


/**
   If the stop watch is currently running, this will return the number of
   milli-seconds that have elapsed since the stop watch was started, till the
   current time. If the stop watch is stopped, this returns the number of
   milli-seconds from the time the stop watch was started, to the time the stop
   watch was stopped.

   If the timer has never started, the time returned will be zero.

   \return The number of milli-seconds the stop watch has counted.
**/
unsigned32
StopWatch::milliseconds() const
{
  if(isRunning())
  {
    struct timeval timeNow;
    gettimeofday(&timeNow,NULL);
    return elapsedMSecs(&m_tvStart,&timeNow);
  }
  return elapsedMSecs(&m_tvStart,&m_tvStop);
}


/**
   If the stop watch is currently running, this will return the number of
   seconds that have elapsed since the stop watch was started, till the current
   time. If the stop watch is stopped, this returns the number of seconds from
   the time the stop watch was started, to the time the stopwatch was
   stopped. The number of milliseconds is ignored. So if the elapsed time is
   1500 milliseconds, seconds() returns 1 second.

   If the timer has never started, the time returned will be zero.

   \return The number of seconds the stop watch has counted.
**/
unsigned32
StopWatch::seconds() const
{
  if(isRunning())
  {
    struct timeval timeNow;
    gettimeofday(&timeNow,NULL);
    return elapsedSeconds(&m_tvStart,&timeNow);
  }
  return elapsedSeconds(&m_tvStart,&m_tvStop);
}


/*
  Subtracts the first time from the second time to calculate the difference
  between the two.

  \param    pFirstTime is the first timing (ie: when you started the stopwatch)
  \param    pSecondTime is the second timing (ie: when you stopped the stopwatch, or the current time)

  \return   The elapsed time in milliseconds. 
*/
unsigned32 
StopWatch::elapsedMSecs(
  const struct timeval* pFirstTime,
  const struct timeval* pSecondTime) const
{
  unsigned32 secs,msecs;
  unsigned32 elapsedTime;

  secs = pSecondTime->tv_sec - pFirstTime->tv_sec;
  msecs = (pSecondTime->tv_usec - pFirstTime->tv_usec) / 1000;
  elapsedTime = secs*1000+msecs;

  return elapsedTime;
}

/*
  Subtracts the first time from the second time, and returns the difference
  between the two. Milliseconds are not included. 

  \param    pFirstTime is the first timing (ie: when you started the stopwatch)
  \param    pSecondTime is the second timing (ie: when you stopped the stopwatch, or the current time)

  \return   The elapsed time in seconds. 
*/
unsigned32 
StopWatch::elapsedSeconds(
  const struct timeval* pFirstTime,
  const struct timeval* pSecondTime) const
{
  return pSecondTime->tv_sec - pFirstTime->tv_sec;
}


/**
   Sets the stopwatch to its initial state of zero, and not running.
**/
void
StopWatch::reset()
{
  m_bRunning = false;
  m_tvStart.tv_sec = m_tvStart.tv_usec = 0;
  m_tvStop.tv_sec  = m_tvStop.tv_usec  = 0;
}
