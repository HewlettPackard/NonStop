/* 
(C) Copyright 2021 Hewlett Packard Enterprise Development LP

 This software is provided subject to the terms of your
 existing license agreement with Hewlett Packard Enterprise
 Development LP or its reseller, integrator,
 distributor or other authorized sub-licensor for the
 use of Licensed Products or Software supplied by
 Hewlett Packard Enterprise Development LP .
 
 This is a Java Server Program which is created for NSMF
 This is used for serverclass operations using NSMF
 How run this - <JAVA_HOME>/bin/java -Dserver.socket.port=5000 com.hpe.nsk.nsdevops.server.Server
*/

package com.hpe.nsk.nsdevops.server;

import java.net.*;
import java.io.*;

public class Server {
  public void run() {
        try {
                int serverPort = Integer.getInteger("server.socket.port");
                ServerSocket serverSocket = new ServerSocket(serverPort);
                while(true) {
                        System.out.println("Waiting for Client on port " + serverSocket.getLocalPort() + "...");
                        Socket server = serverSocket.accept();
                        System.out.println("Connected to " + server.getRemoteSocketAddress());
                        PrintWriter toClient =
                                new PrintWriter(server.getOutputStream(),true);
                        BufferedReader fromClient =
                                new BufferedReader(
                                                new InputStreamReader(server.getInputStream()));
                        String line = fromClient.readLine();
                        System.out.println("Server received: " + line);
                        toClient.println("Thank you for connecting to " + server.getLocalSocketAddress() + " using NSMF" +  "\nGoodbye!");
                }
        }
        catch(UnknownHostException ex) {
                ex.printStackTrace();
        }
        catch(IOException e){
                e.printStackTrace();
        }
  }

  public static void main(String[] args) {
                Server srv = new Server();
                srv.run();
  }
}