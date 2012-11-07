/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software
        http://ssobjects.sourceforge.net

        created  :  5/24/1999
        filename :  packet.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Packet class for sending data over sockets
                
*********************************************************************/

#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

#include <time.h>
#include "defs.h"
#include "generalexception.h"
#include "logs.h"


namespace ssobjects
{


class ClientConnector;
class BufferedSocket;
class PacketBuffer;
class Communicable;

//typedefs for static headerSize function can get the correct size

#define typeCookie unsigned16
#define typeBufferSize unsigned32
#define typeCommand unsigned16

/**
   \brief
   Header information that is sent with every packet.

   The header is what tells you what command/type of packet this is, and how big it is.
   The header is a vital part of the PacketBuffer class.
**/
//TODO: need a copy construtor
class PacketBufferHeader
{
    friend class PacketBuffer;
    friend class Communicable;
  public:
    //header block starts here -->
    typeBufferSize  m_nBufferSize;        ///< How much data is in this packet, not including the header.
    typeCookie      m_wCookie;            ///< Special constant value used to determin if the packet is valid. Not to be mistaken for cookies in browsers.
    typeCommand     m_wCommand;           ///< The command, or type of packet this is.
    // <--
    static unsigned32 m_nMaxPacketSize;   ///< How big the buffer is allowed to get.
    bool        m_bNetworkReady;          ///< True when the header is in network byte order, false otherwise.
  public:
    PacketBufferHeader() : m_nBufferSize(0),m_wCookie(0),m_wCommand(0),m_bNetworkReady(false){};
    PacketBufferHeader(unsigned16 wCommand,unsigned16 wCookie) : m_nBufferSize(0),m_wCookie(wCookie),m_wCommand(wCommand),m_bNetworkReady(false){};
    void        makeNetworkReady(bool bOverrideFailsave=false);         ///< Convert header to network byte order   (htonl)
    void        makeHostReady(bool bOverrideFailsafe=false);            ///< Convert header from network byte order (ntohl)
    unsigned8*  getHeader() const {return (unsigned8*)&m_nBufferSize;}  ///< Returns a pointer to the header data.
    unsigned32  getBufferSize() const {return m_nBufferSize;}           ///< Returns the current buffer size.
    unsigned16  getCookie() const {return m_wCookie;}                   ///< Returns the cookie.
    unsigned16  getCmd() const {return m_wCommand;}                     ///< Returns the command.
    void        reset();                                                ///< Puts header to initial state.
    bool        isValid();                                              ///< Verify this header has valid values.
    bool        isNetworkReady() const {return m_bNetworkReady;}        ///< Returns true if the header is in network byte order, false otherwise.

    static      unsigned32 getMaxPacketSize();                          ///< Get the max allowed packet size.
    static      unsigned32 setMaxPacketSize(unsigned32);                ///< Set what the limit should be.
    static unsigned32  getSize() 
    {return sizeof(typeBufferSize)+sizeof(typeCookie)+sizeof(typeCommand);} ///< Returns the actual size of the header.
};

/**
   \brief
   Packet object used for storing data that is sent or received.

   The packet object is the base class for storing any information you indend to
   send, and when data is received, it is put into a PacketBuffer object. The object
   has operators for storing and extracting 8, 16 or 32 bit numbers, as well as
   zero terminated strings and CStr objects.

   To store data in the packet, you would typically construct the packet with
   the command type, and then insert data into it via the "<<" operator. For
   example:
   <pre>
   ...
   PacketBuffer ping(PacketBuffer::pcAuthenticate);
   unsigned32 nVersion = 100; //version 1.00
   ping << "lpatterson";      //user name
   ping << "mypassword";      //password
   ping << 100;               //version
   ...

   The server would extract the data like this:

   ...
   pPacket = psocket->recvPacket();
   CStr sUserName;
   CStr sPassword;
   unsigned32 nVersion;
   *pPacket >> sUserName;
   *pPacket >> sPassword;
   *pPacket >> nVersion;
   ...
   </pre>
**/
class PacketBuffer 
{
  public:
    /**
       These enums are provided for convience. When you construct a new Packet
       object, you will pass a packet command listed here, or from one you
       create. For instance, you would create a new packet like this:

       <pre>
       PacketBuffer login(PacketBuffer::pcLogin);
       </pre>

       When you want to create your own packet commands, you should create a new
       class and dirive it from PacketBuffer, and start your enumeration from
       pcUser. All ones below pcUser are reserved for furture use. "pc" part 
       stands for Packet Command.

       <pre>
       class ChessPacket : public PacketBuffer
       {
         public:
           pcNewGame = pcUser,  //start your packets at pcUser and up
           pcQuitGame,
           pcStartGame
       };

       ...
       //create a new game packet
       PacketBuffer newGame(ChessPacket::pcNewGame);
       ...
       </pre>
    **/
    enum    //standard commands
      {
      pcInvalid=0,        ///<  0 - Invalid packet or not initialized
      pcLogin,            ///<  1 - server wants client to login
      pcLoginOkay,        ///<  2 - user was validated okay
      pcLoginBad,         ///<  3 - user was not validated 
      pcLogout,           ///<  4 - client is logging out
      pcGetVersion,       ///<  5 - version info
      pcVersion,          ///<  6 - version info
      pcPing,             ///<  7 - we are expecting a pong back
      pcPong,             ///<  8 - reply to a ping
      pcStatus,           ///<  9 - generic status query/result (18)
      pcNetFileStart,     ///< 10 - file being sent over network contains: filesize, filename
      pcNetFileData,      ///< 11 - data for network file contains: size (size of contained data)
      pcNetFileEnd,       ///< 12 - terminater contains: nothing
      pcClosed,           ///< 13 - a connection was closed
      pcNewConnection,    ///< 14 - a new connection was made
      pcAuthenticate,     ///< 15 - client is authenticating
      pcAuthReply,        ///< 16 - server is replying to authentication request

