/*
 * Created on Feb 7, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

public class PacketMessage
{
    //thread data
    private static int m_uniqueID=0;
    private synchronized static int getUniqueID()
    {
        return ++m_uniqueID;
    }
    
    //regular data
    protected int m_id;
    protected BufferedSocket m_socket;
    protected PacketBuffer m_packet;
    
    public PacketMessage(BufferedSocket s,PacketBuffer p)
    {
        m_id=getUniqueID();
        m_socket=s;
        m_packet=p;
    }
    public BufferedSocket socket() {return m_socket;}
    public PacketBuffer packet() {return m_packet;}
}

