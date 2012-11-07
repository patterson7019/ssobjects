package example;

import java.io.IOException;
import java.net.InetAddress;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import ssobjects.TelnetMessage;
import ssobjects.TelnetServer;
import ssobjects.TelnetServerSocket;

public class SimpleTelnetServer extends TelnetServer
{
    final static int IDLE_TIMEOUT = 60000;      //milliseconds                                                                                
    protected Map<TelnetServerSocket,User> userMap = new HashMap<TelnetServerSocket,User>();
    protected long lastIdle = 0;
    
    public SimpleTelnetServer(InetAddress host,int port,long idle) throws Exception {
        super(host,port,idle);
        lastIdle=System.currentTimeMillis();
    }

    @Override
    public void connectionAccepted(TelnetServerSocket sock) {
        System.out.println("New connection from ["+sock.getHostAddress()+"]");
        User u=new User(sock);
        userMap.put(sock,u);
        try {
            sock.sendString("Login:"+CRLF); //sending crlf so junit test will work, as it uses readline (just simplifying things here)
        } catch(IOException e) {
            close(u);
        }
    }
    
    @Override
    public void connectionClosed(TelnetServerSocket sock) {
        User u = userMap.get(sock);
        userMap.remove(sock);
        String name = u==null?"":"-"+u.username;
        System.out.println("Closed connection from ["+sock.getHostAddress()+"]"+name);
    }

    @Override
    public void processSingleMessage(TelnetMessage msg) {
        try {
            User u = userMap.get(msg.sock);
            if(null != u) {
                u.updateIdle();
                processUser(u,msg.text);
            }
        } catch(IOException e) {
            System.out.println("ERROR got exception when sending data");
        }
    }

    @Override
    public void idle(long deltaTime) {
        long now = System.currentTimeMillis();
        lastIdle = now;
        Iterator<User> it = userMap.values().iterator();
        while(it.hasNext()) {
            User u = it.next();
            if(u.idleTime() > IDLE_TIMEOUT) {
                System.out.println("Closing connection for "+u.username);
                try {
                    u.sock.println("Idle time exceeded, closing connection");
                } catch(IOException e) {
                    //can safely igore exception since we are closing his connection anyway
                }
                u.sock.close();
                it.remove();
            }
        }
    }

    public void close(User u) {
        userMap.remove(u.sock);
        u.sock.close();        
    }
    
    protected void processUser(User u,String s) throws IOException {
        if(u.state == UserStateEnum.LOGIN) {
            u.username = s;
            u.state = UserStateEnum.ACTIVE;
            printlnAll("Welcome "+u.username);
        }
        else if(u.state == UserStateEnum.ACTIVE) {
            printlnAll(u.username+":"+s);
        }
    }
    
    public static void main(String[] args) throws Exception 
    {
        int hostPort = 4002;
        SimpleTelnetServer server = new SimpleTelnetServer(null,hostPort,5000);
        System.out.println("Running on port "+hostPort+"...");
        server.run();   //running as single thread
    }
}
