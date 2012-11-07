/*
 * Created on Feb 6, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import java.io.IOException;
import java.net.Socket;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class BufferedSocketTest extends TestCase
{
    protected void setUp(){}
    public static Test suite() {return new TestSuite(BufferedSocketTest.class);}
    
    public void testConstructiont()
    {
        try 
        {
            Socket s=new Socket();
            BufferedSocket sock=new BufferedSocket(s);
            assertTrue(sock.m_socket==s);
            assertTrue(sock.m_buffer!=null && sock.m_buffer.length==BufferedSocket.BUFFER_SIZE);
            assertTrue(sock.m_bytesInBuffer==0);
            assertTrue(sock.m_out==s.getOutputStream());
            assertTrue(sock.m_in==s.getInputStream());
        }
        catch(IOException e)
        {
        }
    }

    public void testParsingOkayPacket()
    {
        byte[] packetData=
            {
                0,0,0,0x12,     //data size
                0x4C,0x50,      //cookie
                0,0x02,           //command pcLoginOkay
                0x4c,0x65,0x65,0x0, //Lee
                0x50,0x61,0x74,0x74,0x65,0x72,0x73,0x6f,0x6e,0x0,   //Patterson
                0x0,0x0,0x10,(byte)0xe1   //user id of 4321
            };
        
        try
        {
            BufferedSocket socket=new BufferedSocket();
            socket.m_buffer=packetData;
            socket.m_bytesInBuffer=packetData.length;
            
            //make sure the packet came out okay
            PacketBuffer packet=socket.parseForPacket();
            for(int i=0; i<packetData.length; i++)
                assertTrue(packetData[i]==packet.m_buffer[i]);

            assertTrue(packet.header().bufferSize()==0x12);
            assertTrue(packet.header().cookie() == PacketBuffer.COOKIE);
            assertTrue(packet.command()==PacketBuffer.pcLoginOkay);
            assertTrue(packet.header().command()==PacketBuffer.pcLoginOkay);
            
            assertTrue(packet.packetSize()==PacketBuffer.headerSize());    //since the index is pointed to the beginning of the data

            //check that the data looks right after extraction
            String first=packet.extractString();
            String last=packet.extractString();
            int userid=packet.extractInt();
            assertTrue(first.equals("Lee"));
            assertTrue(last.equals("Patterson"));
            assertTrue(userid==4321);
        }
        catch(IOException e)
        {
            fail("Should not throw an ioexecption");
        }
    }
    
    public void testBufferMovedOkay()
    {
        byte[] packetData=
            {
                0,0,0,0x12,     //data size
                0x4C,0x50,      //cookie
                0,0x02,           //command pcLoginOkay
                0x4c,0x65,0x65,0x0, //Lee
                0x50,0x61,0x74,0x74,0x65,0x72,0x73,0x6f,0x6e,0x0,   //Patterson
                0x0,0x0,0x10,(byte)0xe1,   //user id of 4321
                0x1,0x2,0x3,0x4     //data size
            };
        try
        {
            BufferedSocket socket=new BufferedSocket();
            socket.m_buffer=packetData;
            socket.m_bytesInBuffer=packetData.length;
            
            //make sure the packet came out okay
            socket.parseForPacket();
            
            //make sure the remaining data got moved correctly
            assertTrue(socket.m_bytesInBuffer==4);
            for(int i=0; i<4; i++)
                assertTrue(socket.m_buffer[i]==packetData[i]);
        }
        catch(IOException e)
        {
            fail("Should not throw an ioexecption");
        }
    }
    
    public void testParsingBadHeaderSize()  
    {
        //create an invalid sized packet
        int size=PacketBuffer.MAX_PACKET_SIZE+1;
        byte[] packetData=
            {
                0,0,0,0,         //data size
                0x4C,0x50,      //cookie
                0,0x02,           //command pcLoginOkay
                0x4c,0x65,0x65,0x0, //Lee
                0x50,0x61,0x74,0x74,0x65,0x72,0x73,0x6f,0x6e,0x0,   //Patterson
                0x0,0x0,0x10,(byte)0xe1   //user id of 4321
            };
        packetData[0] = (byte)((size&0xFF000000)/0x1000000);
        packetData[1] = (byte)((size&0x00FF0000)/0x10000);
        packetData[2] = (byte)((size&0x0000FF00)/0x100);
        packetData[3] = (byte)(size&0x000000FF);
        
        try
        {
            BufferedSocket socket=new BufferedSocket();
            socket.m_buffer=packetData;
            socket.m_bytesInBuffer=packetData.length;

            socket.parseForPacket();    //should throw an IOException 
            fail("Should have thrown an IOException");
        }
        catch(IOException e)
        {
            assertTrue(true);
        }
    }
   
    public void testBadCookieValue()
    {
        //create an invalid sized packet
        byte[] packetData=
            {
                0,0,0,0x12,     //data size
                0x4C,0x00,      //bad cookie
                0,0x02,         //command pcLoginOkay
                0x4c,0x65,0x65,0x0, //Lee
                0x50,0x61,0x74,0x74,0x65,0x72,0x73,0x6f,0x6e,0x0,   //Patterson
                0x0,0x0,0x10,(byte)0xe1   //user id of 4321
            };
        
        try
        {
            BufferedSocket socket=new BufferedSocket();
            socket.m_buffer=packetData;
            socket.m_bytesInBuffer=packetData.length;

            socket.parseForPacket();    //should throw an IOException 
            fail("Should have thrown an IOException");
        }
        catch(IOException e)
        {
            assertTrue(true);
        }
    }
    
    public void testParsingHeaderTooSmall()
    {
        byte[] packetHeaderTooSmall=
            {
                0,0,0,0x12,     //data size
                0x4C,0x50,      //cookie
                0
            };
        try
        {
            BufferedSocket socket=new BufferedSocket();
            socket.m_buffer=packetHeaderTooSmall;
            socket.m_bytesInBuffer=packetHeaderTooSmall.length;

            assertTrue(socket.parseForPacket()==null);    //should throw an IOException
            
        }
        catch(IOException e)
        {
            fail("Should not throw an ioexecption");
        }
    }

    public void testParsingDataTooSmall()
    {
        byte[] packetDataTooSmall=
            {
                0,0,0,0x12,     //data size
                0x4C,0x50,      //cookie
                0,0x02,           //command pcLoginOkay
                0x4c,0x65,0x65,0x0, //Lee
                0x50,0x61,0x74,0x74,0x65,0x72,0x73,0x6f,0x6e,0x0,   //Patterson
                0x0
            };
        try
        {
            BufferedSocket socket=new BufferedSocket();
            socket.m_buffer=packetDataTooSmall;
            socket.m_bytesInBuffer=packetDataTooSmall.length;

            assertTrue(socket.parseForPacket()==null);    //should throw an IOException
            
        }
        catch(IOException e)
        {
            fail("Should not throw an ioexecption");
        }
    }
}

