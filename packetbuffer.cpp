/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  5/24/1999
        filename :  packetbuffer.cpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  see header file for details
*********************************************************************/

#ifdef WIN32
#include "stdafx.h"
#endif

#include "generalexception.h"
#include "packetbuffer.h"
#include "bufferedsocket.h"
#include "logs.h"
#include <string.h>
#include "cstr.h"

#ifdef DEBUG
#include <stdio.h>
#endif
#ifndef WIN32
#include <netinet/in.h>
#endif

using namespace ssobjects;

//#ifdef _WINDOWS
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
//#endif

//The maximum allowed packet size is 1.5 MB. Anything bigger then that
//needs to be broken down. This will give lot's of flexability for the 
//size that the application can use, but gives us a fairly reasonable
//cap. There is a danger however, if a hacker were to cause a number 
//of connections to allocate the max allowed. The lowest possible size
//should be used when possible.
unsigned32 PacketBufferHeader::m_nMaxPacketSize = 1500000;

unsigned32 PacketBuffer::m_nUniqueID = 1;                   //0 is not allowed for a packet ID

//*************************************************************************
//
//   PacketBufferHeader
//
//*************************************************************************

unsigned32 PacketBufferHeader::getMaxPacketSize()
{
  return m_nMaxPacketSize;
}

/**
   Sets the max packet size to newSize. Does nothing to the actual packet
   buffer. This is just a number we use for reference.
**/
unsigned32 PacketBufferHeader::setMaxPacketSize(unsigned32 newSize)
{
  unsigned32 oldSize = m_nMaxPacketSize;
  m_nMaxPacketSize = newSize;
  return oldSize;
}

/**
   Make the header host-byte ordered, and reset the buffer size to zero.
**/
void PacketBufferHeader::reset()
{
  m_nBufferSize = 0;
  if(m_bNetworkReady)
    makeHostReady();
}

/**
   Does some simple tests to see if this header is valid. First checks to see if
   the cookie is our constant cookie value of PacketBuffer::pkCookie. Then checks if
   the command is non-zero. Lastly, we check if the buffersize value is better
   then the max packet size. If all these pass, then we can assume that the
   header is probably safe. 
**/
bool PacketBufferHeader::isValid()
{
  if(m_wCookie!=PacketBuffer::pkCookie) return false;
  if(!m_wCommand) return false;
  if(m_nBufferSize > getMaxPacketSize()) return false;

  //seems to be okay
  return true;
}

/**
   Convert the header longs and shorts into network byte order blah blah blah.

   \param bOverrideFailsafe [in] false (default) will not reorder the bytes if you
            have already. true if you want to anyway.
**/
void PacketBufferHeader::makeNetworkReady(bool bOverrideFailsafe)
{
  if(bOverrideFailsafe || !m_bNetworkReady)
  {
    m_nBufferSize = htonl(m_nBufferSize);
    m_wCookie = htons(m_wCookie);
    m_wCommand = htons(m_wCommand);
    m_bNetworkReady = true;
  }
}

/**
   Convert the header longs and shorts into host byte order.

   \param bOverrideFailsafe [in] false (default) will not reorder the bytes if
   you have already. true if you want to anyway.

   \note
   Used only in by BufferedSocket::parsForPacket() when a packet is just read in. The
   new object will be in host byte order, and the data read in will be in
   network byte order. So we need to force into host byte order even thought the
   object thinks it is already like that.
**/
void PacketBufferHeader::makeHostReady(bool bOverrideFailsafe)
{
  if(bOverrideFailsafe || m_bNetworkReady)
  {
    m_nBufferSize = ntohl(m_nBufferSize);
    m_wCookie = ntohs(m_wCookie);
    m_wCommand = ntohs(m_wCommand);
    m_bNetworkReady=false;
  }
}

//*************************************************************************
//
//   PacketBuffer
//
//*************************************************************************

PacketBuffer::~PacketBuffer()
{
  delete [] m_Buffer;
}

