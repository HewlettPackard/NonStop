# Client server application on NonStop
This example is for the conventional NonStop programmer who is aware of the nuances of writing a fault tolerant server application using process pair technology and whose client is either an FS client or a Pathsend client.


In this example we have the server application using NSFTI. As can be seen in _server_ftilib.cpp_ the programmer opens $RECEIVE for inter-process communication as if the application is a normal application, i.e. without the knowledge of process pairs. The library ensures that only the primary opens the $RECEIVE. Its only upon failure of the primary, the backup opens its $RECEIVE to continue the business operation unabated.


While the focus is to demonstrate developing a fault tolerant server applications, the client is at best rudimentary.
For an adept NonStop process pair programmer the greatly reduced complexity in writing the process pair application using NSFTI, will surely not go unnoticed. For your quick reference the _server_pp.c_ is the conventional server application using FS API while _server_ftilib.cpp_ is the server application using NSFTI library

**NOTE**
> The sample server application is the same as the one in section _Active Backup Example_ of chapter _Fault-Tolerant Programming: Active Backup_ in the manual _Guardian Programmer's Guide_
