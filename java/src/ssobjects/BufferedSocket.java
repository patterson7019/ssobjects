/*
 * Created on Feb 5, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class BufferedSocket
{
    protected static final int BUFFER_SIZE=3000;
    protected Socket m_socket;
    protected OutputStream m_out;
    protected InputStream m_in;
    protected byte[] m_buffer;
    protected int m_bytesInBuffer;
    
    //used only for junit
    protected BufferedSocket()
    {
        m_buffer=new byte[BUFFER_SIZE]; //about 3 packets
        m_bytesInBuffer=0;
    }

    public BufferedSocket(Socket s) throws IOException
    {
        m_socket=s;
        m_buffer=new byte[BUFFER_SIZE]; //about 3 packets
        m_bytesInBuffer=0;
        m_out=s.getOutputStream();
        m_in=s.getInputStream();
    }

    /** Closes connection to the socket. Also closes the output and input streams. 
     * Flushes data data in the output stream before it closes it.
     * @throws IOException if an I/O error occurs.
     */  
    public void close() throws IOException 
    {
        m_out.flush();
        m_out.close();
        m_in.close();
        m_socket.close();
    }
    
    /** 
     * Sends the header and data in packet. All data is garented to be sent.
     * 
     * @param packet Packet you are sending. 
     * @throws IOException if an I/O error occurs. 
     */  
    public void sendPacket(PacketBuffer packet) throws IOException
    {
        m_out.write(packet.buffer(),0,packet.packetSize());
    }
    
    /** Reads a complete packet from the socket. The packet will contain a valid header
     * and data if pressent. An IOException will be generated if the size of the returned
     * packet tries to indicate a size greater then the maximum packet size of 
     * PacketBuffer.MAX_PACKET_SIZE. If this happens it is likely that the data sent is
     * in error, or someone is trying to hack you.
     * 
     * @return Complete PacketBuffer.
     * @throws IOException if an I/O error occurs.
     */
    public PacketBuffer recvPacket() throws IOException
    {
        return readFullPacket();
    }
    
    protected PacketBuffer readFullPacket() throws IOException
    {
        PacketBuffer packet=null;
        int bytesIn=0;
        do
        {
            if(packet==null)
            {
                bytesIn=m_in.read(m_buffer,m_bytesInBuffer,BUFFER_SIZE-m_bytesInBuffer);
                if(bytesIn==-1)
                    throw new IOException("connection closed");
                m_bytesInBuffer+=bytesIn;
                packet=parseForPacket();
            }
        } while(packet==null);
        
        return packet;
    }
    
    protected PacketBuffer parseForPacket() throws IOException 
    {
        PacketBuffer packet=null;
        if(m_bytesInBuffer<PacketBuffer.headerSize())
            return null;    //not enough data in buffer to contruct a packet header
        int size=PacketBuffer.extractInt(m_buffer,0);
        if(size>PacketBuffer.MAX_PACKET_SIZE)
            throw new IOException("Packet size field too large.");
        if(m_bytesInBuffer<size+PacketBuffer.headerSize())
            return null;    //not enough data in buffer to make packet yet

        packet=new PacketBuffer(m_buffer);  //create the packet
        if(packet.header().cookie()!=PacketBuffer.COOKIE)
            throw new IOException("Invalid cookie");
            
        //move the data in the buffer down
        final int offset=size+PacketBuffer.headerSize();
        m_bytesInBuffer-=offset;
        for(int i=0; i<m_bytesInBuffer; i++)
            m_buffer[i]=m_buffer[offset+i];
        return packet;
    }
}