PacketBuffer& PacketBuffer::operator=(const PacketBuffer& packet)
{
#ifdef DEBUG
  //make sure that the pointer is within range
  unsigned32 addy1,addy2;
  addy1=(unsigned32)packet.m_pPointer;
  addy2=(unsigned32)packet.m_Buffer;
  assert(addy1 >= addy2 && addy1 <= addy2+packet.m_nBufferSizeMax);
#endif

  if(&packet==this)   //object assigned to itself
    return *this;
  delete [] m_Buffer; m_Buffer = m_pPointer = NULL;    

  m_Buffer = new unsigned8[packet.m_nBufferSizeMax];

  m_packetHeader   = packet.m_packetHeader;
  m_nBufferSizeMax = packet.m_nBufferSizeMax;
  m_bUsed          = packet.m_bUsed;
  m_nID            = m_nUniqueID++;
  m_pPointer       = m_Buffer+(unsigned32)(packet.m_pPointer-packet.m_Buffer);

  if(packet.m_packetHeader.m_nBufferSize)
    memcpy(m_Buffer,packet.m_Buffer,packet.m_packetHeader.m_nBufferSize);

  return *this;
}
PacketBuffer::PacketBuffer(const PacketBuffer& packet)
  : m_packetHeader(packet.m_packetHeader.m_wCommand,packet.m_packetHeader.m_wCookie),
    m_Buffer(0),
    m_pPointer(0),
    m_bUsed(true),
    m_nID(0),
    m_nBufferSizeMax(packet.m_nBufferSizeMax)
{
#ifdef DEBUG
  //make sure that the pointer is within range
  unsigned32 addy1,addy2;
  addy1=(unsigned32)packet.m_pPointer;
  addy2=(unsigned32)packet.m_Buffer;
  assert(addy1 >= addy2 && addy1 <= addy2+packet.m_nBufferSizeMax);
#endif

  m_Buffer = new unsigned8[packet.m_nBufferSizeMax];
  m_nBufferSizeMax = packet.m_nBufferSizeMax;
  m_packetHeader.m_nBufferSize    = packet.m_packetHeader.m_nBufferSize;
  m_packetHeader.m_wCookie        = packet.m_packetHeader.m_wCookie;
  m_packetHeader.m_wCommand       = packet.m_packetHeader.m_wCommand;
  m_bUsed                         = packet.m_bUsed;
  m_nID                           = m_nUniqueID++;
  m_pPointer                      = m_Buffer+m_packetHeader.m_nBufferSize;

#ifdef DEBUG
  memcpy(m_Buffer,packet.m_Buffer,packet.m_nBufferSizeMax);
#else
  memcpy(m_Buffer,packet.m_Buffer,packet.m_packetHeader.m_nBufferSize);
#endif    
}

/**
   The most commonly used packet constructor. You usually construct passing in
   the packet command. The command may also be refered to as the packet
   type. Basically, the command is how you tell what you are suppose to do with
   the packet once you receive it. 

   An example of creating one would be

   <pre>
   PacketBuffer ping(PacketBuffer::pcPing);
   </pre>

   "pc" part of "pcPing" stands for Packet Command. Normally, if you need
   additional packet commands, you create a new class, and dirive it from
   PacketBuffer. For instance:

   <pre>
   From mypackts.h:
   class mypackets : public PacketBuffer
   {
     public:
       pcBackflip = pcUser,     //start at user, don't start at anything below,
       pcCartWheel              //as they are reserved for ssobjects
   };
   </pre>
**/
PacketBuffer::PacketBuffer(unsigned16 wCmd,unsigned32 nSize)
  : m_packetHeader(wCmd,pkCookie),m_Buffer(0),m_pPointer(0),
    m_bUsed(true),m_nID(0),m_nBufferSizeMax(nSize)
{
  if(nSize)
  {
    m_Buffer = new unsigned8[m_nBufferSizeMax];
#ifdef DEBUG
    assert(m_Buffer);
    memset(m_Buffer,0,m_nBufferSizeMax);     //not used during release version for speed
#endif
  }
  m_packetHeader.m_wCommand = wCmd;
  m_packetHeader.m_wCookie = pkCookie;
  m_pPointer = m_Buffer;
  m_nID = m_nUniqueID++;
}

