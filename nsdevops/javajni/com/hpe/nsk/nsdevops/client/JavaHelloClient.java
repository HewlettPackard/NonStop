package com.hpe.nsk.nsdevops.client;

// File Name JavaHelloClient.java
import java.net.*;
import java.io.*;

public class JavaHelloClient {

   public static void main(String [] args) {
      String serverName = args[0];
      int port = Integer.parseInt(args[1]);
      try {
         
         Socket client = new Socket(serverName, port);
         OutputStream outToServer = client.getOutputStream();
         DataOutputStream out = new DataOutputStream(outToServer);
         
         out.writeUTF("Dear");
         InputStream inFromServer = client.getInputStream();
         DataInputStream in = new DataInputStream(inFromServer);
         
         System.out.println(in.readUTF());
         client.close();
      } catch (IOException e) {
         e.printStackTrace();
      }
   }
}
