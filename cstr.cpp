/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        Original source copyright was lost. Modifications by
        Lee Patterson.

        created  :  3/10/2000
        filename :  cstr.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  String class 
*********************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "cstr.h"

char* ConvertNumbertoASCII(uint nValue, char* pszBuffer, uint nRadix, bool bNegative)
{
  char* pszOffset =       pszBuffer;
  char* pszFirstDigit =   pszOffset;
    
  // This must be known!
  assert(pszBuffer);

  // Radix (base) must be between 2 and 36!
  assert(nRadix >= 2 && nRadix <= 36);

  if ( !pszBuffer )
    return NULL;
    
  // Are we really dealing with a negative value? Handle this case.
  if ( bNegative )
  {
    *pszOffset = '-';
    pszOffset++;
    pszFirstDigit++;
    nValue = (ulong)(-(long)nValue);
  }
    
  do
  {
    uint nDigit = (uint)(nValue % nRadix);
    nValue /= nRadix;
        
    *pszOffset = (char)((nDigit > 9) ? (nDigit - 10 + 'a') : (nDigit + '0'));
    pszOffset++;
  }
  while (nValue > 0);
    
  // We now have the digit of the number in the buffer, but in reverse order. Reverse them.
  *pszOffset = '\0';
  pszOffset--;
    
  do
  {
    char chTemp =       *pszOffset;
    *pszOffset =        *pszFirstDigit;
    *pszFirstDigit =    chTemp;
    pszOffset--;
    pszFirstDigit++;
  }
  while (pszFirstDigit < pszOffset);

  return pszBuffer;
}

// Convert long to ASCII.
char* ltoa(long nValue, char* pszBuffer, uint nRadix)
{ 
  return ConvertNumbertoASCII((ulong)nValue, pszBuffer, nRadix,
                              (bool)((nRadix == 10 && nValue < 0) ? true : false)); 
}

// Convert unsigned long to ASCII.
char* ultoa(ulong nValue, char* pszBuffer, uint nRadix)
{ 
  return ConvertNumbertoASCII(nValue, pszBuffer, nRadix, false); 
}

char* strtolower(char* pszConvert)
{
  if ( pszConvert )
  {
    char* pszOffset = pszConvert;
    
    while ( *pszOffset != '\0' )
    {
      if ( isupper(*pszOffset) )
        *pszOffset = tolower(*pszOffset);
    
      pszOffset++;
    }
  }

  return pszConvert;
}

// Convert pszConvert to uppercase, returning a pointer to it.
char* strtoupper(char* pszConvert)
{
  if ( pszConvert )
  {
    char* pszOffset = pszConvert;
    
    while ( *pszOffset != '\0' )
    {
      if ( islower(*pszOffset) )
        *pszOffset = toupper(*pszOffset);
    
      pszOffset++;
    }
  }

  return pszConvert;
}

using namespace ssobjects;

FmtString::FmtString(const char* fmt,...) : CStr()
{
  char buffer[1024];
  va_list marker;

  va_start(marker,fmt);
  vsnprintf(buffer,sizeof buffer,fmt,marker);
  NULL_TERMINATE(buffer,sizeof buffer);
  format("%s",buffer);
}


/* Construct with existing C style string, or nothing. nMaxChars specifies the maximum number of characters in the
   string to copy from pszString. If 0 is used, the entire string is copied.*/
CStr::CStr(const char* pszString, uint nMaxChars)
  : m_pszString(NULL),
    m_nLength(0),
    m_nAllocLength(0),
    m_nGranularity(0),
    m_nErrorCount(0)
{ 
  init(); 
  String(pszString, 0, nMaxChars); 
}

// Construct object with existing CStr object (copy constructor).
CStr::CStr(CStr const& str) 
  : m_pszString(NULL),
    m_nLength(0),
    m_nAllocLength(0),
    m_nGranularity(0),
    m_nErrorCount(0)
{ 
  init(&str); 
}

CStr::CStr()
  : m_pszString(NULL),
    m_nLength(0),
    m_nAllocLength(0),
    m_nGranularity(0),
    m_nErrorCount(0)
{
  init();
  String(NULL,0,0); 
}