/**
   An empty packet is created with a default packet buffer size of
   PacketBuffer::DefaultPacketBufferSize. You normally only create an empty packet
   if you are using it to receive data. However, the most common way to receive
   data is to let BufferedSocket or SimpleServer create the packet for you.
**/
PacketBuffer::PacketBuffer()
  : m_packetHeader(),m_Buffer(0),m_pPointer(0),
    m_bUsed(true),m_nID(0),m_nBufferSizeMax(DefaultPacketBufferSize)
{
  m_Buffer = new unsigned8[m_nBufferSizeMax];
#ifdef DEBUG
  assert(m_Buffer);
  memset(m_Buffer,0,m_nBufferSizeMax);     //not used during release version for speed
#endif
  m_pPointer = m_Buffer;
  m_nID = m_nUniqueID++;
}

/**
   Calls the sockets recvPacket to read data into this packet object. Once the
   data has been received, process() is called. If this object is part of a
   Communicable object, then process is actually a call to
   Communicable::process which will in turn call Communicable::extract  
   method to start the attribute populating process.

   \throw
   PacketBufferException If the socket is invalid. 
**/
void PacketBuffer::receive(BufferedSocket* psocket)
{
  if(!psocket)
    throwPacketBufferException("socket is invalid");
  psocket->recvPacket(*this);
  process();
}

/** 
    Sends the packet over the socket psocket

    \throw
    PacketBufferException if the socket is not valid.
**/
void PacketBuffer::transmit(BufferedSocket* psocket)
{
  if(!psocket)
    throwPacketBufferException("socket is invalid");
  psocket->sendPacket(*this);
}

/**
   Puts the header into network-byte order. 

   \param bOverrideFailsafe [in] false (default) will not reorder the bytes if you
   have already. true if you want to anyway.
**/
void PacketBuffer::makeNetworkReady(bool bOverride)
{
  m_packetHeader.makeNetworkReady(bOverride);
}

/**
   Puts the header into host-byte order. 

   \param bOverrideFailsafe [in] false (default) will not reorder the bytes if
   you have already. true if you want to anyway.
**/
void PacketBuffer::makeHostReady(bool bOverride)
{
  m_packetHeader.makeHostReady(bOverride);
}

/**
   A PacketBuffer object contains a buffer to hold all the information you will be
   transmitting or receiving. This returns the actual buffer.

   \return A pointer to the begining of the buffer.

   \throw PacketBufferException If the buffer is not allocated.
**/
unsigned8* PacketBuffer::getBuffer() const 
{
  if(!m_Buffer)
    throwPacketBufferException("no buffer associated with this packet");
  return m_Buffer;
}

/**
   To keep track of where the PacketBuffer object last put data, a index pointer is 
   maintained that points one byte past the last data that was appended to
   the buffer. This returns that index.

   \return A pointer to the index pointer.
   \throw PacketBufferException If there is no buffer allocated. 
**/
unsigned8* PacketBuffer::getPointer() const
{
  if(!m_pPointer)
    throwPacketBufferException("no buffer associated with this packet");
  return m_pPointer;
}

/**
   If you wish to increase or decrease the size of your packets buffer, use this.
   It allocates new memory, copies the existing buffer data to the new memory
   location, then deletes the old memory. This isn't very efficent.

   \throw 
   PacketBufferException if there was no memory to allocate new packet. If this happens, 
   the old memory is left untouched, and is still valid.

   \return 
   Pointer to the new memory.
**/
unsigned8* PacketBuffer::resizeBuffer(unsigned32 nNewSize)
{
  assert(nNewSize);

  unsigned8* pNewMem = new unsigned8[nNewSize];
  int iBytes = min(nNewSize,getBufferSizeMax());
  if(!pNewMem) throwPacketBufferException("unable to resize memory");

#ifdef DEBUG
  memset(pNewMem,0,nNewSize);
#endif
  memcpy(pNewMem,m_Buffer,iBytes);
  delete [] m_Buffer;
  m_Buffer = pNewMem;
  m_pPointer = m_Buffer+getHeaderSize();
  return m_Buffer;
}

/**
   Puts header back to an initial state by making the index pointer point to the beginning 
   of the buffer, and calling the headers reset method which sets the buffer size to 0, and
   puts header into host byte order.
**/
void PacketBuffer::reset()
{
  m_pPointer = m_Buffer;
  m_packetHeader.reset();
#ifdef DEBUG
  if(m_Buffer)
    memset(m_Buffer,0xFF,m_nBufferSizeMax);
#endif    
}

