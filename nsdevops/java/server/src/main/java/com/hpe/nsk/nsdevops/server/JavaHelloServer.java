/*
 (C) Copyright 2020 Hewlett Packard Enterprise Development LP

 This software is provided subject to the terms of your
 existing license agreement with Hewlett Packard Enterprise
 Development LP or its reseller, integrator,
 distributor or other authorized sub-licensor for the
 use of Licensed Products or Software supplied by
 Hewlett Packard Enterprise Development LP .
*/

package com.hpe.nsk.nsdevops.server;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.ServerSocket;
import java.net.SocketTimeoutException;

/**
  *JavaHelloServer is a server application to demonstrate CI/CD on Nonstop.
  */
public class JavaHelloServer extends Thread {
 /** ServerSocket. */
 private ServerSocket serverSocket;
 /** Backlog for requests. */
 public static final int SOCKETBACKLOG = 100;
/** Constructor.
  * Constructor for the JavaHelloServer.
  * @param host
  *        hostname or ip address
  * @param port
  *     port number
  * @throws IOException if applicaiton is unable to bind to host and port
  */
   public JavaHelloServer(final String  host, final int port)
   throws IOException {
      serverSocket = new ServerSocket(port, SOCKETBACKLOG,
                  InetAddress.getByName(host));
   }

/** run Method.
  * Waits for connections, accepts, reads the input and sends the response
  */
    public final void run() {
      while (true) {
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

/** sayHello method.
  * Takes an input string and returns the string prepending Hello
  * @param input
  *        String input sent by the client application
  * @return String containg the greeting message
  */
   public final String sayHello(final String input) {
       return "Hello  " + input;
   }
   /** Main Method.
     * @param args
     *      hostname and port number as inputs
     */

   public static void main(final String[] args) {
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
