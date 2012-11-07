/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  03/21/2000 9:00pm
        filename :  file.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  File handling class.
*********************************************************************/

#ifndef FILE_H
#define FILE_H

#include "ssobjects.h"
#include <stdio.h>

namespace ssobjects
{

class GeneralException;

#define throwFileException(m) (throw FileException(m,__FILE__,__LINE__))

class FileException : public GeneralException
{
public: // Constructor/destructor
    FileException(char* pchMessage,const char* pFname,const int iLine) 
            : GeneralException(pchMessage,pFname,iLine){};
};

typedef FILE* PFILE;
class File
{
    private:
        FILE*       m_fp;
        long        m_lFileSize;
        unsigned8*  m_pfileData;

    public:
        File();
        File(const char* pszFilename,const char* pszModes = "rb");
        ~File();

    public:
        int         open(const char* pszFilename,const char* pszModes);
        void        close();
        unsigned8*  load();
        void        unload();
        long        read(void* pDest,uint nMax=0);
        long        write(void* pSrc,uint nCount);
        long        write(CStr& string);
        long        getSize();
        unsigned8*  getDataPtr();
        char*       fgets(char* s,int size);            // reads a single line from file
        char*       fgetsTrimNL(char* s,int size);      // reads a single line from file and trims the NL off the end
        operator    PFILE() {return m_fp;}
        void        rewind();

    private:
        int         gotError(char* pszMsg,int iReturnCode);
        long        calculateFileSize();

    public:
        File(const File& file);
        File& operator=(const File& file);
};

};
#endif
