/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * network_reader.h
 */

#ifndef NETWORK_READER_H_
#define NETWORK_READER_H_

#include <string>
#include <vector>
#include "network_reader_config.h"
#include "request.h"

typedef struct ClientInfo {
	int fd;
	std::unique_ptr<Request> req;
}ClientInfo;

class NetworkReader {
private:
	int fd_;
	std::unique_ptr<NetworkReaderConfig> config_;
	std::vector<ClientInfo> client_info_;

public:
	NetworkReader(std::unique_ptr<NetworkReaderConfig> &config);
	void initialize();
	void start();
	void stop();
	virtual ~NetworkReader();
};

#endif /* NETWORK_READER_H_ */
