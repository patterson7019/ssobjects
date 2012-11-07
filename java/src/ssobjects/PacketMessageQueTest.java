/*
 * Created on Feb 7, 2006
 * Copyright (c) 2005, Ant Works Software
 */
package ssobjects;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import ssobjects.PacketBuffer;
import ssobjects.PacketMessage;
import ssobjects.PacketMessageQue;

public class PacketMessageQueTest extends TestCase
{
    public static Test suite()
    {
        return new TestSuite(PacketMessageQueTest.class);
    }
    
    public void testConstruction() 
    {
        PacketMessageQue que=new PacketMessageQue();
        assertTrue(que.m_listMsgs!=null);
        assertTrue(que.m_bSetEvent==false);
        assertTrue(que.isEmpty()==true);
        
        que=new PacketMessageQue(true);
        assertTrue(que.m_bSetEvent==true);
        assertTrue(que.m_listMsgs!=null);
    }
    
    public void testAddGetRemove()
    {
        PacketBuffer packet=new PacketBuffer(PacketBuffer.pcPing);
        PacketMessage msg=new PacketMessage(null,packet);
        PacketMessageQue que=new PacketMessageQue();
        que.add(msg);
        
        assertTrue(que.isEmpty()==false);
        assertTrue(que.m_listMsgs.size()==1);
        
        PacketMessage msg2=que.get();
        assertTrue(que.m_listMsgs.size()==0);
        assertTrue(que.isEmpty()==true);
        assertTrue(msg2==msg);
        assertTrue(msg2.m_socket==null);
        assertTrue(msg2.m_packet==packet);
        
        //check that purge works
        que.add(msg);
        que.add(msg);
        que.add(msg);
        que.add(msg);
        que.add(msg);
        assertTrue(que.m_listMsgs.size()==5);
        que.purge();
        assertTrue(que.isEmpty()==true);
    }

    public void testEmptyQue()
    {
        PacketMessageQue que=new PacketMessageQue();
        assertTrue(que.get()==null);
    }
}