//
//  Since we don't know how big the resulting output buffer will be we will guess
//  at the size, and if it isn't big enough, we'll double the size, and try again
//  untill we get the right size. The assign it to this string object were the size
//  is adjusted correctly so no waisted space.
//
//NOTE : GNU vsnprintf seems to be implimented as not returning -1 
//NOTE   when there are too many characters for the buffer. 
//NOTE   Win32 however does return -1.
//
char* CStr::format(const char* fmt,...)
{
  char* newbuffer=NULL;
  va_list marker;
  int length,ret=0;

  empty();
  length = ::strlen(fmt);
  do
  {
    length *= 2;
#ifndef WIN32
    if(ret>0)
      length = ret+1;
#endif    
    if(newbuffer)
      delete [] newbuffer;

    newbuffer = new char[length];
    va_start(marker,fmt);
    memset(newbuffer,0,length);
    ret = vsnprintf(newbuffer,length,fmt,marker);
  } while(-1 == ret || ret >= length);

  append(newbuffer);
  delete [] newbuffer;
  return (char*)m_pszString;
}

/* Returns non-const C-Style string pointer. See the String(no parameters) function for very important information
   about the validity of the returned string contents.*/
CStr::operator char* () const 
{ 
  return (char*)m_pszString; 
}

CStr::operator char* () 
{
  return (char*)m_pszString;
}

// Assign an existing CStr object (copy the string) to this object.
CStr& CStr::operator = (CStr const& s)
{ 
  String(s.String()); 
  return *this;
}

// Comparison of string contents with an integer.
bool CStr::operator == (int nValue) const
{ 
  char sz[64]; 
  return (compareNoCase(ltoa(nValue, sz, 10)) == 0) ? true : false; 
}

// Comparison of string contents with an unsigned integer.
bool CStr::operator == (uint nValue) const
{ 
  char sz[64]; 
  return (compareNoCase(ultoa((ulong)nValue, sz, 10)) == 0) ? true : false; 
}

// Comparison (equality) without case sensitivity.
bool CStr::operator == (const char* pszString) const
{ 
  return (compareNoCase(pszString) == 0) ? true : false; 
}

// Comparison (not equal) without case sensitivity.
bool CStr::operator != (const char* pszString) const
{ 
  return (compareNoCase(pszString) != 0) ? true : false; 
}
    
// Increase the error count and return the new error count.
uint CStr::addError()
{
  return ++m_nErrorCount; 
}

bool CStr::isEmpty() const
{ 
  return (getLength()) ? false : true; 
}

bool CStr::isNotEmpty() const
{ 
  return (getLength()) ? true : false; 
}