      //debugging packets
      pcNoop=100,         ///< 100 -  no op. Do nothing with this, except to have a case in msg handler for it

      pcUser=256,         ///< 256 (100H) - user defined packets start here, first 55H are reserved
      };

    enum 
      {
      pkCookie=0x4C50                 ///< Constant value used to make sure a packet is valid.
      }; 
    enum 
      {
      DefaultPacketBufferSize=1000    ///< Default size of a packet.
      };  

    //TODO: packet header should be protected but isn't as I need the info in the servsocket class, and hadn't thought of making it a friend till now :)
    PacketBufferHeader  m_packetHeader;             ///< Contains information about this packet
  protected:
    unsigned8*      m_Buffer;                   ///< Buffer that holds the data holds data that will be sent

    unsigned8*      m_pPointer;                 ///< Current position in the buffer 
    bool            m_bUsed;                    ///< Is this packet in use?
    unsigned32      m_nID;                      ///< Unique ID
    unsigned32      m_nBufferSizeMax;           ///< How big the buffer can get

    //static data
    static  unsigned32    m_nUniqueID;          ///< Unique ID to keep all packets unique

    //static methods
  public:
    static unsigned32     getHeaderSize()     {return PacketBufferHeader::getSize();}
    virtual void makeNetworkReady(bool bOverrideFailsave=false);        ///< Convert header to network byte order   (htonl)
    virtual void makeHostReady(bool bOverrideFailsave=false);           ///< Convert header from network byte order (ntohl)
        
    //friend methods
  protected:
    friend class ClientConnector;                       ///< Friend of ClientConnector class.
    friend class BufferedSocket;                        ///< Friend of BufferedSocket class.
    virtual void receive(BufferedSocket*);        ///< Called to fill itself with data
    virtual void transmit(BufferedSocket*);       ///< Called to send itself
    virtual void process() {}               ///< Called when data is read in from ccClient. This is an empty implimentation. You need to override it if you want this functionality.

    //overloadable methods
  public:
    virtual unsigned32  getPacketSize()    const {return (unsigned32)(getHeaderSize() + getBufferSize());}  ///< Returns the size of the packet buffer plus the size of the header.
    virtual unsigned32  getBufferSizeMax() const {return m_nBufferSizeMax;}                                 ///< Returns the maximum size of the packet.
    virtual unsigned32  getBufferSize()    const {return m_packetHeader.getBufferSize();}                   ///< Returns the size of the buffer.
    virtual unsigned8*  getBuffer()        const;                                                           ///< Returns a pointer to the begining of the packets buffer.
    virtual unsigned8*  getPointer()       const;                                                           ///< Returns the pointer to the buffer contents
    virtual unsigned8*  getHeader()        const {return m_packetHeader.getHeader();}                       ///< Returns a pointer to the first byte of the header for this packet.
    virtual unsigned8*  resizeBuffer(unsigned32 nNewSize);                                                  ///< Change the size of the packet buffer.

  public:
    PacketBuffer(const PacketBuffer&);                                  ///< copy constructor
    PacketBuffer& operator=(const PacketBuffer& packet);      ///< assignment operator
    PacketBuffer(unsigned16 wCmd,unsigned32 nSize=DefaultPacketBufferSize); ///< Most commonly used constructor
    PacketBuffer();                                                         ///< Construct an empty packet. 
    virtual ~PacketBuffer();

