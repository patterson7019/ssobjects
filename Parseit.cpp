/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       Original source from http://www.codeproject.com/datetime/dateclass.asp
       Copyright (c), Richard Stringer

       filename : Parseit.cpp

       changes  : 24-AUG-2000 Lee Patterson (workerant@users.sourceforge.net)
                  - Removed MFC code so it will compile under other 
                    platforms.
                  - Made more functions use const.

       notes    : Most of the date alogrithms used in this class 
                  can be found at:
                  http://www.capecod.net/~pbaum/date/date0.htm

*********************************************************************/

#ifdef WIN32
#include "stdafx.h"
#endif

#include <stdarg.h>
#include "Parseit.h"

using namespace ssobjects;

///////////////////////////////////////////////////////////////////////////
//	Desc:	default constructor just set some default values
//	params:
//	returns:
///////////////////////////////////////////////////////////////////////////
CParseIt::CParseIt(bool Strip)
        : StripQuotes(Strip),
          NumFields(0),
          TheFields(),
          TheData(NULL)
{
	strcpy(Seperator,",");
}

///////////////////////////////////////////////////////////////////////////
//	Desc:	construct object to parse data using sep for the list of seperators
//	params:
//	returns:
///////////////////////////////////////////////////////////////////////////
CParseIt::CParseIt(LPCSTR Data,LPCSTR Sep,bool Strip)
        : StripQuotes(Strip),
          NumFields(0),
          TheFields(),
          TheData(NULL)
{
	TheData=new char[strlen(Data)+1];
	strcpy(TheData,Data);
	strncpy(Seperator,Sep,9);
	Parse();
	
}
///////////////////////////////////////////////////////////////////////////
//	Desc:	destructor: free all allocated memory variables and free up the
//			data stored in the pointer array
//	params:
//	returns:
///////////////////////////////////////////////////////////////////////////
CParseIt::~CParseIt()
{   
    TheFields.purge();
    delete[] TheData;
}

void CParseIt::ReSet()
{
    delete[] TheData;
    TheData=NULL;
	TheFields.purge();

}

///////////////////////////////////////////////////////////////////////////
//	Desc:	parse the string data using sep as a seperator list
//	params: data=string to parse sep= list of seperators
//	returns:true if all is well else false if error occours
///////////////////////////////////////////////////////////////////////////
bool CParseIt::Parse(LPCSTR Data,LPCSTR Sep,bool Strip)
{
	if(TheData!=NULL)
		return false;
	StripQuotes=Strip;
	strncpy(Seperator,Sep,9);
	TheData=new char[strlen(Data)+1];
	strcpy(TheData,Data);
	return Parse();
}
///////////////////////////////////////////////////////////////////////////
//	Desc:	Called to do actual parsing of data
//	params:	none
//	returns:true if OK else false
///////////////////////////////////////////////////////////////////////////
bool CParseIt::Parse()
{
	if(TheData==NULL)
		return false;                      

	TheFields.purge();
		
	NumFields=0;
	char t[1000];
	int tpos=0;
	memset(t,0,sizeof(t));
	for(int y=0;y < (int)strlen(TheData);++y)
	{                       
		if( !IsSeperator(TheData[y]))
		{
			if(StripQuotes)
			{
				if(TheData[y] !='\"')
					t[tpos++]=TheData[y];
			}
			else
				t[tpos++]=TheData[y];
		}
		else
		{
			CParseField* pData=new CParseField;     
			*pData=t;
			TheFields.addTail(pData);
			memset(t,0,sizeof(t));
            tpos=0;
			++NumFields;
		}   
	}
	CParseField* pData=new CParseField;
	*pData=t;
	TheFields.addTail(pData);
	++NumFields;
	return true;		
}
///////////////////////////////////////////////////////////////////////////
//	Desc: 	determines if the given cahracter is in the list of seperators
//	params:	The character in question
//	returns:true if the cahr is a seperator else false
///////////////////////////////////////////////////////////////////////////
bool CParseIt::IsSeperator(char s)
{
	for(int x=0;x < (int) strlen(Seperator);++x)
	{
		if(s==Seperator[x])
			return true;
	}
	return false;
}					

///////////////////////////////////////////////////////////////////////////
//	Desc:	returns field n in the string buff (1 based not zero based
//	params: The field number (1..nun fields) and a pointer to a string
//	returns:true if OK else false (out of bounds index)
///////////////////////////////////////////////////////////////////////////
bool CParseIt::GetField(int N,LPSTR Buff)
{   
	if(N <= (int)TheFields.getNumEntries() && N > 0)
	{
        CParseField* f = TheFields.getAt(--N);
		strcpy(Buff,(LPCSTR)(*f));
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
//	Desc:	returns the CParseField object for the indicated field ( 1 based)
//	params:	the 1 based index of the field
//	returns:A CparseField object ( this can be cast to a string,long,double or int)
///////////////////////////////////////////////////////////////////////////
CParseField  CParseIt::GetField(int N)
{                                        
	CParseField p;
	if(N <= (int)TheFields.getNumEntries() && N > 0)
	{      
		--N;
        CParseField* ptr;
        ptr = TheFields.getAt(N);
        p = *ptr;
		return p;
	}
	return p;
}
