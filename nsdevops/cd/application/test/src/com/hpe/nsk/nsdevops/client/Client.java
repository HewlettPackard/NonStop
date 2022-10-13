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
 How run this - <JAVA_HOME>/bin/java -Dserver.socket.port=5000 com.hpe.nsk.nsdevops.client.Client
*/

package com.hpe.nsk.nsdevops.client;
import java.io.*;
import java.net.*;

public class Client {
  public void run() {
        try {
                int serverPort = Integer.getInteger("server.socket.port");
                InetAddress host = InetAddress.getByName("localhost");
                System.out.println("Connecting to Server on port " + serverPort);
                Socket socket = new Socket(host,serverPort);
                System.out.println("Connected to " + socket.getRemoteSocketAddress());
                PrintWriter toServer =
                        new PrintWriter(socket.getOutputStream(),true);
                BufferedReader fromServer =
                        new BufferedReader(
                                        new InputStreamReader(socket.getInputStream()));
                toServer.println("Hello NSMF from " + socket.getLocalSocketAddress());
                String line = fromServer.readLine();
                System.out.println("Client received- " + line + " from Server");
                toServer.close();
                fromServer.close();
        }
        catch(UnknownHostException ex) {
                ex.printStackTrace();
        }
        catch(IOException e){
                e.printStackTrace();
        }
  }

  public static void main(String[] args) {
                Client client = new Client();
                client.run();
  }
}