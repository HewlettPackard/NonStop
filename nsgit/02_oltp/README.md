# Sample OLTP Application

This example is a C++ TCP/IP server application built with NSFTI library. It receives INSERT, READ, UPDATE and DELETE commands over TCP socket from clients, processes them and stores the result in NSFTI's map data structure. The data stored in NSFTI's map can be retrieved even after the primary process goes down.

# Starting Server Application.

The server application 'TEST_APP' is located in the bin folder. The binary can be rebuilt from the source if required.

Steps to run the server

1. export _RLD_LIB_PATH=\<NSFTI library path>
2. run –name=\<name of the process> –cpu=\<cpu no> ./TEST_APP -a \<IP address> -p \<port>
   
   Example: run –name=/G/SVR –cpu=2 ./TEST_APP -a 127.0.0.1 -p 8080

The maximum number of connections and backlog can also be passed as command line arguments. 
./TEST_APP --help lists all the command line arguments.


# Starting client

The client "/bin/client.py" is a python program that sends commands to OLTP server running on NonStop. This client can be executed on NonStop or off-platform.

Before executing the client program the IP address and port on which the server is listening needs to be updated in "config.properties"

The CRUD commands can be sent to the server as follows:
```
python3 client.py --command INSERT --key=1234 --value=abcd
python3 client.py --command READ --key=1234
python3 client.py --command UPDATE --key=1234  --value=xyz
python3 client.py --command DELETE --key=1234
```
In between the commands the primary server process can be brought down to validate whether the data is still available after the process failure.

The commnad "python3 client.py" generates a workload based on the percentage values specified in config.properties until it is stopped.

# Source 

The folder "src" has source files for the OLTP server. 

### main.cpp 
This is the main program which initialises NSFTI library.

### network_reader_config.h, network_reader_config.cpp, network_reader.h & network_reader.cpp
These files do not have any NSFTI related code. These are responsible for reading data from socket. This has a loop which keeps reading data and invokes the request handler.

### parser.h & parser.cpp
Parses the incoming commands. This also doesn't have any NSFTI related code.

### request.h & request.cpp
This is to hold the request stream and state. Parser is using this to parse the content correctly.

### request_handler.h & request_handler.cpp
Once the complete request is received, the handler defined in these files is invoked by network_reader. This has NSFTI code to store, read and delete application data in NSFTI's map data structure.


# Compiling source

Run "/usr/coreutils/bin/make" command to build the server. The executable will be placed in "bin" directory.

