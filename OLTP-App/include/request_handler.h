/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * request_handler.h
 */

#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_
#include <memory>
#include <nsfti/ftilib.h>
#include "request.h"

/*
 * This class is responsible for handling the requests.
 */
using namespace nsfti;

class RequestHandler {
private:
	std::shared_ptr<Map<std::string,std::string>> ftmap_;
	bool debug_;
private:
	bool insert(std::string &key, std::string &value);
	std::string read(std::string &key);
	bool remove(std::string &key);
	void sendResponse(std::string &data, int sd);
	void sendResponse(bool error, int sd);
	void sendResponse(int count, int sd);
	void writeData(std::string &response, int sd);
public:
	RequestHandler(bool debug);
	void handleRequest(Request &req, int sd);
	virtual ~RequestHandler();
};

#endif /* REQUEST_HANDLER_H_ */
