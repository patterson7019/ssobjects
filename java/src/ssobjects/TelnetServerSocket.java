package ssobjects;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

/** 
 * Socket used by telnet server's. Contains buffer's and commands for sending
 * and recieving data from a non blocked socket. Use <code>print*</code> commands
 * to send data to the socket. 
 * <p>
 * <b>Note</b> Writting currently is blocked, but not a huge priority to make
 * non blocked since most of the time it will go first try.
 * 
 * @author Orion7019
 */
public class TelnetServerSocket {
    protected static final int BUFFER_SIZE=3000;

    static public final String CRLF = "\r\n";
    public static final int CR = 13;
    public static final int LF = 10;
    public static final int NUL = 0;
    
    protected ByteBuffer writeBuffer;
    protected ByteBuffer readBuffer;
    protected SocketChannel sock;
    protected byte[] parseBuffer;

    public TelnetServerSocket() {
        writeBuffer = ByteBuffer.allocate(BUFFER_SIZE);
        readBuffer = ByteBuffer.allocate(BUFFER_SIZE);
        parseBuffer = new byte[BUFFER_SIZE];
    }
    
    public TelnetServerSocket(SocketChannel sock) {
        this();
        this.sock = sock;
    }
    
    public void close() {
        try {
            sock.close();
        } catch(IOException e) {
            //can safely ignore exception
        }
    }
    
    public Socket socket() {return sock.socket();}
    public InetAddress getSockAddr() {return socket().getInetAddress();}
    public String getHostAddress() {return getSockAddr().getHostAddress();}
    
    public int readData() throws IOException {
        return sock.read(readBuffer);
    }
    
    public TelnetMessage extractPacket() {
        TelnetMessage msg = null;
        if(readBuffer.hasArray()) {
            int head = readBuffer.arrayOffset();
            int tail = readBuffer.position();
            msg = parse(tail-head);
        }
        return msg;
    }

    public TelnetMessage parse(int numBytes) {
        //find LF
        int lfpos = -1;
        int oldpos = readBuffer.position();
        //System.out.println("buffpos="+buffPos);
        readBuffer.rewind();
        for(int i=0; i<numBytes && lfpos == -1; i++) {
            parseBuffer[i] = readBuffer.get();
            if(parseBuffer[i] == LF) {
                String text = new String(parseBuffer,0,i-1);
                TelnetMessage msg = new TelnetMessage(this,text);
                int newpos = oldpos-readBuffer.position();
                //System.out.println("newpos=["+newpos+"]");
                readBuffer.compact();   //remove what we just parsed out
                readBuffer.position(newpos);
                return msg;
            }
        }
        //no LF found
        readBuffer.position(oldpos);
        return null;
    }
    
    public int print(final String s) throws IOException {
        return sendString(s);
    }
    
    public int println(final String s) throws IOException {
        return sendString(s+CRLF);
    }

    /** 
     * Send the string <code>s</code> as a stream of bytes. 
     * 
     * @param s String to send. 
     * @return The number of bytes sent. 
     * @throws IOException
     */
    public int sendString(final String s) throws IOException {
        addPacketBuffer(s);
        return sendBuffer();
    }
    
    /** 
     * We are allocating a write buffer each time since it seems to try to send
     * all the bytes in the buffer regardless of the amount of data we have put in. 
     * Perhaps there is a better way we can manage the outgoing data.
     * 
     * @param s String to send, and is sent as a stream of bytes.  
     */
    public void addPacketBuffer(final String s) {
        writeBuffer = ByteBuffer.allocate(s.length());
        for(int i=0; i<s.length(); i++) 
        {
            byte ch = (byte)s.charAt(i);
            writeBuffer.put(ch);
        }
    }

    /** 
     * Sends the data in the write buffer. While a write opperation rarely blocks,
     * we will want to use a non blocked write at some point. Keep in mind this simplifies
     * the server logic as we don't have to keep checking for writable sockets in the key
     * selector and we would have to check every time we try to write. 
     *  
     * The number of bytes written will always be the size of the write buffer.
     * 
     * @return Number of bytes written.
     * @throws IOException If there is a write failure.
     */
    public int sendBuffer() throws IOException {
        int numBytes = writeBuffer.position();
        //System.out.println("Sending ["+numBytes+"] bytes");
        
        writeBuffer.rewind();
        int bytesSent = sock.write(writeBuffer);
        //System.out.println("Sent ["+bytesSent+"] bytes");
        writeBuffer = null;
        return numBytes;
    }
}
