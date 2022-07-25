/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */

/*
 * request_handler.cpp.
 *
 * The request handling is implemented in this file. The application state is stored through NSFTI map interface.
 */

#include "request_handler.h"
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <sys/time.h>

/*
 * Creates fault-tolerant map object here for processing commands later.
 */
RequestHandler::RequestHandler(bool debug) {
	this->debug_ = debug;
	std::shared_ptr<FTILib> instance = FTILib::getInstance();
	std::shared_ptr<MapFactory> factory = instance->getMapFactory();
	ftmap_ = factory->createNamedMap<std::string,std::string>("OLTP_EXAMPLE");
}

/*
 * Handles the request. Invokes appropriate function based on the operation type.
 * Parameters: req - holds the request data.
 *             sd - socket descriptor to write the response.
 *
 */
void RequestHandler::handleRequest(Request &req, int sd) {
	std::vector<std::string> &elements = req.getElements();
	switch (req.getOperation()) {
		case INSERT: {
			bool code = insert(elements[1],elements[2]);
			sendResponse(code, sd);
			req.reset();
			break;
		}
		case READ: {
			try {
				std::string data = read(elements[1]);
				sendResponse(data,sd);
			}catch(const std::exception &e) {
				std::cerr<< e.what() << std::endl;
				sendResponse("",sd);
			}
			req.reset();
			break;
		}
		case DELETE: {
			bool code = remove(elements[1]);
			int count = 0;
			count = (code) ? 1 : 0;
			sendResponse(count,sd);
			req.reset();
			break;
		}
		default: {
			sendResponse(false,sd);
			break;
		}
	}
}
/*
 * Inserts the key and value in NSFTI map. NSFTI internally stores the data in the backup process as well.
 * When the primary process goes down, the inserted data can still be accessed.
 */
bool RequestHandler::insert(std::string &key, std::string &value) {
	int ret = 0;
	if(debug_) {
		std::cout<<"Inserting.. Key Length[" << key.length() << "] Value Length[" << value.length() << "]" << key << "\n";
	}
	ret =  ftmap_->insert(key,value);
	return ret;

}
/*
 * Returns the value for a given key.
 */
std::string RequestHandler::read(std::string &key) {
	std::string ret ;
	if(debug_) {
		std::cout<<"Reading.. Key Length[" << key.length()  << "]" << key << "\n";
	}
	ret =  ftmap_->get(key);
	return ret;
}
/*
 * Removes the key from the NSFTI map.
 */
bool RequestHandler::remove(std::string &key) {
	int ret = 0;
	if(debug_) {
		std::cout<<"Deleting.. Key Length[" << key.length()  << "]" << key << "\n";
	}
	ret =  ftmap_->erase(key);
	return ret;
}


RequestHandler::~RequestHandler() {
	// TODO Auto-generated destructor stub
}

/*
 * Writes the response to the socket descriptor.
 */
void RequestHandler::sendResponse(std::string &data, int sd){
	if(data.empty())
		sendResponse("$-1\r\n", sd);
	else {
		std::string CRLF = "\r\n";
		std::string buffer("$");
		buffer.append(std::to_string(data.length()));
		buffer.append(CRLF);
		buffer.append(data);
		buffer.append(CRLF);
		writeData(buffer,sd);
	}
}
void RequestHandler::sendResponse(int count, int sd){
	std::string CRLF = "\r\n";
	std::string data(":");
	data += std::to_string(count);
	data += CRLF;
	writeData(data,sd);
}

void RequestHandler::sendResponse(bool success, int sd) {
	std::string response_msg = "+OK\r\n";
	std::string error_msg = "-Operation failed\r\n";
	if(success)
		writeData(response_msg,sd);
	else {
		//if(debug_)
			std::cout<<"Operation failed." << std::endl;
		writeData(error_msg,sd);
	}
}
void RequestHandler::writeData(std::string &response, int sd) {
	int written = 0;
	const char *buffer = response.c_str();
	int buf_length = response.length();
	while(written < buf_length) {
		written += send(sd,buffer + written, buf_length - written,0);
	}
}
