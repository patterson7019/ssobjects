package ssobjects;


import static org.junit.Assert.*;

import java.nio.ByteBuffer;

import org.junit.Before;
import org.junit.Test;


public class TelnetServerSocketTest {
    TelnetServerSocket socket = null;

    @Before
    public void setUp() throws Exception {
        socket = new TelnetServerSocket();
    }

        //test one line
        //test multiple lines
        //test one and a partial line
    @Test
    public void testParseOneLine() {
        byte[] byteString = {'h','e','l','l','o',' ','t','h','e','r','e','\r','\n'};
        socket.readBuffer.put(byteString);
        assertEquals('h',socket.readBuffer.get(0));

        TelnetMessage msg = socket.parse(byteString.length);
        assertTrue(msg != null);
        assertTrue("hello there".equals(msg.text));
    }

    @Test
    public void testParseFromTwoLine() {
        byte[] byteString = 
            {
            'h','e','l','l','o',' ','t','h','e','r','e','\r','\n',
            'g','o','o','d','b','y','e','\r','\n'
            };
        socket.readBuffer.put(byteString);

        TelnetMessage msg = socket.extractPacket();
        assertTrue(msg != null);
        assertTrue("hello there".equals(msg.text));
        assertEquals('g',socket.readBuffer.get(0));     //make sure we are at the beginning of the next line
        msg = socket.extractPacket();
        assertTrue(msg != null);
//        System.out.println("msg=["+msg.text+"]");
        assertTrue("goodbye".equals(msg.text));
    }

    @Test
    public void testParsePartialLine() {
        byte[] byteString = 
            {
            'h','e','l','l','o'
            };
        socket.readBuffer.put(byteString);
        assertEquals(null,socket.extractPacket());
    }

    @Test
    public void testParseOneAndPartialLine() {
        byte[] byteString = 
            {
            'h','e','l','l','o',' ','t','h','e','r','e','\r','\n',
            'g','o','o','d','b','y'
            };
        socket.readBuffer.put(byteString);
        TelnetMessage msg = socket.extractPacket();
        assertTrue(msg != null);
        msg = socket.extractPacket();
        assertTrue(msg == null);
    }

    @Test
    public void testParsePartialAndAppended() {
        byte[] byteString = {'h','e','l','l','o',' '};
        byte[] moreBytes = {'t','h','e','r','e','\r','\n'};

        //make sure that it doesn't extract a packet yet
        socket.readBuffer.put(byteString);
        TelnetMessage msg = socket.extractPacket();
        assertTrue(msg == null);

        //add more data and make sure it parses it as a full packet
        socket.readBuffer.put(moreBytes);
        msg = socket.extractPacket();
        assertTrue(msg != null);
        assertTrue("hello there".equals(msg.text));
    }


    @Test
    public void testByteBuffer() {
        assertEquals(0,0);
        byte[] b = {1,2,3};
        socket.writeBuffer.put(b);
        assertEquals(3,socket.writeBuffer.position());
        assertEquals(1,socket.writeBuffer.get(0));
        assertEquals(2,socket.writeBuffer.get(1));
        assertEquals(3,socket.writeBuffer.get(2));
        
        socket.writeBuffer.rewind();
        assertEquals(1,socket.writeBuffer.get());
        assertEquals(2,socket.writeBuffer.get());
        assertEquals(3,socket.writeBuffer.get());
        
        assertEquals(socket.writeBuffer.hasArray(),true);
        
        ByteBuffer duplicate = socket.writeBuffer.duplicate();
        duplicate.rewind();
        socket.writeBuffer.rewind();
        duplicate.get();
        duplicate.get();
        assertEquals(1,socket.writeBuffer.get());
        assertEquals(3,duplicate.get());
    }
    
    @Test
    public void testShort() {
        short sh = (short)0xA78;
        int shint = 0xA78;
        assertEquals(sh,shint);
        socket.writeBuffer.putShort(sh);
        socket.writeBuffer.rewind();
        assertEquals(10,socket.writeBuffer.get());
        assertEquals(120,socket.writeBuffer.get());
    }
    
    @Test 
    public void testString() {
        socket.addPacketBuffer("ABC");
        socket.writeBuffer.rewind();
     
        byte b = socket.writeBuffer.get();
        b=socket.writeBuffer.get();
        b=socket.writeBuffer.get();

        socket.writeBuffer.rewind();
        assertEquals(65,socket.writeBuffer.get());
        assertEquals(66,socket.writeBuffer.get());
        assertEquals(67,socket.writeBuffer.get());
    }
    
    
}