//  Append new string segment to existing string. If nMaxChars is 0, entire string is appended, otherwise only
//  the first nMaxChars of the string is added. To support client not having to check for NULL strings before calling this
//  function, it can deal with a NULL value for pszString. If this is the case, the function is a NOOP, and the error count
//  is NOT increased. This is not considered an error. If appending a string of a known length, a small performance boost may
//  be seen by specifying the known string length in the parameter nKnownStringLength. This value does NOT include the NULL
//  terminator. It is valid to call this function with pszString set to itself, or within the existing string.
char* CStr::append(const char* pszString, uint nMaxChars, uint nKnownStringLength)
{
    bool bSuccess = true;
        
    // Anything to do?
    if ( pszString )
    {
        // Check for required duplication.
        bool bDuplicateRequired;
        CStr* pOverlap = duplicateOverlap(pszString, bDuplicateRequired);

        // Return error if we needed a overlap duplication made and couldn't.
        if ( bDuplicateRequired && !pOverlap )
            bSuccess = false;

        // Still OK?
        if ( bSuccess )
        {
            // Assign overlap to string?
            if ( pOverlap )
                pszString = pOverlap->String();

            // Granularity cannot be 0! Set to default if it is.
            if ( !m_nGranularity )
                m_nGranularity = 64;

            // Determine memory allocation length. pszString cannot be NULL.
            if ( !nKnownStringLength )
                nKnownStringLength = (pOverlap) ? pOverlap->getLength() : ::strlen(pszString);

            // Are we only interested in a certain number of characters? Save that memory.
            if ( nMaxChars && nMaxChars < nKnownStringLength )
                nKnownStringLength = nMaxChars;
                
            // Do we have something to do?
            if ( nKnownStringLength )
            {
                char* pszBuffer = m_pszString;

                // Do we need to allocate more memory blocks?
                if ( !pszBuffer || nKnownStringLength + 1 > m_nAllocLength - m_nLength )
                {
                    // Determine number of blocks and total block size to allocate.
                    uint nBlocks =      ((nKnownStringLength + 1) / m_nGranularity) + 1;
                    uint nNewAlloc =    (nBlocks * m_nGranularity);

                    // (Re)Alloc memory now?
                    pszBuffer = (char*)realloc(m_pszString, m_nAllocLength + nNewAlloc);
                        
                    // Did the re-allocation succeed? If not, m_pszString is still valid.
                    if ( pszBuffer )
                    {
                        // Zero the memory we just allocated, but don't overwrite previous!
                        ZEROMEMORY((pszBuffer + m_nAllocLength), nNewAlloc);

                        // Make assignments and calculations.
                        m_pszString =       pszBuffer;
                        m_nAllocLength +=   nNewAlloc;
                    }
                }

                // Do we (still) have a buffer to write to?
                if ( pszBuffer )
                {
                    // Copy string now.
                    memcpy(m_pszString + m_nLength, pszString, nKnownStringLength);

                    // Increment known length.
                    m_nLength += nKnownStringLength;

                    // Zero terminate it.
                    *(m_pszString + m_nLength) = '\0';
                }
                else
                {
                    addError();
                    bSuccess = false;
                }
            }
        }

        DELETE_NULL(pOverlap)
    }

    return (bSuccess) ? m_pszString : NULL;
}

char* CStr::append(char nChar)
{
  char sz[2] = {nChar, '\0'}; 
  return append(sz);
}

char* CStr::appendCRLF() 
{ 
  return append("\r\n"); 
}

// Returns a character reference at specified index into the string.
char& CStr::charAt(uint nIndex) const
{ 
  assert(nIndex < getLength()); 
  return m_pszString[nIndex]; 
}

// Set a character at specified index into the string.
void CStr::charAt(uint nIndex, char nChar)
{ 
  charAt(nIndex) = nChar; 
}

int CStr::compare(const char* pszString) const
{ 
  return (getLength() && pszString) ? strcmp(m_pszString, pszString) : 2; 
}

int CStr::compareNoCase(const char* pszString) const
{
  return (getLength() && pszString) ? strcasecmp(m_pszString, pszString) : 2; 
}

// Free the internal buffer and reset internal variables. If bFreeBuffer is true, the internal buffer will be free'd,
// otherwise this function will only reset internal members so the previously allocated memory can be re-used. If the internal
// buffer is kept for future use, this class guards against calls that make use of it. In other words, even if the internal
// pointer m_pszString is not NULL, calls to getLength() or the comparison functions for instance will behave like it is NULL.
char* CStr::empty(bool bFreeBuffer, bool bResetErrorCount)
{
  // Free string if allocated and told to free the buffer.
  if ( m_pszString && bFreeBuffer )
  {
    free(m_pszString);
    m_pszString = NULL;
  }
            
  // Reset internal variables.
  m_nLength = 0;
        
  if ( bFreeBuffer )
    m_nAllocLength = 0;

  if ( bResetErrorCount )
    m_nErrorCount = 0;

  return m_pszString;
}

// Append a character to the end of the string if the string does not already
// end with that character. 
// If the string already ends with the character specified by nChar, this
// function will be a NOOP. This function will only succeed on a string that 
// already has length. It will not set the first character of an otherwise empty 
// string to nChar.*/
void CStr::endWith(char nChar)
{
  uint nLength = getLength();

  if ( nLength )
  {
    if ( *(m_pszString + (nLength - 1)) != nChar )
      append(nChar);
  }
}

