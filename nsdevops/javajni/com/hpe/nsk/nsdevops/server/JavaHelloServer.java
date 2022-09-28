package com.hpe.nsk.nsdevops.server;
import java.net.*;
import java.io.*;

public class JavaHelloServer extends Thread {
   private ServerSocket serverSocket;
   private native void sayHelloNative(String input);

	static {
		try {
			String dataModel = System.getProperty("sun.arch.data.model", "?");
			
			System.loadLibrary("javajnihelloserver");
		
		} catch (Exception e) {
			e.printStackTrace(System.err);
		}
	}

/** Constructor */
   public JavaHelloServer(String host, int port) throws IOException {
      serverSocket = new ServerSocket(port, 100, InetAddress.getByName(host));
      //serverSocket.setSoTimeout(10000);
   }

/** run Method 
    Waits for connections, accepts, reads the input and sends the response*/
    public void run() {
      while(true) {
         try {
            
            Socket server = serverSocket.accept();
            
           
            DataInputStream in = new DataInputStream(server.getInputStream());
            DataOutputStream out = new DataOutputStream(server.getOutputStream());
            out.writeUTF(sayHello(in.readUTF()));
            server.close();
            
         } catch (SocketTimeoutException s) {
            System.out.println("Socket timed out!");
            break;
         } catch (IOException e) {
            e.printStackTrace();
            break;
         }
      }
   }

/** sayHello method 
    Takes an input string and returns the string prepending Hello */
   public String sayHello(String input) {
      // TODO Auto-generated method stub
		sayHelloNative(input);
       return "Hello  " + input;
   }
   public static void main(String [] args) {
      String host = args[0];
      int port = Integer.parseInt(args[1]);
      try {
         Thread t = new JavaHelloServer(host, port);
         t.start();
      } catch (IOException e) {
         e.printStackTrace();
      }
   }
}