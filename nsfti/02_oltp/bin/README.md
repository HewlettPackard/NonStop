This is an oltp server  application. This appliation is linked with NSFTI library and this application accepts the requests for insert, get and 
erase operation. The requests for this oltp server is generated externally.
client.py is the client that can be used as the client for the oltp server. Using the configuration file config.properties user can request
 different operations percentages. A prefill of number of keys can also be configured for the python client.

#run the oltp application as follows

run -name=/G/SVR ./OLTP-App  -a 127.0.0.1 -p 8080 -m 20