// Find first occurence of character or substring in string. 
//Returns index into string if found, -1 otherwise.
int CStr::find(const char* pszSubString) const
{
  assert(pszSubString);

  if ( getLength() && pszSubString )
  {
    char* pszFound = strstr(m_pszString, pszSubString);
            
    if ( pszFound )
      return pszFound - m_pszString;
  }

  return -1;
}

// Find first occurence of a character in string. Returns index into string if found, -1 otherwise.
int CStr::find(char nChar) const
{ 
  char sz[2] = {nChar, '\0'}; 
  return find(sz); 
}

    // Returns the number of times a particular character is found in the string.
uint CStr::getCharCount(char nChar) const
{
  uint nCount =   0;
  uint nLength =  getLength();

  for ( uint i = 0; i < nLength; i++ )
  {
    assert(m_pszString);

    if ( *(m_pszString + i) == nChar )
      nCount++;
  }

  return nCount;
}

/* Get the length of the string, or if bBuffer is true, the amount of allocated buffer memory. This function could
   return 0 when the internal string actually has length. See the String(no parameters) function for more information
   about this feature used to avoid unneeded memory allocations.*/
uint CStr::getLength(bool bBuffer) const
{
  // Get buffer length?
  if ( bBuffer )
    return m_nAllocLength;
            
  if ( m_nLength )
  {   
    // If m_nLength is set, this better be valid!
    assert(m_pszString);
    return m_nLength;
  }

  return 0;
}

/* Substring extraction, returns a new CStr object with the first nCount of this strings characters in it. If this string
   is less that nCount in length, the entire string will be returned.*/
CStr CStr::left(uint nCount) const
{
  assert(nCount);

  CStr str;
        
  if ( nCount && getLength() )
    str.String(m_pszString, 0, nCount);

  return str;
}

CStr CStr::mid(uint nFirst, uint nCount) const
{
  CStr str;

  if ( nFirst < getLength() )
    str.String(m_pszString + nFirst, 0, nCount);

  return str;
}

CStr CStr::right(uint nCount) const
{
  assert(nCount);

  CStr str;
        
  if ( getLength() && nCount )
  {
    if ( nCount > m_nLength )
      nCount = m_nLength;

    str.String(m_pszString + (m_nLength - nCount), 0, nCount);
  }

  return str;
}


// Make all characters lower case.
char* CStr::makeLower()
{
  if ( getLength() )
  {
    assert(m_pszString);
    return strtolower(m_pszString);
  }

  return NULL;
}

char* CStr::makeUpper()
{
  if ( getLength() )
  {
    assert(m_pszString);
    return strtoupper(m_pszString);
  }

  return NULL;
}


/* Remove a section from the center of this string. nFirst is the index into this string of the first character to be
   removed. nRemoveCount is the number of characters to remove. For example, if this class holds the string "Testing" and
   you pass 1 as the first character to remove and 5 as the remove count, the resulting string would be "tg". Starting
   with the "e" in "Testing" which is at index 1 in the string, 5 characters, including the "e" were removed. Returns
   internal string buffer on success, NULL on failure. On failure, the internal string is not modified. If nRemoveCount
   would extend beyond the length of the string, all characters after nFirst (inclusive) will be removed. If nFirst is
   0 and nRemoveCount is the string length (or greater), the string will be emptied and NULL returned. This function
   never results in a memory allocation, since it shortens the string.*/
char* CStr::removeSection(uint nFirst, uint nRemoveCount)
{
  uint nLength = getLength();

  if ( nLength > nFirst )
  {
    // Anything to do?
    if ( nRemoveCount )
    {
      // Validate remove count.
      if ( nRemoveCount > nLength - nFirst )
        nRemoveCount = nLength - nFirst;

      // Going to empty string?
      if ( nRemoveCount == nLength )
        empty();
      else
      {
        // Copy remainder of string to first offset
        memcpy(m_pszString + nFirst, m_pszString + nFirst + nRemoveCount, nLength - (nFirst + nRemoveCount));

        // Zero-fill remainder.
        ZEROMEMORY((m_pszString + (nLength - nRemoveCount)), nRemoveCount);

        // Update length.
        m_nLength -= nRemoveCount;
      }
    }

    return m_pszString;
  }
        
  return NULL;    
}

