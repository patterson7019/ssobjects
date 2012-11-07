/*
 * Created on Feb 5, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import ssobjects.PacketBuffer;

public class PacketBufferTest extends TestCase
{
    protected PacketBuffer packet;
    protected void setUp()
    {
        packet=new PacketBuffer(PacketBuffer.pcPing,20);
    }
    
    public static Test suite()
    {
        return new TestSuite(PacketBufferTest.class);
    }

    public void testHeader()
    {
        assertTrue(PacketBuffer.headerSize()==8);
        assertTrue(packet.header().cookie() == PacketBuffer.COOKIE);
        assertTrue(packet.command() == PacketBuffer.pcPing);
        assertTrue(packet.header().bufferSize()==0);
    }
    
    public void testPacketCommands()
    {
        int i=0;
        assertTrue(PacketBuffer.pcInvalid==i++);        ///<  0 - Invalid packet or not initialized
        assertTrue(PacketBuffer.pcLogin==i++);            ///<  1 - server wants client to login
        assertTrue(PacketBuffer.pcLoginOkay==i++);        ///<  2 - user was validated okay
        assertTrue(PacketBuffer.pcLoginBad==i++);         ///<  3 - user was not validated 
        assertTrue(PacketBuffer.pcLogout==i++);           ///<  4 - client is logging out
        assertTrue(PacketBuffer.pcGetVersion==i++);       ///<  5 - version info
        assertTrue(PacketBuffer.pcVersion==i++);          ///<  6 - version info
        assertTrue(PacketBuffer.pcPing==i++);             ///<  7 - we are expecting a pong back
        assertTrue(PacketBuffer.pcPong==i++);             ///<  8 - reply to a ping
        assertTrue(PacketBuffer.pcStatus==i++);           ///<  9 - generic status query/result (18)
        assertTrue(PacketBuffer.pcNetFileStart==i++);     ///< 10 - file being sent over network contains: filesize, filename
        assertTrue(PacketBuffer.pcNetFileData==i++);      ///< 11 - data for network file contains: size (size of contained data)
        assertTrue(PacketBuffer.pcNetFileEnd==i++);       ///< 12 - terminater contains: nothing
        assertTrue(PacketBuffer.pcClosed==i++);           ///< 13 - a connection was closed
        assertTrue(PacketBuffer.pcNewConnection==i++);    ///< 14 - a new connection was made
        assertTrue(PacketBuffer.pcAuthenticate==i++);     ///< 15 - client is authenticating
        assertTrue(PacketBuffer.pcAuthReply==i++);        ///< 16 - server is replying to authentication request

        assertTrue(PacketBuffer.pcNoop==100);         ///< 100 -  no op. Do nothing with this, except to have a case in msg handler for it
        assertTrue(PacketBuffer.pcUser==256);         ///< 256 (100H) - user defined packets start here, first 55H are reserved
    }
    
    public void testAppendExtract()
    {
        final String STRING_DATA="Lee Patterson";
        final short SHORT_DATA=12;
        final int INT_DATA=3456;

        packet.append(STRING_DATA);
        packet.append(SHORT_DATA);
        packet.append(INT_DATA);
        packet.packHeader();
        
        packet.resetIndex();

        String stringData;
        short shortData;
        int intData;
        
        stringData=packet.extractString();
        shortData=packet.extractShort();
        intData=packet.extractInt();

        assertTrue(stringData.equals(STRING_DATA));
        assertTrue(shortData==SHORT_DATA);
        assertTrue(intData==INT_DATA);
    }
    
    public void testByteConstructor()
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
        
        PacketBuffer p=new PacketBuffer(packetData);
        byte[] packetBuffer=p.buffer();
        
        assertTrue(packetBuffer.length==packetData.length);
        
        for(int i=0; i<packetData.length; i++)
            assertTrue(packetData[i]==packetBuffer[i]);
        
        assertTrue(p.header().bufferSize()==0x12);
        assertTrue(p.header().cookie() == PacketBuffer.COOKIE);
        assertTrue(p.command()==PacketBuffer.pcLoginOkay);
        assertTrue(p.header().command()==PacketBuffer.pcLoginOkay);
        
        assertTrue(p.packetSize()==PacketBuffer.headerSize());    //since the index is pointed to the beginning of the data

        //check that the data looks right after extraction
        String first=p.extractString();
        String last=p.extractString();
        int userid=p.extractInt();
        assertTrue(first.equals("Lee"));
        assertTrue(last.equals("Patterson"));
        assertTrue(userid==4321);
    }
}
