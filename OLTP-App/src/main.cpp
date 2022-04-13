/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
 
#include <unistd.h>
#include <string>
#include <iostream>
#include <nsfti/ftilib.h>
#include "network_reader_config.h"
#include "network_reader.h"
#include <getopt.h>


/*
 * This is a sample fault-tolerant OLTP server application built with NSFTI library.
 * It receives INSERT, READ, UPDATE and DELETE commands from the clients, processes them and stores the result through NSFTI.
 */
void usage()
{
    std::cout <<
            "--address <ip address>: IP address/host name of the machine\n"
            "--port    <port>:  Port number to bind to\n"
            "--max_connections <value>:  Maximum number of connections to be handled.\n"
            "--backlog <value>: Number of pending connections the queue will hold\n"
    		"--debug: Enable debug messages\n"
            "--help:              Show help\n";
    exit(1);
}

int main(int argc, char *argv[]) {
	int port = 8080;
	std::string host("localhost");
	int max_connections = 20;
	int backlog = 10;
	bool debug = false;

	/*
	 * Parse the command line arguments.
	 */
	const char* const short_opts = "a:p:m:b:dh";
	const option long_opts[] {
			{ "address", required_argument, nullptr, 'a' },
			{ "port", required_argument, nullptr, 'p' },
			{ "max_connections", required_argument, nullptr, 'm' },
			{ "backlog", required_argument, nullptr, 'b' },
			{ "debug", no_argument, nullptr, 'd' },
			{ "help", no_argument, nullptr, 'h'},
			{ nullptr, no_argument, nullptr, 0 } };

	while (true) {
		const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

		if (-1 == opt)
			break;

		switch (opt) {
		case 'a':
			host = std::string(optarg);
			break;

		case 'p':
			port = std::stoi(optarg);
			break;

		case 'm':
			max_connections = std::stoi(optarg);
			break;

		case 'b':
			backlog = std::stoi(optarg);
			break;

		case 'd':
			debug = true;
			break;

		case 'h': // -h or --help
		case '?': // Unrecognized option
		default:
			usage();
			break;
		}
	}

	/*
	 * FTILib::initialize function must be invoked first, before processing any request.
	 */
	std::shared_ptr<nsfti::GlobalConfig> nsfti_config = nsfti::FTILib::getGlobalConfig();
	nsfti_config->setProgramArgs(argc, argv);
	try {
		nsfti::FTILib::initialize();
	} catch (char *err) {
		std::cerr<< err << std::endl;
		exit(1);
	}

	std::unique_ptr<NetworkReaderConfig> reader_config(new NetworkReaderConfig(host,port,debug));
	reader_config->setMaxConnections(max_connections);
	reader_config->setBacklog(backlog);
	NetworkReader reader(reader_config);
	reader.start();

	return 0;
}
