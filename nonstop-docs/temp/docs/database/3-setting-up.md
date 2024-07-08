---
sidebar_label: 'Setting Up Tomcat'
sidebar_position: 3
---

# Getting Started

## Our NonStop IT Team
In this tutorial, we will be creating a Java application on Apache Tomcat to help the NonStop IT Team with their Tickets. The application will manage new tickets, and edit existing tickets. 

We will need 2 tables for this - **User and Ticket**. 

The **Ticket** table will be located in the **SQL/MX**, and the **User** table will be located in the **MySQL**.

## Functions To Complete

At the end of this tutorial, our Java application should be able to 
1. Create new Tickets, if they are an existing user
2. Update existing Tickets if the tickets exist


# Setting up our development machine

Before we start coding, we are going to do some prep work for our development machines.

Check and make sure that you have the following:

**1. SQLMX** -  While we're keep on working on the fault-tolerant features of this database, come speak with us for a demo and a good feel of our efforts! 

**2. Java** - [Download Java for Windows](https://www.java.com/download/ie_manual.jsp)

**3. MySQL** - [Download MySQL for Windows](https://dev.mysql.com/downloads/mysql/)

**4. Apache Tomcat** - step-by-step guide down in the next segment.

For this tutorial, we are using MySQL-8.0.30-winx64 package, Java Openjdk version 18.0.1 - development is on a Windows OS.

## How to Install Apache Tomcat

### 1.0 Creating a directory for this tutorial
Create a directory and name it as ItTicketing to store and keep track of the project files.

### 1.1 Downloading and Installing Tomcat

For Windows:
1. Go to [Tomcat Downloads](https://tomcat.apache.org/) and search for "Downloads" under Tomcat 10.x Released segment.

2. Scroll down to the Source Code Distributions segment download the zip file.

3. Unzip and extract the downloaded file into the ItTicketing project directory. 

4. Rename the extracted folder to \tomcat. i.e `C:\ItTicketing\tomcat`

#### Tomcat's Sub-directories
* **bin** - contains binaries and scripts to startup and shutdown the server.
* **conf** - contains the system-wide configuration files such as server.xml, web.xml and content.xml.
* **webapps** - contains the webapps to be deployed. You can place WAR files for deployment in this folder.
* **lib** - contains Tomcat's system-wide library JAR files, accessible by all webapps. All other JAR files such as JDBC drivers will be placed here.
* **logs** - contains Tomcat's log files with error messages as generated.
* ** work** - Tomcat's working directory used by JSP for JSP-to-Servlet conversion.

### 1.2 Creating the JAVA_HOME environment variable

For Windows:
1. Search for "Edit System Variables" on your machine. 

2. Create an environment variable called "JAVA_HOME" and set it to the JDK installed directory.

### 1.3 Configuring the Tomcat Server

The Tomcat configuration files in XML format are located in teh "conf" sub-directory of the Tomcat installed directory. i.e `server.xml, web.xml, content.xml`

>**Create a backup copy of the configuration files**.

#### 1.3.1 Setting the TCP Port Number - `\conf\server.xml`

1. Open the `\conf\server.xml` file using a text editor (such as VSCode, Sublime Text or Atom).

2. Change the port variable in the "Connector" to be **"port="2020""**.  

  

A "Connector" represents an endpoint by which requests are received and responses are returned.

    <Connector port="2020" protocol="HTTP/1.1"
               connectionTimeout="20000"
               redirectPort="8443" />

#### 1.3.2 Enabling Directory Listing - `\conf\web.xml`

1. Open the `\conf\web.xml` file using a text editor.

2. Change the **param-value to false** to enable directory listing.

>       ...
        <init-param>
            <param-name>listings</param-name>
            <param-value>false</param-value>
        </init-param>
        <load-on-startup>1</load-on-startup>
    </servlet>


#### 1.3.3 Enabling Automatic Reload - `\conf\context.xml`

1. Open the `\conf\context.xml` file using a text editor.

2. Add **reloadable="true"**, **crossContext="true"**, and **parallelAnnotationScanning="true"** into the _Context_ attribute.
> 
    <Context reloadable="true" crossContext="true" parallelAnnotationScanning="true">

### 1.4 Starting the Tomcat Server

#### 1.4.1 Starting the Server

For Windows:
1. Launch a Command Shell console and navigate into the Tomcat folder's `bin`.

>   _cd C:\ItTicketing\tomcat\bin_
>    
>   _startup_ 
    
#### 1.4.2 Starting a Client to access the Server

1. Start a browser and connect using the link - [http://localhost:2020](http://localhost:2020).

2. The Tomcat server's welcome page should appear. 

3. To view other servlet examples, try navigating to [http://localhost:2020/examples](http://localhost:2020/examples).

#### 1.4.3 Shutting down the Server

For Windows:

* Press Ctrl+C on the Tomcat console **OR**
* Open another Command Shell console and run the `shutdown.bat` script.

>   _cd C:\ItTicketing\tomcat\bin_
>    
>   _shutdown_

### 1.5 Developing and Deploying a "Hello World!" Web App

#### 1.5.1 Creating a directory structure for the application - `\hello`
Let's create our first Hello World webapp with Tomcat.

1. Create a new folder `hello` in the `webapps` directory, a `WEB-INF` in the `hello` folder, and a `classes` folder in `WEB-INF` -> `\tomcat\webapps\hello`
>   _cd C:\ItTicketing\tomcat\webapps_
>
>  _mkdir hello_
>
>  _cd C:\ItTicketing\tomcat\webapps\hello_
>
>  _mkdir WEB-INF_
>


