/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  September 4, 2001
        filename :  stopwatch.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)

*********************************************************************/

#ifndef STOPWATCH_H
#define STOPWATCH_H

#ifdef _WIN32
# include "gettimeofday.h"
#else
# include <sys/time.h>
#endif

#include "ssobjects.h"

namespace ssobjects
{

/** 
    \brief Timer operations.

    You can start and stop a stopwatch. 

    \note Plans are that this should work seemlessly with simple time or
    something so I can produce a report on how many days, minutes and seconds a
    timer has run.
**/
class StopWatch
{
  private:
    bool m_bRunning;
    struct timeval m_tvStart;
    struct timeval m_tvStop;

  public:
    StopWatch();                ///< Construct a stop watch.
    void start();               ///< Start the timer.
    void stop();                ///< Stop the timer.
    void reset();               ///< Reset all times to zero.

    bool isRunning() const;           ///< Find out if the stop watch is currently running.
    unsigned32 milliseconds() const;  ///< Get the current time in milli-seconds.
    unsigned32 seconds() const;       ///< Get the current time in seconds.

  private:
    unsigned32 elapsedMSecs(const struct timeval* pFrom,const struct timeval* pTo) const;
    unsigned32 elapsedSeconds(const struct timeval* pFrom,const struct timeval* pTo) const;
};

};

#endif //STOPWATCH_H
