/*
 * Created on Feb 5, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import java.io.IOException;
import java.net.Socket;

public class ClientConnector
{
    protected BufferedSocket m_bsocket;
    protected String m_host;
    protected int m_port;
    
    public ClientConnector() 
    {
        m_bsocket=null;
    }
    
    public ClientConnector(String host,int port)
    {
        m_host=host;
        m_port=port;
        m_bsocket=null;
    }
    
    public BufferedSocket getBufferedSocket()
    {
        return m_bsocket;
    }
    

    public String geHost()
    {
        return m_host;
    }

    public void setHost(String host)
    {
        m_host=host;
    }

    public int getPort()
    {
        return m_port;
    }

    public void setPort(int port)
    {
        m_port=port;
    }

    public void connect() throws IOException 
    {
        Socket s=new Socket(m_host,m_port);
        m_bsocket=new BufferedSocket(s);
    }

    public void connect(String host,int port) throws IOException
    {
        m_host=host;
        m_port=port;
        connect();
    }

    public void reconnect() throws IOException 
    {
        close();
        connect();
    }

    public void close() throws IOException
    {
        if(m_bsocket!=null)
            m_bsocket.close();
        m_bsocket=null;
    }
    
    public void sendPacket(PacketBuffer packet) throws IOException 
    {
        m_bsocket.sendPacket(packet);
    }
    
    public PacketBuffer recvPacket() throws IOException
    {
        return m_bsocket.recvPacket();
    }
    
    public static void main(String[] args) throws Exception
    {
        ClientConnector connector=new ClientConnector("fireant",9999);
        connector.connect();
        connector.sendPacket(new PacketBuffer(PacketBuffer.pcPing));
        PacketBuffer pong=connector.recvPacket();
        if(pong.command()==PacketBuffer.pcPong)
        System.out.println("Got pong");
        else
            System.out.println("Didn't get pong, got "+pong.command()+" instead.");
        connector.close();
    }
}

