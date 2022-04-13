/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * network_reader_config.cpp
 */

#include "network_reader_config.h"

NetworkReaderConfig::NetworkReaderConfig(std::string host, int port, bool debug):
handler_(new RequestHandler(debug)){
	this->host_ = host;
	this->port_ = port;
	this->backlog_ = 10;
	this->max_connections_ = 20;
	this->debug_ = debug;
}

NetworkReaderConfig::~NetworkReaderConfig() {
	// TODO Auto-generated destructor stub
}

