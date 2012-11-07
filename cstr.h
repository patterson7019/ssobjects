/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        Original source copyright was lost. Modifications by
        Lee Patterson.

        created  :  3/10/2000
        filename :  cstr.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  String class 
*********************************************************************/

#ifndef CSTR_H
#define CSTR_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

char* strtoupper(char* pszConvert);
char* strtolower(char* pszConvert);
char* ultoa(ulong nValue, char* pszBuffer, uint nRadix=10);
char* ltoa(long nValue, char* pszBuffer, uint nRadix=10);

namespace ssobjects
{

class CStr
{
  public:
    virtual uint addError();                    // Increase the error count and return the new error count.
    virtual ~CStr() { empty(); }

  public:
    //  Append new string segment to existing string. If nMaxChars is 0, entire string is appended, otherwise only
    //  the first nMaxChars of the string is added. To support client not having to check for NULL strings before calling this
    //  function, it can deal with a NULL value for pszString. If this is the case, the function is a NOOP, and the error count
    //  is NOT increased. This is not considered an error. If appending a string of a known length, a small performance boost may
    //  be seen by specifying the known string length in the parameter nKnownStringLength. This value does NOT include the NULL
    //  terminator. It is valid to call this function with pszString set to itself, or within the existing string.
    char* append(const char* pszString, uint nMaxChars = 0, uint nKnownStringLength = 0);
    char* append(char nChar); 
    char* appendCRLF();
    char* appendNumber(int nValue)      { return appendNumber((long)nValue); }
    char* appendNumber(uint nValue)     { return appendNumber((ulong)nValue); }
    char* appendNumber(long nValue)     { char sz[64]; return append(ltoa(nValue, sz, 10)); }
    char* appendNumber(ulong nValue)    { char sz[64]; return append(ltoa(nValue, sz, 10)); }

  public:
    CStr(const char* pszString, uint nMaxChars = 0);
    CStr(CStr const& str); 
    CStr();

    //referancing 
    operator char* () ;
    operator char* () const;
//        operator const char* () const;

    // Assign to string string
    char* format(const char* pszFormat,...);
    CStr& operator = (CStr const& s);
    CStr& operator = (const char* pszString)    { String(pszString); return *this; }
    CStr& operator = (int iValue)               { String(iValue); return *this; }
    CStr& operator = (uint nValue)              { String(nValue); return *this; }
    
    // Concatenation of string
    char* operator += (const char* pszString)   { return append(pszString); }
    char* operator << (const char* pszString)   { return append(pszString); }
    char* operator += (char nChar)              { return append(nChar); }
    char* operator << (char nChar)              { return append(nChar); }
    char* operator += (int nValue)              { return appendNumber(nValue); }
    char* operator << (int nValue)              { return appendNumber(nValue); }
    char* operator += (uint nValue)             { return appendNumber(nValue); }
    char* operator << (uint nValue)             { return appendNumber(nValue); }

    // Comparison of string contents
    bool operator == (int nValue) const;
    bool operator == (uint nValue) const;
    bool operator == (const char* pszString) const;
    bool operator != (const char* pszString) const;
    

    // find out information about this object
    bool isEmpty() const;
    bool isNotEmpty() const;
    uint getCharCount(char nChar) const;        // Returns the number of times a particular character is found in the string.


    //return a referance or set a charactoer at specified index into string 
    char& charAt(uint nIndex) const;
    void charAt(uint nIndex, char nChar);


    //Compairs. Returns zero if the strings are identical, -1 if this CStr object is less
    //than pszString, or 1 if this CStr object is greater than pszString. 
    //If either string is NULL or empty, 2 will be returned.*/
    int compare(const char* pszString) const;
    int compareNoCase(const char* pszString) const;


    // Find first occurence of character or substring in string. 
    //Returns index into string if found, -1 otherwise.
    int find(const char* pszSubString) const;
    int find(char nChar) const;


