/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from http://www.codeproject.com/datetime/dateclass.asp
       Copyright (c), Richard Stringer

       filename : SimpleDate.h

       changes  : 24-AUG-2000 Lee Patterson (workerant@users.sourceforge.net)
                  - Removed MFC code so it will compile under other 
                    platforms.
                  - Made more functions use const.

       notes    : Most of the date alogrithms used in this class 
                  can be found at:
                  http://www.capecod.net/~pbaum/date/date0.htm

*********************************************************************/

#ifndef SIMPLEDATE_H
#define SIMPLEDATE_H

namespace ssobjects
{

enum{
    MMDDYY,
    DDMMYY,
    YYMMDD,
    MMDDYYYY,
    DDMMYYYY,
    YYYYMMDD,
};

// NOTE:: If you choose any of the two digit year formats you had better be sure all
//        dates will be in the range of 1900-1999 else there will be unexpected results

#ifdef WIN32
# define snprintf _snprintf		//make it linux like (NOTE: snprintf in Linux behaives a little different when you exceed the size
#else
 typedef const char* LPCSTR;
 typedef char* LPSTR;
#endif

class CSimpleDate  
{
public:
    //  constructors and destructors
    CSimpleDate(int FormatType=MMDDYYYY);
    CSimpleDate(LPCSTR DateString,int FormatType=MMDDYYYY);
    CSimpleDate(long JD,int FormatType=MMDDYYYY);
    virtual ~CSimpleDate();

    //  Date math routines
    const   CSimpleDate& AddDays(int Days);
    const   CSimpleDate& AddYears(int Yrs);
    const   CSimpleDate& AddMonths(int Mon);
    const   CSimpleDate& SubtractYears(int Yrs);
    const   CSimpleDate& SubtractDays(int Days);
    const   CSimpleDate& SubtractMonths(int Mon);
    virtual int         YearsOld();

    // access routines;
    LPCSTR      GetFullDateString();
    LPCSTR      GetFullDateStringLong();
    virtual     int GetDayOfWeek();
    virtual     bool IsValid() const;
    long        GetJulianDate(); // easier to use (long)CSimpelDate but some prefer this

    virtual     int GetDay()
                    {if(!IsValid()) return 0;return m_Day;};
    virtual     int GetMonth()
                    {if(!IsValid()) return 0;return m_Month;};
    virtual     int GetYear()
                    {if(!IsValid()) return 0;return m_Year;};
    virtual     void    GetIntegerDate(int& m, int& d,int& y)
                    {if(!IsValid()) return;m=m_Month;y=m_Year;d=m_Day;};
    virtual     int     GetHour()
                    {if(!IsValid()) return 0;return m_Hour+m_bPM*12;};
    virtual     int     GetMin()
                    {if(!IsValid()) return 0;return m_Min;};
    virtual     int     GetSeconds()
                    {if(!IsValid()) return 0;return m_Second;};

    virtual     void    GetTimeString(LPSTR s,int nLen,bool AmPm=true);
    virtual     void    GetTimeStringShort(LPSTR s,int nLen,bool AmPm=true);

    //  operator assignments conversions equality etc...
    operator    LPCSTR();
    operator    long() const;
    const CSimpleDate& operator = (const CSimpleDate& Date);
    const CSimpleDate& operator = (LPCSTR Date);
    bool operator > (const CSimpleDate& Date) const;
    bool operator < (const CSimpleDate& Date) const;
    bool operator >= (const CSimpleDate& Date) const;
    bool operator <= (const CSimpleDate& Date) const;
    bool operator == (const CSimpleDate& Date) const;
    bool operator != (const CSimpleDate& Date) const;
    bool operator > (LPCSTR Date) const;
    bool operator < (LPCSTR Date) const;
    bool operator >= (LPCSTR Date) const;
    bool operator <= (LPCSTR Date) const;
    bool operator == (LPCSTR Date) const;
    bool operator != (LPCSTR Date) const;



protected:
    //  internal class stuff
    virtual bool    SetToday();
    virtual bool    ParseDateString(LPCSTR,int& m,int& d,int& y);
    virtual bool    ParseDateString(LPCSTR);
    virtual long    ConvertToJulian( int month,int day,int year);
    virtual long    ConvertToJulian();
    virtual void    ConvertFromJulian(int& Month,int& Day,int& Year);
    virtual void    ConvertFromJulian();
    virtual void    AdjustDays();
    virtual void SetTime();

    //  STATIC MEMBER FUNCTIONS
public:  
    
    // the static functions assume a date format of MMDDYY or MMDDYYYY
    //  They also do not call this IsValid() function
    static  bool    VerifyDateFormat(LPCSTR date);
    static  bool    FixDateFormat(LPSTR date);
    

    //  class data
protected:
    int         m_Year;
    int         m_Month;
    int         m_Day;
    long        m_JulianDate;
    int         m_Format;
    char        m_DateString[80];

    int         m_Hour;
    int         m_Min;
    int         m_Second;
    bool        m_bPM;

};

};

#endif  //SIMPLEDATE_H

