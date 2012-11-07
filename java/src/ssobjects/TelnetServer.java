package ssobjects;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;

/** 
 * Telnet server class for creating a text only telnet server. 
 * To run single threaded, construct and call the <code>run()</code> method.
 * <p>
 * Use the debug(), info(), warn(), error() methods for server logging. Timestamp
 * and log level is prepended to message. <i>Note that log4j will be added
 * in later. It is preferable to use the server logging so you can add additional
 * finctionality, like making special logs when you get exceptions from a script.</i>
 *
 * @author workerant@users.sourceforge.net
 */
public abstract class TelnetServer implements Runnable
{
  //TODO if we get an IOException on send methods we should probably close this socket
    
    static public final String CRLF = "\r\n";
    static public final long DEFAULT_IDLE = 1000;
    
    protected AtomicBoolean running = new AtomicBoolean();
    protected InetAddress hostAddress;
    protected int hostPort;
    protected Map<SocketChannel,TelnetServerSocket> serverSockMap = new HashMap<SocketChannel,TelnetServerSocket>();
    protected ServerSocketChannel serverChannel;
    protected Selector selector;
    protected List<TelnetMessage> messageQueue = new ArrayList<TelnetMessage>();
    protected long idleTime = DEFAULT_IDLE;  //idle in milliseconds
    protected DateFormat df = new SimpleDateFormat("yyyy/MM/dd kk:mm:ss");
    
    public TelnetServer(InetAddress host,int port) throws Exception {
        this(host,port,DEFAULT_IDLE);
    }

    public TelnetServer(InetAddress host,int port,long idle) throws Exception {
        this.hostAddress = host;
        this.hostPort = port;
        selector = initSelector();
        idleTime = idle;
    }
    
    public String timestamp() {return df.format(new Date());}
    
    public void error(String s,Exception e) {
        error(s);
        error(e);
    }
    
    public void error(Exception e) {
        e.printStackTrace();
    }
    
    public void error(String s) {
        System.out.println("[ERROR] "+timestamp()+" "+s);
    }
    
    public void warn(String s) {
        System.out.println("[WARN]  "+timestamp()+" "+s);
    }
    
    public void info(String s) {
        System.out.println("[INFO]  "+timestamp()+" "+s);
    }
    
    public void debug(String s) {
        System.out.println("[DEBUG] "+timestamp()+" "+s);
    }
    
    public abstract void processSingleMessage(TelnetMessage msg);
    public void connectionAccepted(TelnetServerSocket sock) {}
    public void connectionClosed(TelnetServerSocket sock) {}
    public void idle(long deltaTime) {}
    
    public void printlnAll(String s) throws IOException {
        printAll(s+CRLF);
    }
    
    public void printAll(String s) throws IOException {
        for(TelnetServerSocket sock : serverSockMap.values()) {
            try {
                sock.sendString(s);
            } catch(IOException e) {
                //TODO need to close the socket
            }
        }
    }
    
    public void printlnOthers(String s,TelnetServerSocket skipSock) throws IOException {
        printOthers(s+"CRLF",skipSock);
    }
    
    public void printOthers(String s,TelnetServerSocket skipSock) throws IOException {
        for(TelnetServerSocket sock : serverSockMap.values()) {
            if(sock != skipSock)
                sock.sendString(s);
        }
    }

    
    public boolean update(long elapsedTime) throws Exception {
        boolean needProcessing = false;
        Set<SelectionKey> keys = selector.keys();
        System.out.println("select: num keys="+keys.size());
        int readyKeyCount = selector.select(100);
        System.out.println("readyKeyCount="+readyKeyCount);
        
        if(readyKeyCount > 0)
        {
            Iterator<SelectionKey> selectedKeys = selector.selectedKeys().iterator();
            while(selectedKeys.hasNext())
            {
                SelectionKey key = selectedKeys.next();
                selectedKeys.remove();
                if(!key.isValid())
                    continue;
                
                if(key.isAcceptable())
                    accept(key);
                else if(key.isReadable()) {
                    needProcessing |= read(key);
                }
            }
        }
        return needProcessing;
    }

    protected TelnetMessage getMessage() {
        TelnetMessage msg = null;
        if(messageQueue.size() > 0) {
            msg = messageQueue.get(0);
            messageQueue.remove(0);
        }
        return msg;
    }

    public void stopServer() {
        running.set(false);
    }

