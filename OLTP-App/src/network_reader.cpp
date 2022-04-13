/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * network_reader.cpp
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <system_error>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include "network_reader.h"
#include "parser.h"
#include "request.h"

/*
 * This class is responsible for reading messages from the TCP/IP socket. It uses select() system call to multiplex the connections.
 * The message read from the socket is passed to the Parser class to parse the message. The parsed message is stored in Request object and
 * once the complete message is received, the object is passed to RequestHandler::handleRequest() for processing.
 */
NetworkReader::NetworkReader(std::unique_ptr<NetworkReaderConfig> &config) {
	this->config_ = std::move(config);
	initialize();
}

void NetworkReader::initialize() {
	int opt = true;
	struct sockaddr_in address;

	client_info_.resize(config_->getMaxConnections());

	if ((fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw std::system_error(EFAULT, std::generic_category());
	}
	if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt))
			< 0) {
		throw std::system_error(EFAULT, std::generic_category());
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(config_->getHost().c_str());
	address.sin_port = htons(config_->getPort());

	if (bind(fd_, (struct sockaddr *) &address, sizeof(address)) < 0) {
		throw std::system_error(EFAULT, std::generic_category());
	}
	if (listen(fd_, config_->getBacklog()) < 0) {
		throw std::system_error(EFAULT, std::generic_category());
	}
}
/*
 * This function waits for connections from client, reads messages, invokes parser and request handlers.
 */
void NetworkReader::start() {
	int new_socket, activity, i, valread, sd;
	struct sockaddr_in address;
	int addrlen;
	int max_fd;
	char buffer[1025]; //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;
	int max_clients = config_->getMaxConnections();
	RequestHandler &handler = config_->getHandler();
	Parser parser(config_->isDebugEnabled());

	//accept the incoming connection
	addrlen = sizeof(address);
	std::cout<<"Waiting for connections ...\n";

	while (1) {
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(fd_, &readfds);
		max_fd = fd_;

		//add child sockets to set
		for (i = 0; i < max_clients; i++) {
			//socket descriptor
			sd = client_info_[i].fd;

			//if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);

			//highest file descriptor number, need it for the select function
			if (sd > max_fd)
				max_fd = sd;
		}

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR)) {
			std::cout<<"select error: errno " << errno;
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(fd_, &readfds)) {
			if ((new_socket = accept(fd_, (struct sockaddr *) &address,
					(socklen_t*) &addrlen)) < 0) {
				throw std::system_error(EFAULT, std::generic_category());
			}
			int flags;
			flags = fcntl(new_socket,F_GETFL,0);
			fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);

			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++) {
				//if position is empty
				if (client_info_[i].fd == 0) {
					client_info_[i].fd = new_socket;
					client_info_[i].req = std::unique_ptr<Request>(new Request());
					break;
				}
			}
		}

		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++) {
			sd = client_info_[i].fd;
			Request *req = client_info_[i].req.get();
			valread = 0;

			if (FD_ISSET(sd, &readfds)) {
				//Check if it was for closing , and also read the
				//incoming message
				do {
					Request *req = client_info_[i].req.get();
					valread = read(sd, buffer, 1024);
					if ( valread == 0) {
						//Close the socket and mark as 0 in list for reuse
						close(sd);
						client_info_[i].fd = 0;
					} else if( valread < 0 && errno != EAGAIN){
						//Error.
					} else {
						/*
						 * It is possible the data read from the socket may have multiple requests.
						 * Process all the requests.
						 */
                        while(valread != 0 || req->isDataAvailable()) {
							int status = 0;
							if(valread > 0)
								status = parser.parse(*req, buffer,valread);
							else
								status = parser.parse(*req, NULL,0);
							if(status == 0) {
								handler.handleRequest(*req,sd);
							} else if(status == -1) {
								// TODO Error occured. send correct data.
							}
							valread = 0;
                        }
                        // Break the loop after processing the request if if there is data on the socket to read.
                        // This is to serve the requests waiting on other connections.
						break;
					}
				}while(valread > 0);
			}
		}
	}
}
NetworkReader::~NetworkReader() {
	// TODO Auto-generated destructor stub
}


