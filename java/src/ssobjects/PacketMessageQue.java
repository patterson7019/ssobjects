/*
 * Created on Feb 7, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import java.util.ArrayList;

public class PacketMessageQue
{
    protected boolean m_bSetEvent;
    /** List of PacketMessage objects. */
    protected ArrayList m_listMsgs;
    
    public PacketMessageQue()
    {
        this(false);
    }
    public PacketMessageQue(boolean setEvent)
    {
        m_listMsgs=new ArrayList();
        m_bSetEvent=setEvent;
    }
    
    public synchronized void add(PacketMessage msg)
    {
      m_listMsgs.add((Object)msg);
    }
    
    public synchronized PacketMessage get() 
    {
        if(m_listMsgs.size()>0)
        {
            PacketMessage msg=(PacketMessage)m_listMsgs.get(0);
            m_listMsgs.remove(msg);
            return msg;
        }
        return null;
    }
    
    public synchronized boolean isEmpty()
    {
        return m_listMsgs.size()==0;
    }
    
    public synchronized void purge()
    {
        while(m_listMsgs.size()>0)
            m_listMsgs.remove(0);
    }
}
