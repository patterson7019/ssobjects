/*
 * Created on Feb 5, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class PacketBuffer 
{
    class PacketBufferHeader
    {
        protected static final int HEADER_SIZE=8;

        protected int m_bufferSize;
        protected short m_wCookie;
        protected short m_wCommand;
        
        public PacketBufferHeader(int wCmd) {m_wCommand=(short)wCmd;m_wCookie=COOKIE;m_bufferSize=0;}
        public PacketBufferHeader(short cmd,short cookie,int size)
        {
            m_bufferSize=size;
            m_wCookie=cookie;
            m_wCommand=cmd;
        }
        public int bufferSize() {return m_bufferSize;}
        public short cookie() {return m_wCookie;}
        public short command() {return m_wCommand;}

        public void setBufferSize(int size) {m_bufferSize=size;}
        public void inc(int size) {m_bufferSize+=size;}
        public void dec(int size) {m_bufferSize-=size;}
    }

    public final static int MAX_PACKET_SIZE=3000;
    public final static short COOKIE = 0x4C50; 

    public static final short pcInvalid=0;          ///<  0 - Invalid packet or not initialized
    public static final short pcLogin=1;            ///<  1 - server wants client to login
    public static final short pcLoginOkay=2;        ///<  2 - user was validated okay
    public static final short pcLoginBad=3;         ///<  3 - user was not validated 
    public static final short pcLogout=4;           ///<  4 - client is logging out
    public static final short pcGetVersion=5;       ///<  5 - version info
    public static final short pcVersion=6;          ///<  6 - version info
    public static final short pcPing=7;             ///<  7 - we are expecting a pong back
    public static final short pcPong=8;             ///<  8 - reply to a ping
    public static final short pcStatus=9;           ///<  9 - generic status query/result (18)
    public static final short pcNetFileStart=10;    ///< 10 - file being sent over network contains: filesize, filename
    public static final short pcNetFileData=11;     ///< 11 - data for network file contains: size (size of contained data)
    public static final short pcNetFileEnd=12;      ///< 12 - terminater contains: nothing
    public static final short pcClosed=13;          ///< 13 - a connection was closed
    public static final short pcNewConnection=14;   ///< 14 - a new connection was made
    public static final short pcAuthenticate=15;    ///< 15 - client is authenticating
    public static final short pcAuthReply=16;       ///< 16 - server is replying to authentication request

    //debugging packets
    public static final short pcNoop=100;           ///< 100 -  no op. Do nothing with this, except to have a case in msg handler for it

    public static final short pcUser=256;           ///< 256 (100H) - user defined packets start here, first 55H are reserved

    protected PacketBufferHeader m_header;
    protected byte[] m_buffer;
    protected int m_bufferIndex;
    protected int m_headerOffset;

    public PacketBuffer(int wCmd)
    {
        this(wCmd,0);
    }
    public PacketBuffer(int wCmd,int size)
    {
        m_header=new PacketBufferHeader(wCmd);
        m_headerOffset=PacketBufferHeader.HEADER_SIZE;
        m_bufferIndex=m_headerOffset;
        m_buffer=new byte[PacketBufferHeader.HEADER_SIZE+size];
    }

    public PacketBuffer(byte[] packet) throws IllegalArgumentException
    {
        int dataSize=extractInt(packet,0);
        short wCookie=extractShort(packet,4);
        short wCmd=extractShort(packet,6);
        
        if(dataSize>MAX_PACKET_SIZE)
            throw new IllegalArgumentException("Packet size too large");  //make sure we are not trying to create a HUGE packet, prevent hacking
        m_header=new PacketBufferHeader(wCmd,wCookie,dataSize);
        m_headerOffset=headerSize();
        m_bufferIndex=m_headerOffset;
        m_buffer=new byte[dataSize+headerSize()];
        for(int i=0; i<PacketBufferHeader.HEADER_SIZE+dataSize; i++)
            m_buffer[i]=packet[i];
    }
    
    /** Store the header data into the buffer. Called before sending the packet. */
    public void packHeader()
    {
        int index=m_bufferIndex;
        int buffSize=m_header.bufferSize();
        m_bufferIndex=0;
        append(m_header.bufferSize());
        append(m_header.cookie());
        append(m_header.command());
        m_bufferIndex=index;
        m_header.setBufferSize(buffSize);
    }

    /** Returns the buffer that contains the header and data. */
    public byte[] buffer() {return m_buffer;}
    /** Returns the size of the header plus the size of the data that is currently in the buffer.*/
    public int packetSize() {return m_bufferIndex;}
    public short command() {return m_header.command();}
    public PacketBufferHeader header() {return m_header;}
    public int bufferIndex() {return m_bufferIndex;}
    public void resetIndex() { m_bufferIndex=m_headerOffset; }
    public static int headerSize() {return PacketBufferHeader .HEADER_SIZE;}

    public void append(byte byData)
    {
        m_header.inc(1);
        m_buffer[m_bufferIndex++]=byData;
    }
    public byte extractByte()
    {
        m_header.dec(1);
        return m_buffer[m_bufferIndex++];
    }
    
    public void append(short wData)
    {
        m_header.inc(2);
        m_buffer[m_bufferIndex++] = (byte)(((int)wData&0xFF00)/0x100);
        m_buffer[m_bufferIndex++] = (byte)((int)wData&0xFF);
    }
    public short extractShort()
    {
        m_header.dec(2);
        short value=extractShort(m_buffer,m_bufferIndex);
        m_bufferIndex+=2;
        return value;
    }
    static public short extractShort(byte[] buffer,int offset)
    {
        short value=(short)((buffer[offset++]&0xFF)*0x100);
        value += buffer[offset++]&0xFF;
        return value;
    }
    
    public void append(int iData)
    {
        m_header.inc(4);
        m_buffer[m_bufferIndex++] = (byte)((iData&0xFF000000)/0x1000000);
        m_buffer[m_bufferIndex++] = (byte)((iData&0x00FF0000)/0x10000);
        m_buffer[m_bufferIndex++] = (byte)((iData&0x0000FF00)/0x100);
        m_buffer[m_bufferIndex++] = (byte)(iData&0x000000FF);
    }
    public int extractInt() 
    {
        m_header.dec(4);
        m_bufferIndex+=4;
        return extractInt(m_buffer,m_bufferIndex-4);
    }
    static public int extractInt(byte[] buffer,int offset)
    {
        int value=(buffer[offset++]&0xFF)*0x1000000;
        value+=(buffer[offset++]&0xFF)*0x10000;
        value+=(buffer[offset++]&0xFF)*0x100;
        value+=buffer[offset]&0xFF;
        return value;
    }
    
    public void append(String s)
    {
        //TODO: Want to make C++ code use string length instead of null terminated
        //append a string preceded by it's length
//        short slen=(short)s.length();
//        append(slen);
//        for(int i=0; i<s.length(); i++)
//            m_buffer[m_bufferIndex++]=(byte)s.charAt(i);
//        m_header.inc(slen);   // string data
        
        //append a null terminated string
        short slen=(short)s.length();
        for(int i=0; i<s.length(); i++)
            m_buffer[m_bufferIndex++]=(byte)s.charAt(i);
        m_buffer[m_bufferIndex++]=0;    //null terminate
        m_header.inc(slen+1);   // string data, null terminated
    }
    public String extractString()
    {
        //extract a null terminated string
        StringBuffer buffer = new StringBuffer();
        char c=0;
        do
        {
            c=(char)(extractByte()&0xFF);
            m_header.dec(1);          // string data, null terminated
            if(c!=0)
                buffer.append(c);
        } while(c!=0);
        return buffer.toString();
    }
    
    public static void main(String[] args) throws Exception 
    {
        System.out.println("Connecting to test server...");
        PacketBuffer packet = new PacketBuffer(pcPing,0);
        Socket sock=new Socket("fireant",9999);
        OutputStream out=sock.getOutputStream();
        InputStream in=sock.getInputStream();
        System.out.println("Sending ping");
        out.write(packet.buffer(),0,packet.packetSize());
        out.flush();
        packet.resetIndex();
        in.read(packet.buffer(),0,8);
        System.out.println("Got pong");
        sock.close();
        System.out.println("Connection closed\n");

        packet=new PacketBuffer(pcLogin,255);
        packet.append("someusername");
        packet.append("somepassword");
        packet.packHeader();
        System.out.println("Connecting to test server...");
        BufferedSocket bsock=new BufferedSocket(new Socket("fireant",9999));
        System.out.println("Sending login packet");
        bsock.sendPacket(packet);
        System.out.println("Recieving reply");
        packet=bsock.recvPacket();
        if(packet.command()==PacketBuffer.pcLoginOkay)
        {
            String first=packet.extractString();
            String last=packet.extractString();
            int userid=packet.extractInt();
            System.out.println("user="+first+" "+last+" userid="+userid);
        }
        else if(packet.command()==PacketBuffer.pcLoginBad)
        {
            System.out.println("Login Failed: "+packet.extractString());
        }
        bsock.close();
        System.out.println("Connection closed");
    }
}

