/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * request.cpp
 *
 */

#include "request.h"
#include <iostream>

Request::Request() {
	this->param_count_ = 0;
	this->remaining_ = 0;
	this->cur_size_ = 0;
	this->size_parsing_complete_ = false;
	this->param_found_ = false;
	this->length_found_=false;
	this->param_read_count_ = 0;
}

Request::~Request() {
	// TODO Auto-generated destructor stub
}

bool Request::isNewRequest(){
	return (param_count_ == 0);
}

void Request::setParamCount(int count) {
	this->param_count_ = count;
}
int Request::getParamCount() {
	return this->param_count_;
}
void Request::setRemaining(int value) {
	this->remaining_ = value;
}
int Request::getRemaining() {
	return this->remaining_;
}

void Request::updateElement(char *buf, int len) {
	remaining_ -= len;
	std::string &data = elements_.back();
	data.append(buf,len);
}

bool Request::isComplete() {
	return (remaining_ == 0 && param_count_ > 0 && param_count_ == elements_.size());
}

void Request::addElement(char *buf, int len) {
	elements_.push_back(std::string(buf,len));
}

void Request::reset() {
	elements_.clear();
	this->param_count_ = 0;
	this->remaining_ = 0;
	this->cur_size_ = 0;
	this->size_parsing_complete_ = false;
	this->param_found_ = false;
	this->length_found_=false;
	this->param_read_count_ = 0;
}

void Request::nextRequestData(char * buf, int len) {
	raw_data_.append(buf,len);
}

void Request::resetNextRequestData() {
	raw_data_.clear();
}
std::string Request::getNextRequestData() {
	return raw_data_;
}

bool Request::isDataAvailable() {
	return !raw_data_.empty();
}

std::vector<std::string> &Request::getElements() {
	return elements_;
}

Operation Request::getOperation() {
	if(!elements_.empty()) {
		std::string &operation = elements_[0];
		if(operation.compare("SET") == 0)
			return INSERT;
		if(operation.compare("GET") == 0)
			return READ;
		if(operation.compare("DEL") == 0)
			return DELETE;
	}
	return UNKNOWN;
}


int Request::getCurrentSize() {
	return this->cur_size_;
}
void Request::setCurrentSize(int size) {
	this->cur_size_ = size;
}
void Request::resetCurrentSize() {
	this->cur_size_ = 0;
	this->size_parsing_complete_ = false;
}
void Request::setSizeParsingComplete() {
	this->size_parsing_complete_ = true;
}
bool Request::isSizeParsingComplete() {
	return this->size_parsing_complete_;
}
bool Request::isParamIndicatorFound() {
	return this->param_found_;
}
void Request::setParamIndicatorFound(bool value) {
	this->param_found_ = value;
}
int Request::getParamReadCount() {
	return this->param_read_count_;
}
bool Request::isLengthIndicatorFound() {
	return this->length_found_;
}
void Request::setLengthIndicatorFound(bool value) {
	this->length_found_ = value;
}
void Request::increaseParamReadCount() {
	this->param_read_count_++;
}
