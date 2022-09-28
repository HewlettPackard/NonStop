/*
 (C) Copyright 2020 Hewlett Packard Enterprise Development LP

 This software is provided subject to the terms of your
 existing license agreement with Hewlett Packard Enterprise
 Development LP or its reseller, integrator,
 distributor or other authorized sub-licensor for the
 use of Licensed Products or Software supplied by
 Hewlett Packard Enterprise Development LP .
*/

package com.hpe.nsk.nsdevops.client;

// File Name JavaHelloClient.java
import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.Socket;
/**.
  *JavaHelloClient is a sample application to demonstrate CI/CD on Nonstop
  */
public final class JavaHelloClient {

/**
  * Constructor.
*/
 private JavaHelloClient() {
  }
/**
  *Main method.
  *@param args - arguments to main method
  */
   public static void main(final String[] args) {
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