/* Set a pointer to an externally allocated C style string (allocated with malloc()) into object. This can be done
   to replace a string pointer with a previously "stolen" pointer from the StealBuffer() function. This class will
   then assume ownership of the string, deleting it as needed. The length of the allocated memory MUST be given to this
   class for the function to succeed. The C string essentially becomes a VString object, so the allocated memory size
   must be given, and it must be at least 1 byte longer than the string to be valid.*/
char* CStr::replaceBuffer(char* pszString, uint nAllocatedMemorySize, uint nKnownStringLength)
{
  // Free current string.
  empty();
        
  // Save pointer?
  if ( pszString )
  {
    assert(nAllocatedMemorySize > (uint)::strlen(pszString));
                
    // Set member variables.
    m_pszString =       pszString;
    m_nAllocLength =    nAllocatedMemorySize;
    m_nLength =         (nKnownStringLength) ? nKnownStringLength : ::strlen(pszString);
  }

  return m_pszString;
}

// Steal the buffer from an existing VString object and place into this object. If strFrom is empty, this string will also be emptied.
char* CStr::replaceBuffer(CStr& strFrom)
{
  if ( strFrom.isEmpty() )
    empty();
  else
  {
    uint nAllocatedMemorySize = strFrom.getLength(true);
    uint nKnownStringLength   = strFrom.getLength(false);
    replaceBuffer(strFrom.stealBuffer(), nAllocatedMemorySize, nKnownStringLength);
  }

  return m_pszString;
}

// Replace all occurances of nFind in the string with nReplacement.
char* CStr::replaceCharacters(char nFind, char nReplacement)
{
  if ( getLength() )
  {
    char* pszOffSet = m_pszString;

    while ( *pszOffSet != '\0' )
    {
      if ( *pszOffSet == nFind )
        *pszOffSet = nReplacement;

      pszOffSet++;
    }

    return m_pszString;
  }

  return NULL;
}

void CStr::updateLength(uint nLength)
{
  // nLength cannot be more than what is already allocated, at a max.
  if ( nLength > m_nAllocLength )
    nLength = m_nAllocLength;

  m_nLength = (nLength) ? nLength : ((m_pszString) ? ::strlen(m_pszString) : 0);
}

/* Steal the C-Style string buffer. Calling code takes ownership of string pointer and must free it when done using the CRT
   function free(). On exit, this class is a newly initialized state.*/
char* CStr::stealBuffer()
{
  // Save return buffer after emptying the current object.
  char* pszString = empty(false);

  // Reset internal members.
  init(this);

  return pszString;
}

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
char* CStr::String(const char* pszString, uint nExtraBytes, uint nMaxChars, uint nKnownStringLength)
{
    bool bSuccess = true;

    // Same as empty?
    if ( !pszString && !nExtraBytes )
        return empty();

    // Anything to do?
    if ( pszString || nExtraBytes )
    {
        // Check for required duplication.
        bool bDuplicateRequired;
        CStr* pOverlap = duplicateOverlap(pszString, bDuplicateRequired);

        // Return error if we needed a overlap duplication made and couldn't.
        if ( bDuplicateRequired && !pOverlap )
            bSuccess = false;

        // Still OK?
        if ( bSuccess )
        {
            // Assign overlap to string?
            if ( pOverlap )
                pszString = pOverlap->String();

            // Determine memory allocation length. pszString can be NULL.
            if ( !nKnownStringLength )
                nKnownStringLength = (pOverlap) ? pOverlap->getLength() : ((pszString) ? ::strlen(pszString) : 0);

            // Are we only interested in a certain number of characters? Save that memory.
            if ( nMaxChars && nMaxChars < nKnownStringLength )
                nKnownStringLength = nMaxChars;
                
            // Determine allocation size, including requested extra bytes.
            uint nAllocLength = nKnownStringLength + nExtraBytes;

            // Anything to do?
            if ( nAllocLength )
            {
                // Add one for trailing 0 on string?
                if ( pszString )
                    nAllocLength++;

                // (Re)Allocate memory for string.
                char* pszBuffer = (char*)realloc(m_pszString, nAllocLength);

                if ( pszBuffer )
                {
                    // Initialize member variables.
                    m_pszString =       pszBuffer;
                    m_nLength =         nKnownStringLength;
                    m_nAllocLength =    nAllocLength;

                    // Zero fill unused memory.
                    if ( nAllocLength > nKnownStringLength )
                        ZEROMEMORY((m_pszString + nKnownStringLength), nAllocLength - nKnownStringLength);

                    // Copy string as needed.
                    if ( pszString )
                        memcpy(m_pszString, pszString, nKnownStringLength);
                }
                else
                {
                    addError();
                    bSuccess = false;
                }
            }
            else
                empty();
        }

        DELETE_NULL(pOverlap)
            }

    return (bSuccess ) ? m_pszString : NULL;
}

