/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * network_reader_config.h
 */

#ifndef NETWORK_READER_CONFIG_H_
#define NETWORK_READER_CONFIG_H_
#include <string>
#include "request_handler.h"

class NetworkReaderConfig {
private:
	std::string host_;
	int port_;
	int max_connections_;
	int backlog_;
	bool debug_;
	std::unique_ptr<RequestHandler> handler_;
public:
	NetworkReaderConfig(std::string host, int port, bool debug = false);
	virtual ~NetworkReaderConfig();

	std::string getHost() const {
		return host_;
	}

	int getMaxConnections() const {
		return max_connections_;
	}

	void setMaxConnections(int cons) {
		max_connections_ = cons;
	}

	int getPort() const {
		return port_;
	}

	int getBacklog() const {
		return backlog_;
	}

	void setBacklog(int value) {
		backlog_ = value;
	}
	bool isDebugEnabled() {
		return debug_;
	}
	RequestHandler &getHandler() const {
		return (*handler_.get());
	}
};

#endif /* NETWORK_READER_CONFIG_H_ */