    /* Substring extraction, returns a new CStr object with the first nCount of this strings characters in it. If this string
       is less that nCount in length, the entire string will be returned.*/
    CStr left(uint nCount) const;
    CStr mid(uint nFirst, uint nCount = 0) const;
    CStr right(uint nCount) const;

    // Remote all spaces from left, right or both sides of a string.
    char* trimRight();
    char* trimLeft();
    char* trim();

    // Change case of entire string
    char* makeLower();
    char* makeUpper();


    /* Get the length of the string, or if bBuffer is true, the amount of allocated buffer memory. This function could
       return 0 when the internal string actually has length. See the String(no parameters) function for more information
       about this feature used to avoid unneeded memory allocations.*/
    uint getLength(bool bBuffer = false) const;
    uint strlen() {return getLength();}

    // Append a character to the end of the string if the string does not already
    // end with that character. 
    // If the string already ends with the character specified by nChar, this
    // function will be a NOOP. This function will only succeed on a string that 
    // already has length. It will not set the first character of an otherwise empty 
    // string to nChar.*/
    void endWith(char nChar);


    // Free the internal buffer and reset internal variables. If bFreeBuffer is true, the internal buffer will be free'd,
    // otherwise this function will only reset internal members so the previously allocated memory can be re-used. If the internal
    // buffer is kept for future use, this class guards against calls that make use of it. In other words, even if the internal
    // pointer m_pszString is not NULL, calls to getLength() or the comparison functions for instance will behave like it is NULL.
    char* empty(bool bFreeBuffer = true, bool bResetErrorCount = true);


    /* Remove a section from the center of this string. nFirst is the index into this string of the first character to be
       removed. nRemoveCount is the number of characters to remove. For example, if this class holds the string "Testing" and
       you pass 1 as the first character to remove and 5 as the remove count, the resulting string would be "tg". Starting
       with the "e" in "Testing" which is at index 1 in the string, 5 characters, including the "e" were removed. Returns
       internal string buffer on success, NULL on failure. On failure, the internal string is not modified. If nRemoveCount
       would extend beyond the length of the string, all characters after nFirst (inclusive) will be removed. If nFirst is
       0 and nRemoveCount is the string length (or greater), the string will be emptied and NULL returned. This function
       never results in a memory allocation, since it shortens the string.*/
    char* removeSection(uint nFirst, uint nRemoveCount = 1);

    /* Set a pointer to an externally allocated C style string (allocated with malloc()) into object. This can be done
       to replace a string pointer with a previously "stolen" pointer from the StealBuffer() function. This class will
       then assume ownership of the string, deleting it as needed. The length of the allocated memory MUST be given to this
       class for the function to succeed. The C string essentially becomes a VString object, so the allocated memory size
       must be given, and it must be at least 1 byte longer than the string to be valid.*/
    char* replaceBuffer(char* pszString, uint nAllocatedMemorySize, uint nKnownStringLength = 0);
    char* replaceBuffer(CStr& strFrom);
    char* replaceCharacters(char nFind, char nReplacement);

    /* Replace the first occurance of nChar with nReplacement in the string. The default for nReplacement terminates
       the string. The return value is the point at which the string was terminated on success, NULL on failure.*/
    char* replaceFirstChar(char nChar, char nReplacement = '\0') { return replaceFirstOrLastChar(nChar, nReplacement, true); }

    /* Replace the last occurance of nChar with nReplacement in the string. The default for nReplacement terminates
       the string. The return value is the point at which the string was terminated on success, NULL on failure.*/
    char* replaceLastChar(char nChar, char nReplacement = '\0') { return replaceFirstOrLastChar(nChar, nReplacement, false); }


    // Return the internal error count. This value is incremented anytime a memory allocation fails, or externally
    // by calls to AddError().
    uint getErrorCount() const { return m_nErrorCount; }

