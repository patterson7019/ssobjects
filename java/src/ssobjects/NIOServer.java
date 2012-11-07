package ssobjects;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;
import java.util.Iterator;
import java.util.concurrent.atomic.AtomicBoolean;

//code snippets from http://rox-xmlrpc.sourceforge.net/niotut/#The%20server
public class NIOServer implements Runnable
{
    protected static final int BUFFER_SIZE=3000;

    protected AtomicBoolean running = new AtomicBoolean();
    protected InetAddress hostAddress;
    protected int hostPort;
    
    protected ServerSocketChannel serverChannel;
    protected Selector selector;
    protected ByteBuffer readBuffer = ByteBuffer.allocate(BUFFER_SIZE);
    protected ByteBuffer writeBuffer = ByteBuffer.allocate(BUFFER_SIZE);
    protected int numBytesOut = 0;
    
    public NIOServer(InetAddress host,int port) throws Exception 
    {
        this.hostAddress = host;
        this.hostPort = port;
        selector = initSelector();
    }
    
    public void run() 
    {
        System.out.println("Server starting");
        running.set(true);
        while(running.get())
        {
            try
            {
                long timeout=30000;
                System.out.println("Waiting for connection on port "+hostPort+"...");
                int readyKeyCount = selector.select(timeout);
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
                        if(key.isReadable())
                            read(key);
//                        if(key.isWritable())
//                            write(key);
                    }
                }
                else
                {
                    System.out.println("select timeout");
                }
            }
            catch(Exception e) 
            {
                e.printStackTrace();
                running.set(false);
            }
        }
        running.set(false);
        System.out.println("Server ended");
    }
    
    protected void accept(SelectionKey key) throws IOException  
    {
     // For an accept to be pending the channel must be a server socket channel.
        ServerSocketChannel serverSocketChannel = (ServerSocketChannel) key.channel();

        // Accept the connection and make it non-blocking
        SocketChannel socketChannel = serverSocketChannel.accept();
        //Socket socket = socketChannel.socket();
        socketChannel.configureBlocking(false);

        // Register the new SocketChannel with our Selector, indicating
        // we'd like to be notified when there's data waiting to be read
        socketChannel.register(this.selector, SelectionKey.OP_READ);
    }
    
    protected void read(SelectionKey key) throws IOException 
    {
        SocketChannel socketChannel = (SocketChannel) key.channel();

        readBuffer.clear();
        
        // Attempt to read off the channel
        int numRead;
        try {
          numRead = socketChannel.read(this.readBuffer);
        } catch (IOException e) {
          // The remote forcibly closed the connection, cancel
          // the selection key and close the channel.
          key.cancel();
          socketChannel.close();
          return;
        }

        System.out.println("Read ["+numRead+"] bytes");

        if (numRead == -1) {
          // Remote entity shut the socket down cleanly. Do the
          // same from our end and cancel the channel.
          key.channel().close();
          key.cancel();
          return;
        }
        processData(socketChannel,this.readBuffer.array(),numRead);
        System.out.println("Done processing");
    }
    
    protected void write(SelectionKey key) throws IOException 
    {
        int numBytesOut = writeBuffer.position();
//        writeBuffer.rewind();
        SocketChannel sock = (SocketChannel)key.channel();
        System.out.println("Writing [remaining/position]=["+writeBuffer.remaining()+"/"+writeBuffer.position()+"] bytes of data");
        int bytesOut = 0;
//        bytesOut = sock.write(writeBuffer);
        sock.socket().getOutputStream().write(writeBuffer.array(),0,numBytesOut);
        System.out.println("Wrote ["+bytesOut+"] bytes. [remaining/position]=["+writeBuffer.remaining()+"/"+writeBuffer.position()+"] bytes of data");
        key.interestOps(SelectionKey.OP_READ);
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
    
    protected void processData(SocketChannel sock,byte[] data,int numRead) throws IOException 
    {
        StringBuffer buf = new StringBuffer();
        System.out.println("Server read:");
        char c=0;
        for(int i=0; i<numRead; i++)
        {
            c=(char)(data[i]&0xFF);
            if(c != 0)
                buf.append(c);
            System.out.println("byte "+data[i]);
        }
        System.out.println("String ["+buf.toString()+"]");
        writeBuffer.clear();
        writeBuffer.put(data,0,numRead);
        numBytesOut = numRead;
//        sock.register(this.selector, SelectionKey.OP_WRITE);
       
        
        writeBuffer.rewind();
//        SocketChannel sock = (SocketChannel)key.channel();
        System.out.println("Writing [remaining/position]=["+writeBuffer.remaining()+"/"+writeBuffer.position()+"] bytes of data");
        int bytesOut = sock.write(writeBuffer);
        System.out.println("Wrote ["+bytesOut+"] bytes. [remaining/position]=["+writeBuffer.remaining()+"/"+writeBuffer.position()+"] bytes of data");
       
//        sock.socket().getOutputStream().write(data,0,numRead);
//        sock.socket().getOutputStream().flush();
    }
    
    public static void main(String[] args) throws Exception 
    {
        NIOServer server = new NIOServer(null,4002);
        server.run();
    }
}
