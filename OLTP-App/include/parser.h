/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * parser.h
 */

#ifndef _PARSER_H_
#define _PARSER_H_
#include <memory>
#include "request.h"

class Parser {
private:
	bool debug_;
public:
	Parser(bool debug);
	int parse(Request &req, char *buffer, int length);
	char *parseSize(Request &req, char *curptr, char *buffer, int buf_length);
	virtual ~Parser();
};

#endif /* _PARSER_H_ */