    /* Return a reference to the granularity size. This is how much memory, during appends, is allocated each time an
       append or allocation operation would require more memory than what is currently allocated. Any value, other than 0 is valid
       as the granularity setting. The larger the value, the more memory will be allocated each time a new block is required,
       but fewer allocations are needed. A smaller number decreases total memory usage, at the expense of more frequent
       reallocations. The default value is 64 bytes, and this will be used if set to 0.*/
    uint& getGranularity() const { return (uint&)m_nGranularity; }




    /* Steal the C-Style string buffer. Calling code takes ownership of string pointer and must free it when done using the CRT
       function free(). On exit, this class is a newly initialized state.*/
    char* stealBuffer();

    /* Get the internal string buffer. It is important to note that a valid buffer pointer might be returned from this
       function, even though the CStr class knows the specified length is 0. The reason is that internal buffers can be
       reused to avoid unneeded memory allocations. For instance, if you call the Empty() method and tell it not to release
       the internal buffer, the internal m_nLength member is set to 0 but the internal string pointer m_pszString is not
       touched. Therefore, calling this function would return the internal pointer. This could also be the case where memory
       for a string is preallocated with another String() function but not yet populated, or populated but before the
       UpdateLength() function is called to set the known string length.*/
    char* String() const { return m_pszString; }

    /* Save pszString in class and returns pointer to buffer if a string is held. If nExtraBytes is set, that much
       more memory will be allocated in addition to the length of pszString. pszString can be NULL and still have memory
       allocated if nExtraBytes is not 0. Using the function this way is a good way to dynamically allocate heap memory
       and still have this class free the memory when no longer needed. The allocated memory will be 0 filled. If
       extra bytes are allocated, they will be filled with 0's. If this function is used to allocate memory only (pszString
       set to NULL and nExtraBytes set to non-0), UpdateLength() should be called as soon as the class contains a valid
       string so that future processing using this class is not harmed.
       It should be noted that for string that are not likely to change size, this function differs from Append() by not
       allocating more memory than is required. This does not affect future appends which can still be done, it merely
       means that for strings not likely to change, it uses a more efficient storage scheme. If pszString is not NULL and
       nMaxChars is given, only the number of character specified will be copied to the string, although the extra bytes will
       still be allocated if given. A very slight performance boost can be realized if the length of pszString is known when
       this function is called, and can be specified as the nKnownStringLength parameter. If this function fails to allocate
       memory, it will return NULL, however the previous string held by this class will be left intact and safe.*/
    char* String(const char* pszString, uint nExtraBytes = 0, uint nMaxChars = 0, uint nKnownStringLength = 0);

    // Set a value into string
    char* String(int nValue) { return String((long)nValue); }
    char* String(uint nValue) { return String((ulong)nValue); }
    char* String(long nValue);
    char* String(ulong nValue);
    
    /* This function should be called anytime the length of the string is altered outside of the class. If the exact length
       of the string is known (or you want to lie about it), pass a non-0 value for nLength. Passing 0 causes this function to
       determine the length of the string. nLength cannot be larger than the real string length, although it can be smaller. Since
       this class can be used to store non-string data as a string, which could contain embedded 0 terminators, this class cannot
       check the validity of nLength when non-0. Care must be taken here to set the length to the exact length of the data.
       This class will verify that nLength is not larger than the internally allocated length.*/
    void updateLength(uint nLength = 0);

  protected:
    // Copy internal string if pszString is within us.
    CStr* duplicateOverlap(const char* pszString, bool& bDuplicateRequired);

    // Initialize member variables to default values, or copy from another object.
    void init(CStr const* pExisting = NULL);
        
    // Replace the first or last occurance of nChar with nReplacement in the string.
        
    char* replaceFirstOrLastChar(char nChar, char nReplacement, bool bFirst);

    // Embedded Member(s).
    char*           m_pszString;
    uint            m_nLength;
    uint            m_nAllocLength;
    uint            m_nGranularity;
    uint            m_nErrorCount;
};

/**
   \brief Formated String
 **/
class FmtString : public CStr
{
  public:
    //construct with a printf style format
    FmtString(const char* fmt,...);
};

};


#endif


