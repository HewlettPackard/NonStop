# NSDevOps Java Starter Kit
This package is designed to demonstrate the Continuous Integration of a  Purely Platform Agnostic Application profile of NonStop DevOps through a javaquickstart application. 
The Pre-requisite setup, tooling, gating criteria and artifacts like the JenkinsFile can be used to automate the Continuous Integration Phase of any Java Project.  This package acts as a starter kit for users. 

## Contents 
The package contains a java net client application, a server application JenkinsFile that can be used to automate the Continuous Integration Workflow and this JavaStarterKitUsageInstruction.docx file that provides instructions to use the javaquickstart application. 

###Client Application
The Client application (JavaHelloClient) is a sample Java Client application used to demonstrate Continuous Integration. The application that uses the java.net package. 
The application takes two inputs <IPAddress> of the server to connect to and the <PortNumber> on which the server is listening at. 
The client can be run on-platform (NonStop) or off-platform(windows). 	

###Server Application
The Server application (JavaHelloServer) is a sample Java Server application to demonstrate Continuous Integration. The Server application waits for requests from client application. Once a connection request is made by the client, the server connects to the client and exchanges greeting message.  The Server application takes two inputs, namely the <HostName> and the <PortNumber> on which the server will listen. The server can be run on-platform (NonStop) for this demo. It can be run as an OSS process or in Pathway Environment.

##System Requirements 
Refer to the HPE Nonstop Server-Modern DevOps-Instructions-for-CI-CD-Setup Documnet_v1.1.pdf for details 

##Required Software  
Jenkins	 			2.222.3 or later
GITHub	 			2.26.2-64-bit or later
Java	       			JDK 1.8.0_181 or later version of JDK 8. 
Maven	Standard 			6.3 or later 
SonarQube				8.3.0.xx or later 
Junit					1.5.1 (included in the package)
Nexus Repository Manager	3.17.0.01 or later

#Usage Instructions 
Refer to JavaStarterKitUsageInstructions.docx for details of usage with Jenkins, AWSCodeBuid & AzureDevOps