    //regular methods
    unsigned32  getID()       {return m_nID;}                           ///< Returns the unique ID of this packet.
    unsigned16  getCmd() const {return m_packetHeader.getCmd();}        ///< Returns the command (type) of this packet.
    unsigned16  cookie() const {return m_packetHeader.getCookie();}     ///< Returns the cookie; the always constant value.
    void        rewind()      {m_pPointer = m_Buffer;}                  ///< Puts the index pointer to the beginning of the buffer.
    void        reset();                                                ///< Resets the packet and the packets header.
    void        append(unsigned8* pdata,unsigned32 nSize);              ///< Add data to the end of the buffer.

  public:
    //overloads
    void operator << (const char*);   ///< Add a zero terminated string.
    void operator << (char*);         ///< Add a zero terminated string.
    void operator << (CStr&);     ///< Add a zero terminated string.
    void operator << (signed32);      ///< Add a signed 32-bit number.
    void operator << (unsigned32);    ///< Add a unsigned 32-bit number.
    void operator << (signed16);      ///< Add a signed 16-bit number.
    void operator << (unsigned16);    ///< Add a unsigned 16-bit number.
    void operator << (unsigned8);     ///< Add a signed 8-bit number.
    void operator << (double);        ///< Add a double number.
    void operator << (float);         ///< Add a float number.
    
    void operator >> (char*);         ///< Extract a zero terminated string.
    void operator >> (CStr&);     ///< Extract a zero terminated string.
    void operator >> (signed32&);     ///< Extract a signed 32-bit number.
    void operator >> (unsigned32&);   ///< Extract a unsigned 32-bit number.
    void operator >> (signed16&);     ///< Extract a signed 16-bit number.
    void operator >> (unsigned16&);   ///< Extract a unsigned 16-bit number
    void operator >> (unsigned8&);    ///< Extract a unsigned 8-bit number.
    void operator >> (double&);       ///< Extract a double number.
    void operator >> (float&);        ///< Extract a float number
};

#define throwPacketBufferException(m) (throw PacketBufferException(m,__FILE__,__LINE__))
/**
   \brief
   PacketBuffer object errors.
**/
class PacketBufferException : public GeneralException
{
  public: // Constructor/destructor
    PacketBufferException(char* pchMessage,const char* pFname,const int iLine) 
      : GeneralException(pchMessage,pFname,iLine){};
};

class NetFile : public PacketBuffer
{
  protected:  //attributes
    unsigned32 m_nFileSize;
    CStr       m_strFilename;
    unsigned8* m_pFileData;

  protected:  //methods
    virtual void receive(BufferedSocket* psocket);
    virtual void transmit(BufferedSocket* psocket);

  public:     //construction
    NetFile(const char* pszFilename,unsigned32 nSize,unsigned8* pdata);
    NetFile() : m_nFileSize(0),m_strFilename(),m_pFileData(0){};
    virtual ~NetFile();

  public:     //user callable methods
    CONSTCHAR*       getFilename()   {return m_strFilename;}
    const unsigned32  getSize()      {return m_nFileSize;}
    unsigned8*        getData()      {return m_pFileData;}

  private:
    //unused overloads
    NetFile(const NetFile&);
    NetFile& operator=(const NetFile&);
};

#define throwNetFileException(m) (throw NetFileException(m,__FILE__,__LINE__))

class NetFileException : public PacketBufferException
{
  public: // Constructor/destructor
    NetFileException(char* pchMessage,const char* pFname,const int iLine) 
      : PacketBufferException(pchMessage,pFname,iLine){};
};

//
// all objects that will be sent over internet will be derived from 
// this class. You must override the two pure virtual methods to impliment
// your new object. You don't override process(). Just the pure methods.
//
// All communicable objects can be ACKed or NAKed. 
//
class Communicable : public PacketBuffer
{
  public:
    Communicable() {}
    Communicable(unsigned16 wCmd) : PacketBuffer(wCmd) {}
//        Communicable(PacketBuffer* ppacket) {extract(ppacket);}   //example of how to call extract

  protected:
    //extract data from packet, and fill your class data when receiving
    //you should override extraceData, and call extract when constructing with a packet pointer
    //
    virtual void extract(PacketBuffer* ppacket) {extractHeader(ppacket); extractData(ppacket);}
    virtual void extractData(PacketBuffer*)=0;
    virtual void extractHeader(PacketBuffer* ppacket) 
    {
      //TODO: should use a copy constructor or something to copy entire header
      //TODO  in one shot instead of each member
      m_packetHeader.m_nBufferSize = ppacket->m_packetHeader.m_nBufferSize;
      m_packetHeader.m_wCookie     = ppacket->m_packetHeader.m_wCookie;
      m_packetHeader.m_wCommand    = ppacket->m_packetHeader.m_wCommand;
    }

    //take your class data, and put it into our packet buffer for sending
    virtual void stuff()=0;
    virtual void process() {extract(this);}      //you don't override this
};

  };  //namespace

#endif

