/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from http://www.codeproject.com/datetime/dateclass.asp
       Copyright (c), Richard Stringer

       filename : Parseit.h

       changes  : 24-AUG-2000 Lee Patterson (workerant@users.sourceforge.net)
                  - Removed MFC code so it will compile under other 
                    platforms.
                  - Made more functions use const.

       notes    : Most of the date alogrithms used in this class 
                  can be found at:
                  http://www.capecod.net/~pbaum/date/date0.htm

*********************************************************************/

#ifndef	__PARSEIT_H_
#define	__PARSEIT_H_

#ifdef WIN32
# define snprintf _snprintf		//make it linux like (NOTE: snprintf in Linux behaives a little different when you exceed the size
#else
 typedef const char* LPCSTR;
 typedef char* LPSTR;
#endif

#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

namespace ssobjects
{

///////////////////////////////////////////////////////////////////////////////////	
//	class definition for the ParseField class
///////////////////////////////////////////////////////////////////////////////////
//	this class is just a CString with some assignment and cast operators
//	it is used to store a parsed field value and allow the data to be accessed
//	thru casting the result to the wanted datatype
//	************************** ALLOWED OPERATIONS *********************************
//	CParseField p;
//	p="1234.12" 		// assignment to LPCSTR
//	CStrinf e=(LPCSTR)p;//	operator (LPCSTR)
//	double d=(double)p;	//	d=1234.12 	operator double 
//	int n=(int)p;		//	n=1234 	operator int
//////////////////////////////////////////////////////////////////////////////////
class CParseField
{
    public:
        CParseField() {};
        ~CParseField() {};

        operator int() 		{ return atoi(TheData);};
        operator long()		{ return atol(TheData);};
        operator double()	{ return atof(TheData);};
        operator LPCSTR()	{ return (LPCSTR) TheData;}; const
        CParseField& operator =(LPCSTR s) {strcpy(TheData,s);return *this;};
        CParseField&  operator =(CParseField& s) {strcpy(TheData,s.TheData);return *this;};
	
// Implementation
    protected:
        char TheData[80];
};
///////////////////////////////////////////////////////////////////////////////////
//	END OF CparseField DEFINITION AND CODE ( ALL FUNCTIONS HERE)
///////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////	
//	class definition for the Parsing class
///////////////////////////////////////////////////////////////////////////////////
class CParseIt
{
  private:    //unused overloads
    CParseIt(const CParseIt&);
    CParseIt& operator=(const CParseIt&);

  public:
    CParseIt(bool Strip=false);
    CParseIt(LPCSTR Data,LPCSTR Sep=",",bool Strip=false);
    ~CParseIt();

    //	use default constructoe then call this function to open and use a file contaimning
    //	the data    
#ifdef WIN32
    bool	ParseFile(LPCSTR lpzFileName,LPCSTR Sep=",");
#endif
// Attributes
  public:
// Operations             
  protected:           
    bool 		IsSeperator(char s);
#ifdef WIN32
    bool 		LoadFile(LPCSTR lpzFileName);
#endif
	
  public:         
    //	returns the one based number of fields if 0 there are no fields
    int			GetNumFields() {return NumFields;};
    //	returns the list of seperators
    LPCSTR		GetSeperator() {return Seperator;};
    //	sets the seperator list IE "|,*"
    void		SetSeperator(LPCSTR Sep) {strncpy(Seperator,Sep,9);};
    //	parse a object constructed bt the second constructor (data,sep);
    bool		Parse();
    //	parse a object created by the default constructor will call Parse();
    bool		Parse(LPCSTR Data,LPCSTR Sep=",",bool Strip=false);
    //	returns the 1 based field in a string
    bool	 	GetField(int nFNum,LPSTR Buff);
    //	will return the cparsefield object. This can be cast to the desired type
    //	by (int) (long) (double) (lpcstr)
    CParseField	GetField(int n);
	
// Implementation
  public:
    void ReSet();

  protected:
    bool		        StripQuotes;
    char		        Seperator[10];
    int			        NumFields;
    LinkedList<CParseField>	TheFields;	
    LPSTR		        TheData;
};

};

#endif