    public void run() 
    {
        running.set(true);
        long now = System.currentTimeMillis();
        long waitUntil = now+idleTime;
        long timeout=idleTime;
        long lastIdleCall=now;
        long idleCalledAt = now;
        while(running.get())
        {
            try
            {
                Thread.sleep(1000);
                boolean needProcessing = false;
                Set<SelectionKey> keys = selector.keys();
                //System.out.println("select: num keys="+keys.size());
                int readyKeyCount = selector.select(timeout);
//                int readyKeyCount = selector.select();
                //System.out.println("readyKeyCount="+readyKeyCount);
                if(readyKeyCount > 0)
                {
                    Iterator<SelectionKey> selectedKeys = selector.selectedKeys().iterator();
                    while(selectedKeys.hasNext())
                    {
                        SelectionKey key = selectedKeys.next();
                        selectedKeys.remove();
                        if(!key.isValid())
                            continue;

                        if(key.isAcceptable())
                            accept(key);
                        else if(key.isReadable()) {
                            needProcessing |= read(key);
                        }
                    }
                    if(needProcessing) {
                        processMessages();
                    }
                }
                now = System.currentTimeMillis();
                if(now >= waitUntil) {
                    timeout = idleTime;     //max idle time
                    idleCalledAt = now;
                    idle(now-lastIdleCall);
                    lastIdleCall = now;
                    
                    //take into account how long the idle call took, and subtract that from out wait time
                    now = System.currentTimeMillis();
                    timeout -= now-idleCalledAt;
                    if(timeout < 1)
                        timeout=1;  //minimum wait is 1 ms
                    
                    waitUntil = now+timeout;
                }
                else {
                    timeout = waitUntil-now;  //still have some idle time left
                }
            }
            catch(Exception e) 
            {
                e.printStackTrace();
                running.set(false);
            }
        }
        //System.out.println("Server ended");
    }

    protected void processMessages() {
        while(messageQueue.size() > 0) {
            TelnetMessage msg = messageQueue.get(0);
//            try {
                processSingleMessage(msg);
//            }
//            catch(IOException e) {
//                SocketChannel socketChannel = msg.sock.sock;
//                System.out.println("IOException on socket "+socketChannel);
//            }
            messageQueue.remove(0);
        }
    }
    
    protected void accept(SelectionKey key) throws IOException  
    {
        //System.out.println("accepting key");
        // For an accept to be pending the channel must be a server socket channel.
        ServerSocketChannel serverSocketChannel = (ServerSocketChannel) key.channel();

        // Accept the connection and make it non-blocking
        SocketChannel socketChannel = serverSocketChannel.accept();
        //Socket socket = socketChannel.socket();
        socketChannel.configureBlocking(false);

        // Register the new SocketChannel with our Selector, indicating
        // we'd like to be notified when there's data waiting to be read
        socketChannel.register(this.selector, SelectionKey.OP_READ);
        
        TelnetServerSocket serverSock = new TelnetServerSocket(socketChannel);
        serverSockMap.put(socketChannel,serverSock);
        connectionAccepted(serverSock);
    }
    
    protected boolean read(SelectionKey key) throws IOException 
    {
        //System.out.println("reading key");
        SocketChannel socketChannel = (SocketChannel) key.channel();
        TelnetServerSocket serverSock = serverSockMap.get(socketChannel);
        if(null != serverSock) {
            int numRead = serverSock.readData();
            //System.out.println("read: ["+numRead+"] bytes");

            if (numRead == -1) {
                // Remote entity shut the socket down cleanly. Do the
                // same from our end and cancel the channel.
                key.cancel();
                socketChannel.close();
                postCloseMsg(serverSock);
            }
            else {

                //extract as many packets as we can from the socket
                TelnetMessage msg = null;
                while((msg = serverSock.extractPacket()) != null) 
                    messageQueue.add(msg);

                //System.out.println("read: Done processing");
            }
        }
        return messageQueue.size() > 0;
    }
    
    public void postCloseMsg(TelnetServerSocket sock) {
        connectionClosed(sock);
    }
    
    public void close(TelnetServerSocket sock) {
//        try {
//            sock.key.channel().close();
//        } catch(IOException e) {}   //safely ignore since we are closing it anyway
//        sock.key.cancel();
        sock.close();
        serverSockMap.remove(sock.sock);
    }
    
    protected Selector initSelector() throws IOException
    {
        Selector socketSelector = SelectorProvider.provider().openSelector();
        serverChannel = ServerSocketChannel.open();
        serverChannel.configureBlocking(false);

        InetSocketAddress isa = new InetSocketAddress(hostAddress, hostPort);
        serverChannel.socket().bind(isa);
        serverChannel.register(socketSelector, SelectionKey.OP_ACCEPT);
        
        return socketSelector;
    }
}
