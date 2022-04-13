/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * request.h
 */

#ifndef REQUEST_H_
#define REQUEST_H_

/*
 * This class is used to parse the messages along with the Parser class.
 * The state of the request is stored in this class. The state information is used by the Parser.
 */
#include <vector>
#include <string>

enum Operation {
    INSERT,
    UPDATE,
    READ,
    DELETE,
    UNKNOWN
};

class Request {
private:
	int param_count_;
	int remaining_;
	std::vector<std::string> elements_;
	std::string raw_data_;
	int cur_size_;
	bool size_parsing_complete_;
	bool param_found_;
	bool length_found_;
	int param_read_count_;
public:
	Request();
	void setParamCount(int count);
	int getParamCount();
	void setRemaining(int value);
	int getRemaining();
	void addElement(char *buf, int len);
	void updateElement(char *buf, int len);
	bool isNewRequest();
	bool isComplete();
	void reset();
	void nextRequestData(char * buf, int len);
	void resetNextRequestData();
	std::string getNextRequestData();
	bool isDataAvailable();
	std::vector<std::string> &getElements();
	Operation getOperation();

	int getCurrentSize();
	void setCurrentSize(int size);
	void resetCurrentSize();
	bool isSizeParsingComplete();
	void setSizeParsingComplete();
	bool isParamIndicatorFound();
	void setParamIndicatorFound(bool value);
	int getParamReadCount();
	bool isLengthIndicatorFound();
	void setLengthIndicatorFound(bool value);
	void increaseParamReadCount();

	virtual ~Request();
};

#endif /* REQUEST_H_ */
