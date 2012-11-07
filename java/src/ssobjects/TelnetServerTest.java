package ssobjects;


import static org.junit.Assert.*;

import java.io.*;
import java.net.*;

import org.junit.*;

public class TelnetServerTest
{
    Socket sock;
    PrintWriter out;
    BufferedReader in;
    
//    @Before
//    public void setUp() throws Exception {
//        sock = new Socket("localhost",4002);
//        out = new PrintWriter(sock.getOutputStream());
//        in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
//    }
    
//    @Test
//    public void testSendRecv() throws Exception {
//        String recv = in.readLine();
//        assertEquals("Login:",recv);
//        final String TEST_STRING = "testuser";
//        out.print(TEST_STRING+"\r\n");
//        out.flush();
//        recv = in.readLine();
//        assertEquals("Welcome "+TEST_STRING,recv);
//    }

    @Test
    public void testServerInThread() throws Exception {
        class TheTelnetServer extends TelnetServer {
            public TheTelnetServer(InetAddress host,int port) throws Exception {
                super(host,port,1000);
            }

            @Override
            public void processSingleMessage(TelnetMessage msg) {
                try {
                    printlnAll("got:"+msg.text+CRLF);
                } catch(IOException e) {
                    System.err.println("ERROR got exception when sending data");
                }
            }
        };
        TheTelnetServer server = new TheTelnetServer(null,4003); 
        Thread t = new Thread(server);
        t.start();

        sock = new Socket("localhost",4003);
        out = new PrintWriter(sock.getOutputStream());
        in = new BufferedReader(new InputStreamReader(sock.getInputStream()));

        out.print("bleh\r\n");
        out.flush();
        String buff = in.readLine();
        assertEquals("got:bleh",buff);
        server.stopServer();
    }

//    @Test     not ready for prime time yet
    public void testTelnetServerPolled() throws Exception {
        class PolledServer extends TelnetServer {
            public PolledServer(InetAddress host,int port) throws Exception {
                super(host,port,1000);
            }
            @Override
            public void connectionAccepted(TelnetServerSocket sock) {
                debug("connection accepted");
            }
            @Override
            public void connectionClosed(TelnetServerSocket sock) {
                debug("Connection closed");
            }
            @Override
            public void processSingleMessage(TelnetMessage msg) {
                debug("Got message");
                try {
                    printlnAll("got:"+msg.text+CRLF);
                    System.err.println("Should not have gotten here.");
                } catch(IOException e) {
                    System.err.println("ERROR got exception when sending data");
                }
            }
        };
        PolledServer server = new PolledServer(null,4004);
        sock = new Socket("localhost",4003);
        out = new PrintWriter(sock.getOutputStream());
        in = new BufferedReader(new InputStreamReader(sock.getInputStream()));

        out.print("bleh\r\n");
        out.flush();
        boolean hasMessages = false;
        for(int i=0; i<10 && hasMessages==false; i++) {
            Thread.sleep(100);
            System.out.println("Checking for data...");
            hasMessages = server.update(0);
        }
        assertEquals(true,hasMessages);
        
    }
}
