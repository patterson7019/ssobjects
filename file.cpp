/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  03/21/2000 9:00pm
        filename :  file.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  File handling class.
*********************************************************************/

#include "file.h"

using namespace ssobjects;

File::File() : m_fp(0),m_lFileSize(-1),m_pfileData(NULL)
{
}

File::File(const char* pszFilename,const char* pszModes)
        : m_fp(0),m_lFileSize(-1),m_pfileData(NULL)
{
    if((m_fp=fopen(pszFilename,pszModes))==NULL)
        throwFileException("error opening file");
    m_lFileSize = getSize();
}

File::File(const File& file)
        : m_fp(NULL),m_lFileSize(file.m_lFileSize),m_pfileData(NULL)
{
    //file should remain closed, but will make sure other information and data 
    //is copied.

    if(file.m_pfileData && file.m_lFileSize)
    {
        unsigned8* p = (unsigned8*)malloc(m_lFileSize);
        if(!p) throwFileException("error allocating memory");
        m_pfileData = p;
        memcpy(m_pfileData,file.m_pfileData,m_lFileSize);
    }
}

File& File::operator=(const File& file)
{
    //file should remain closed, but will make sure other information and data 
    //is copied.

    if(&file == this)   //object assigned to itself
        return *this;

    if(m_pfileData) free(m_pfileData);  //remove old data

    m_fp=NULL;
    m_lFileSize = file.m_lFileSize;
    if(file.m_pfileData && file.m_lFileSize)
    {
        unsigned8* p = (unsigned8*)malloc(m_lFileSize);
        if(!p) throwFileException("error allocating memory");
        m_pfileData = p;
        memcpy(m_pfileData,file.m_pfileData,m_lFileSize);
    }

    return *this;
}

File::~File()
{
    close();
    unload();
}

int File::open(const char* pszFilename,const char* pszModes)
{
    if((m_fp=fopen(pszFilename,pszModes))==NULL)
        throwFileException("couldn't open file");
    m_lFileSize = getSize();
    return 1;
}

void File::close()
{
    if(m_fp)
        fclose(m_fp);
    m_fp = NULL;
}

long File::read(void* pDest,uint count)
{
    long ret;
    ret = fread(pDest,count,1,m_fp);
    if(!ret)
    {
        if(feof(m_fp))
            return 0;
        else
            throwFileException("read error");
    }
    return ret;
}

//
//writes a CStr object. Does not include the NULL char.
//
long File::write(CStr& string)
{
    return write((char*)string,string.strlen());
}

long File::write(void* pSrc,uint count)
{
    int ret = fwrite(pSrc,count,1,m_fp);
    if(!ret)
    {
        if(feof(m_fp))
            return 0;
        else
            throwFileException("write error");
    }
    return ret;
}

//
// Loads the file, and returns a pointer to the memory allocated for the 
// file. 
//
unsigned8* File::load()
{
    unsigned8* p;
    int ret;

    if(!m_fp) throwFileException("no file is open");
    p = (unsigned8*)malloc(getSize());
    if(!p) throwFileException("error allocating memory");

    m_pfileData = p;
    ret = read(p,getSize());
    if(-1 == ret)
    {
        throwFileException("error reading");
        unload();
    }
    return p;
}

//
// frees the loaded data
//
void File::unload()
{
    if(m_pfileData)
    {
        free(m_pfileData);
        m_pfileData = NULL;
    }
}

//returns:
//
//  -1 on error or throw an exception
//  size of the file. If file is opened in ascii file, 
//      size may be not be correct.
//
long  File::getSize()
{
    if(-1 == m_lFileSize)
        m_lFileSize = calculateFileSize();
    return m_lFileSize;
}


long File::calculateFileSize()
{
    assert(m_fp);

    long filesize;

    if(fseek(m_fp,0,SEEK_END)!=0)
        throwFileException("error getting file size");
    else
    {
        if((filesize = ftell(m_fp)) == -1)
            throwFileException("error getting file size");
    }
    fseek(m_fp,0,SEEK_SET);
    return filesize;
}

unsigned8* File::getDataPtr()
{
    if(!m_fp)
        throwFileException("no file open");
    if(!m_pfileData)
        throwFileException("no data read");

    return m_pfileData;
}

//
// reads a single line from file. equivalent to the stdlib call.
//
char* File::fgets(char* s,int size)
{
    char* p=::fgets(s,size,m_fp);
    if(!p)
        throwFileException("EOF");
    return p;
}

//
// reads a single line from file and trims the NL off the end
// also checks for if there is a carrage return, and will remove
// that dos text file thingy.
//
char* File::fgetsTrimNL(char* s,int size)
{
    char* p=fgets(s,size);
    char* nl=&s[strlen(s)-1];
    *nl='\0';    //set new line to null
    nl--;
    if('\r'==*nl)  //check for a carrage return
        *nl='\0';
    return p;
}

void File::rewind()
{
    if(!m_fp)
        throwFileException("no file open");
    else
        fseek(m_fp,0,SEEK_SET);
}
