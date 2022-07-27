/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */
/*
 * Common header file used by examples
 * A copy of this header is available in all examples
 */

#include <fstream>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>


/* For PROCESSHANDLE_GETMINE_ and PROCESSHANDLE_DECOMPOSE_ APIs */
#include <cextdecs.h>

#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_

/* keyGenerator class:
 * to get random number
 * to get a sequential key w.r.t an index from a base key
 * to get a sequential value w.r.t an index from a base value
 * */
class keyGenerator {
private:
	/* User defined key pattern, used to generate key names */
	std::string baseKeyString;
	/* User defined value pattern, used to generate values */
	std::string baseValueString;

public:
	/* Constructor for this class */
	keyGenerator();
	/* Method to get a generated random number*/
	unsigned int getRandomNum(unsigned int order);
	/* Method to update the base key string, so that a predefined key sequence can be generated*/
	unsigned int updateBasekeystring(std::string str);
	/* Method to update the base value string, so that a predefined value sequence can be generated*/
	unsigned int updateBaseValuestring(std::string str);
	/* Method to generate and get a key string with the sequence of index */
	void generateKeyWithIndex(int index, std::string &key);\
	/* Method to generate and get a key string with the sequence of index */
	void generateValueWithIndex(int id, std::string &value);
};

/* readWriteFile class:
 * to open a file
 * to read from a file line by line
 * write to a file line by line
 */
class readWriteFile {
private:
	/* File name to be opened */
	std::string fileName;
	std::string line;
	/* File stream object to read and write a file */
	std::fstream fileStream;

public:
	/* Class constructor */
	readWriteFile();
	/* Overloaded class constructor */
	readWriteFile (std::string);
	/* Class destructor */
	~readWriteFile();
	/* Method to reset the file stream error status */
	void resetFileAtEOD(void);
	/* Method to seek to the start of the file */
	void gotoFileStart(void);
	/* Method to seek to the end of the file */
	void gotoFileEnd(void);
	/* Method to write one line into the opened file */
	void writeOneLineToFile(std::string str);
	/* Method to read one line from the opened file */
	unsigned long readOneLineFromFile(std::string & str);
};

/* tokenizeString class:
 * to tokenize a string using a given token
 */
class tokenizeString {
private:
	/* The string token */
	std::string token;

public:
	/* The vector data , which will have the tokenized  strings */
	std::vector<std::string> vectorOfStrings;
	/* Method to initialize the string token */
	tokenizeString(std::string tkn);
	/* Method to break the string with the token */
	unsigned long breakStringWithToken(std::string st);
};

/* configReader class:
 * to get the configuration parameters
 */
class configReader: public readWriteFile {
private:
	/* Configuration file name variable */
	std::string configFileName;
public:
	/* Parameterised Constructor */
	configReader(std::string fileNme);
	/* Return the configuration value of the passed configuration parameter */
	std::string readConfigParameter(std::string configParameter);
};

/* timer class:
 * to have minimalistic timer functions
 */
class timer {
private:
	/* Timeval structure variables to track the timer */
	struct timeval t0, t1;
public:
	/* Constructor */
	timer();
	/* Method to start the timer */
	void startTimer(void);
	/* Method to get the elapsed time in micro seconds */
	unsigned long getElapsedTime(void);
	/* Method to reset the timer */
	void resetTimer(void);
};

/* class processInfo:
 * to the process related information
 */
class processInfo {
private:
	/* Read process name information */
	std::string processName;
	/* Read CPU information of the process */
	short processCpu;
	/* Read PIN information of the process */
	short processPin;
	/* Method for getting the process attribute */
	void getProcessAttribute(void);
public:
	/* Constructor for this class */
	processInfo();
	/* Method to get the information to the user */
	void getProcessInfo(std::string &name, short &cpu, short &pin);

};

int  timerHandler(void(*fptr)(int),int sec);
#endif