/**
   Adds data to the end of the buffer, and increases the buffer size. Good way
   to add misc data. The size of the data that was appended is not stored
   however. This means that when you are extracting the data, you will some way
   to know how much data to extract. 

   \param pdata Pointer to the data you will be copying into the packet buffer.
   \param nSize The number of bytes you will be copying into the packet buffer.

   \throw
   PacketBufferException If there is not enough room left in the buffer.

   \note
   There is no extraction method to compliment this method. Using this method is
   discouraged. 
**/
void PacketBuffer::append(unsigned8* pdata,unsigned32 nSize)
{
  if(m_packetHeader.m_nBufferSize+nSize>getBufferSizeMax())
    throwPacketBufferException("ccBlob::append -- Data too large for buffer");

  memcpy(m_pPointer,pdata,nSize);
  m_pPointer+=nSize;      //move past the data we just put in
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies the zero terminated string pointed to by pString

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (const char* pString)
{
  if(m_packetHeader.m_nBufferSize+strlen(pString)+1>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << const char* -- Data too large for buffer");

  strcpy((char*)m_pPointer,pString);
  m_pPointer+=strlen(pString)+1;      //move past the data we just put in, plus the null character
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies the zero terminated string pointed to by pString

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (char* pString)
{
  if(m_packetHeader.m_nBufferSize+strlen(pString)+1>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << char* -- Data too large for buffer");

  strcpy((char*)m_pPointer,pString);
  m_pPointer+=strlen(pString)+1;      //move past the data we just put in, plus the null character
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies String to the buffer. String is stared as a zero terminated string. If
   String has no buffer, an empty string is stored. An empty string consists of
   a single zero byte.

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (CStr& String)
{
  if(m_packetHeader.m_nBufferSize+String.getLength()+1>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << char* -- Data too large for buffer");

  if((char*)String != NULL)
  {
    strcpy((char*)m_pPointer,String);
    m_pPointer+=String.getLength()+1;      //move past the data we just put in, plus the null character
  }
  else
  {
    *m_pPointer = 0;    //insert an empty string
    m_pPointer++;       
  }

  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is converted to network-byte order
   when it is stored in the buffer. 

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (signed16 Number)
{
  if(m_packetHeader.m_nBufferSize+sizeof(signed16)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << signed16 -- Data too large for buffer");

  signed16* pnumber = (signed16*)m_pPointer;
  *pnumber = htons(Number);
  m_pPointer+=sizeof(signed16);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is converted to network-byte order
   when it is stored in the buffer.

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (signed32 iNumber)
{
  if(m_packetHeader.m_nBufferSize+sizeof(signed32)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << signed32 -- Data too large for buffer");

  signed32* pnumber = (signed32*)m_pPointer;
  *pnumber = htonl(iNumber);
  m_pPointer+=sizeof(signed32);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is converted to network-byte order
   when it is stored in the buffer. 

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (unsigned8 Number)
{
  if(m_packetHeader.m_nBufferSize+sizeof(unsigned8)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << unsigned8 -- Data too large for buffer");

  memcpy(m_pPointer,(char*)&Number,sizeof(unsigned8));
  m_pPointer+=sizeof(unsigned8);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is converted to network-byte order
   when it is stored in the buffer. 

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (unsigned16 Number)
{
  if(m_packetHeader.m_nBufferSize+sizeof(unsigned16)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << unsigned16 -- Data too large for buffer");

  unsigned16* pnumber = (unsigned16*)m_pPointer;
  *pnumber = htons(Number);
  m_pPointer+=sizeof(unsigned16);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is converted to network-byte order
   when it is stored in the buffer. 

   \throw
   PacketBufferException If there is not enough room left in the buffer.
**/
void PacketBuffer::operator << (unsigned32 Number)
{
  if(m_packetHeader.m_nBufferSize+sizeof(unsigned32)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << unsigned32 -- Data too large for buffer");

  unsigned32* pnumber = (unsigned32*)m_pPointer;
  *pnumber = htonl(Number);
  m_pPointer+=sizeof(signed32);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is not converted to network-byte
   order, as there are no network conversion routines for this.

   \throw
   PacketBufferException If there is not enough room left in the buffer.

   \note
   This has not been tested on big-indian machines. You may want to convert 
   Number to a string, and pass it as a zero terminated string instead to
   be safe.
**/
void PacketBuffer::operator << (double Number)
{
  if(m_packetHeader.m_nBufferSize+sizeof(double)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << double -- Data too large for buffer");

  memcpy(m_pPointer,(char*)&Number,sizeof(double));
  m_pPointer+=sizeof(double);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}

/**
   Copies Number to the buffer. The number is not converted to network-byte
   order, as there are no network conversion routines for this.

   \throw
   PacketBufferException If there is not enough room left in the buffer.

   \note
   This has not been tested on big-indian machines. You may want to convert 
   Number to a string, and pass it as a zero terminated string instead to
   be safe.
**/
void PacketBuffer::operator << (float Number)
{
  if(m_packetHeader.m_nBufferSize+sizeof(float)>getBufferSizeMax())
    throwPacketBufferException("PacketBuffer::operator << float -- Data too large for buffer");

  memcpy(m_pPointer,(char*)&Number,sizeof(float));
  m_pPointer+=sizeof(float);
  m_packetHeader.m_nBufferSize=m_pPointer-m_Buffer;
}


/** 
    Copies the data from the buffer to pString. pString is assumed to have
    enough space to hold the string. If pString is not large enough, the results
    are undefined.

    It's a good idea to either use a CStr object instead, or allocate a
    string the same size as the PacketBuffers max buffer size. pString will be zero
    terminated.
**/
void PacketBuffer::operator >> (char* pString)
{
  strcpy(pString,(char*)m_pPointer);
  m_pPointer+=strlen(pString)+1;
}

/**
   Copies the zero terminated string in the buffer to String. This is a safer
   way to extract strings then the char* operator.
**/
void PacketBuffer::operator >> (CStr& String)
{
  String = (char*)m_pPointer;
  m_pPointer+=String.getLength()+1;
}

/**
   Copies the 32-bit number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is expected to be in network-byte
   order. When the insertion operator "<<" is used, it will be.
**/
void PacketBuffer::operator >> (signed32& Number)
{
  signed32* pnumber = (signed32*)m_pPointer;
  Number = ntohl(*pnumber);
  m_pPointer+=sizeof(signed32);
}

/**
   Copies the 16-bit number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is expected to be in network-byte
   order. When the insertion operator "<<" is used, it will be.
**/
void PacketBuffer::operator >> (signed16& Number)
{
  signed16* pnumber = (signed16*)m_pPointer;
  Number = ntohs(*pnumber);
  m_pPointer+=sizeof(signed16);
}

/**
   Copies the 8-bit (BYTE) number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is expected to be in network-byte
   order. When the insertion operator "<<" is used, it will be.
**/
void PacketBuffer::operator >> (unsigned8& Number)
{
  memcpy(&Number,m_pPointer,sizeof(unsigned8));
  m_pPointer+=sizeof(unsigned8);
}

/**
   Copies the 16-bit (WORD) number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is expected to be in network-byte
   order. When the insertion operator "<<" is used, it will be.
**/
void PacketBuffer::operator >> (unsigned16& Number)
{
  unsigned16* pnumber = (unsigned16*)m_pPointer;
  Number = ntohs(*pnumber);
  m_pPointer+=sizeof(unsigned16);
}

/**
   Copies the 32-bit (DWORD) number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is expected to be in network-byte
   order. When the insertion operator "<<" is used, it will be.
**/
void PacketBuffer::operator >> (unsigned32& Number)
{
  unsigned32* pnumber = (unsigned32*)m_pPointer;
  Number = ntohl(*pnumber);
  m_pPointer+=sizeof(unsigned32);
}

/**
   Copies the double number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is NOT expected to be in network-byte
   order as there is no network conversion routines for this. 

   \note
   This has not been tested on big-indian machines. You may want to convert the
   double number to a string, and pass it as a zero terminated string instead to
   be safe.
**/
void PacketBuffer::operator >> (double& Number)
{
  memcpy(&Number,m_pPointer,sizeof(double));
  m_pPointer+=sizeof(double);
}

/**
   Copies the float number in the buffer to Number. Number will be in
   host-byte order. The number in the buffer is NOT expected to be in network-byte
   order as there is no network conversion routines for this. 

   \note
   This has not been tested on big-indian machines. You may want to convert the
   float number to a string, and pass it as a zero terminated string instead to
   be safe.
**/
void PacketBuffer::operator >> (float& Number)
{
  memcpy(&Number,m_pPointer,sizeof(float));
  m_pPointer+=sizeof(float);
}


//*************************************************************************
//
//   NetFile
//
//*************************************************************************

NetFile::NetFile(const char* pszFilename,unsigned32 nSize,unsigned8* pdata) 
        : PacketBuffer(PacketBuffer::pcNetFileStart,strlen(pszFilename)+1+sizeof(unsigned32)),
          m_nFileSize(nSize),m_strFilename(pszFilename),m_pFileData(0)
{
  *this << m_nFileSize;
  *this << m_strFilename;
  m_pFileData = new unsigned8[nSize];
  assert(m_pFileData);
  memcpy(m_pFileData,pdata,nSize);
}

NetFile::~NetFile()
{
  if(m_pFileData) 
    delete [] m_pFileData;
}

//this will receive a compete file, no matter what the size. It will be commited to memory
//we are expecting a pcStart, pcData for each chunk of the file, and pcEnd to mark the end
//of the file
//TODO: when being used over and over, there is a memory leak.
// For example:
//  ClientConnector client(...,...);
//  NetFile file;
//  for(int i=0; i<5; i++)
//      client >> file;
//  net file should have leaks
void NetFile::receive(BufferedSocket* psocket)
{
  assert(psocket);
  char szFilename[MAX_PATH];

  //read in the pcStart that contains this files information
  psocket->recvPacket(*this);
  if(getCmd()!=pcNetFileStart)  //verify that this a pcStart packet
    throwNetFileException("received out of sequence packet at start");

  *this >> m_nFileSize;
  *this >> szFilename;
  m_strFilename = szFilename;
  m_pFileData = new unsigned8[m_nFileSize];
  assert(m_pFileData);

  //keep reading pcNetFileData packets and copying the bytes into our buffer
  //until we get a pcNetFileEnd packet
  PacketBuffer packetIn;
  unsigned32 nBytesContained=0,nBytesRead=0;
  unsigned8* ptr = m_pFileData;
  do
  {
    psocket->recvPacket(packetIn);
    if(packetIn.getCmd()!=pcNetFileData && packetIn.getCmd()!=pcNetFileEnd)
      throwNetFileException("received out of sequence packet during data transfer");
    if(packetIn.getCmd()==pcNetFileData)
    {
      packetIn >> nBytesContained;   //get how many bytes are included
      if(nBytesContained + nBytesRead > m_nFileSize)
        throwNetFileException("too much data received");
      memcpy(ptr,packetIn.getPointer(),nBytesContained);
      ptr+=nBytesContained;
      nBytesRead+=nBytesContained;
    }
  } while(packetIn.getCmd()==pcNetFileData);
}

void NetFile::transmit(BufferedSocket* psocket)
{
  assert(psocket);
  psocket->sendPacket(*this); //send pcNetFileStart

  //send pcNetFileData packets till we have sent all m_nfileSize bytes
  unsigned32 nMaxSize = 128;  //128 is at most, how many bytes I want to put into packet
  PacketBuffer packetData(pcNetFileData,nMaxSize+sizeof(unsigned32));
  unsigned32 nToSend;
  unsigned32 nSize = m_nFileSize;
  unsigned8* ptr = m_pFileData;
  while(nSize>0)
  {
    nToSend = min(nSize,nMaxSize);
    packetData << nToSend;
    packetData.append(ptr,nToSend);
    psocket->sendPacket(packetData);
    packetData.reset();
    nSize -= nToSend;
    ptr   += nToSend;
  }
  PacketBuffer packetEnd(pcNetFileEnd,0);
  psocket->sendPacket(packetEnd);
}
