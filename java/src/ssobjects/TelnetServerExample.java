package ssobjects;

import java.io.IOException;
import java.net.InetAddress;

import ssobjects.TelnetMessage;
import ssobjects.TelnetServer;
import ssobjects.TelnetServerSocket;

public class TelnetServerExample extends TelnetServer
{
    char ESCAPE = 27;
    long lastIdle = 0;
    public TelnetServerExample(InetAddress host,int port,long idle) throws Exception {
        super(host,port,idle);
    }

    @Override
    public void connectionAccepted(TelnetServerSocket sock) {
        System.out.println("New connection from ["+sock.getHostAddress()+"]");
        try {
            sock.print(ESCAPE+"[2JScreen cleared? ");
        } catch(Exception e) {
            e.printStackTrace();
        }
    }
    
    @Override
    public void connectionClosed(TelnetServerSocket sock) {
        System.out.println("Closed connection from ["+sock.getHostAddress()+"]");
    }

    @Override
    public void processSingleMessage(TelnetMessage msg) {
        try {
            printlnAll("got:"+msg.text+CRLF);
        } catch(IOException e) {
            System.out.println("ERROR got exception when sending data");
        }
    }
    
    @Override 
    public void idle(long deltaTime) {
        long now = System.currentTimeMillis();
        System.out.println("idle deltaTime/currentTime ["+deltaTime+"/"+(now-lastIdle)+"]");
        lastIdle = now;
        try{Thread.sleep(1000);} catch(InterruptedException e){}
    }

    public static void main(String[] args) throws Exception 
    {
        int hostPort = 4002;
        TelnetServerExample server = new TelnetServerExample(null,hostPort,5000);
        System.out.println("Running on port "+hostPort+"...");
        System.out.println("You can run the run-telnetservertest script");
        server.run();
    }

}
