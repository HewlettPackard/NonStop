# Client server application on NonStop
This example is for the conventional NonStop programmer who is aware of the nuances of writing a fault tolerant server application using process pair technology and whose client is either an FS client or a Pathsend client.

In this example we have the server application using NSFTI and only the primary process opening the $RECEIVE. Upon failure of the primary, the backup opens its $RECEIVE to continue the business operation unabated.

While the focus is to demonstrate developing a fault tolerant server applications, the client is at best rudimentary.
For an adept NonStop process pair programmer the greatly reduced complexity in writing the process pair application using NSFTI, will surely not go unnoticed. For your quick reference the _server_pp.c_ is the conventional server application using FS API while _server_ftilib.c_ is the server application using NSFTI library