char* CStr::String(long nValue)
{
  char sz[64];
  return String(ltoa(nValue, sz, 10)); 
}

char* CStr::String(ulong nValue)
{
  char sz[64]; 
  return String(ultoa(nValue, sz, 10)); 
}

// Remote all spaces from left, right or both sides of a string.
char* CStr::trim()
{ 
  return (trimLeft() && trimRight()) ? m_pszString : NULL; 
}

// Remove all space characters from left side of string.
char* CStr::trimLeft()
{
  if ( getLength() )
  {
    char* pszOffSet = m_pszString;

    while ( *pszOffSet == ' ' )
      pszOffSet++;
            
    // Anything to do?
    if ( pszOffSet != m_pszString )
    {
      // Copy to temp first.
      CStr sTemp(pszOffSet);
                
      // Steal buffer.
      replaceBuffer(sTemp);
    }

    return m_pszString;
  }

  return NULL;
}

// Remove all space characters from right side of string.
char* CStr::trimRight()
{
  if ( getLength() )
  {
    char* pszOffSet = m_pszString + (m_nLength - 1);

    while ( pszOffSet != m_pszString && *pszOffSet == ' ' )
      pszOffSet--;
            
    // Anything to do?
    if ( pszOffSet != m_pszString + (m_nLength - 1) )
    {
      // All spaces?
      if ( pszOffSet == m_pszString && *pszOffSet == ' ' )
        empty();
      else
      {
        // Terminate string.
        *(pszOffSet + 1) = '\0';

        // Recalc new length.
        m_nLength = (pszOffSet - m_pszString) + 1;
      }
    }
  }

  return (m_nLength) ? m_pszString : NULL;
}

// Copy internal string if pszString is within us.
CStr* CStr::duplicateOverlap(const char* pszString, bool& bDuplicateRequired)
{
  bDuplicateRequired = false;

  // Deal with appending from an overlapping string. That is, a string that is part of us already.
  if ( pszString && m_pszString && pszString >= m_pszString && pszString <= m_pszString + m_nLength )
  {
    bDuplicateRequired = true;
            
    CStr* pDuplicate = new CStr(pszString);

    if ( pDuplicate && pDuplicate->isNotEmpty() )
      return pDuplicate;

    DELETE_NULL(pDuplicate)

      // Incremement error count.
      addError();
  }

  return NULL;
}

// Initialize member variables to default values, or copy from another object.
void CStr::init(CStr const* pExisting)
{
  // Initiailize members.
  m_pszString =                                   NULL;
  m_nAllocLength = m_nLength = m_nErrorCount =    0;

  // Copy the granularity as needed.
  m_nGranularity = (pExisting) ? pExisting->m_nGranularity : 64;

  // Copy string too?
  if ( pExisting && pExisting->isNotEmpty() )
    String(pExisting->String());
}

// Replace the first or last occurance of nChar with nReplacement in the string.
char* CStr::replaceFirstOrLastChar(char nChar, char nReplacement, bool bFirst)
{
  if ( getLength() )
  {
    char* psz = (bFirst) ? strchr(m_pszString, nChar) : strrchr(m_pszString, nChar);
            
    if ( psz )
    {
      *psz = nReplacement;
      return psz;
    }
  }

  return NULL;
}
