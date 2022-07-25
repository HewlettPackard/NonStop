/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * parser.cpp
 *
 */

#include <stdio.h>
#include "parser.h"
#include "request.h"
#include <iostream>

Parser::Parser(bool debug) {
	this->debug_ = false;//debug;
}

Parser::~Parser() {
	// TODO Auto-generated destructor stub
}

char *Parser::parseSize(Request &req, char *curptr, char *buffer, int buf_length) {
	int len = req.getCurrentSize();

	while(curptr < (buffer + buf_length)) {
		if(*curptr == '\r'){
			curptr++;
			continue;
		}
		if(*curptr == '\n'){
			req.setSizeParsingComplete();
			curptr++;
			break;
		}
		len = (len*10)+(*curptr - '0');
		curptr++;
	}
	req.setCurrentSize(len);

	return curptr;
}
/*
 * Parses the raw data and updates the Request Object.
 */
int Parser::parse(Request &req, char *buffer, int buf_length) {
	if(debug_ && buffer != NULL) {
		fwrite(buffer, buf_length, 1, stdout);
		std::cout<<"\n-----\n";
	}

    char *p = buffer;
	std::string raw_data = req.getNextRequestData();
	if(!raw_data.empty()) {
		p = buffer = (char *)raw_data.c_str();
		buf_length = raw_data.length();
		req.resetNextRequestData();
	}

	 while(p < (buffer + buf_length)) {
		 int len = 0;
		if(req.getParamCount() == 0) {
			if(!req.isParamIndicatorFound()) {
				if(*p == '*') {
					req.setParamIndicatorFound(true);
					p++;
				} else {
					std::cout<<"Protocol error. Parsing parameter count failed.\n";
					return -1;
				}
			}
			p = parseSize(req, p, buffer, buf_length);

			if(!req.isSizeParsingComplete())
				return 1;
			req.setParamCount(req.getCurrentSize());
			req.resetCurrentSize();
			continue;
		}
		if(req.getParamReadCount() < req.getParamCount()) {
			if(!req.isLengthIndicatorFound()) {
				if(*p == '$') {
					req.setLengthIndicatorFound(true);
					p++;
				} else {
					std::cout<<"Protocol error. Parsing length failed.\n";
					return -1;
				}
			}
			if(!req.isSizeParsingComplete()) {
				p = parseSize(req, p, buffer, buf_length);
				if(!req.isSizeParsingComplete())
					return 1;
				req.setRemaining(req.getCurrentSize());
				continue;
			}
			len = req.getRemaining(); // length can be 0 if the previous buffer had all data except \r\n
			if(len == 0) {
				if(*p == '\r') {
					p++;
					continue;
				}
				if(*p == '\n') {
					req.increaseParamReadCount();
					p++;
					// Reset the counters to read the next parameter,
					req.resetCurrentSize();
					req.setLengthIndicatorFound(false);
					continue;
				}
			}
			if(p + len  < (buffer + buf_length)) {
				if(len == req.getCurrentSize()) // First chunk of data
					req.addElement(p,len);
				else
					req.updateElement(p,len);
				p=p+len;
				req.setRemaining(0);
			} else {
				int data_len = buf_length - (p - buffer);
				if(len == req.getCurrentSize()) // First chunk of data
					req.addElement(p, data_len);
				else
					req.updateElement(p, data_len);
				req.setRemaining(len - data_len);
				return 1;
			}
		} else {
			int next_req_len = buf_length - (p - buffer);
			if(next_req_len != 0) {
				req.nextRequestData(p,next_req_len);
			}
			return 0;
		}
	 }
	// Complete data for the request is received.
	if(req.getParamReadCount() == req.getParamCount())
		return 0;

	return 1;
}
