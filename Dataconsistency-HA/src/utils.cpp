/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */

/*
 * This file contains utility APIs that are used by the example applications.
 */
#include "utils.h"
using namespace std;

/*
 ****************************keyGenerator***********************************
 */

/* Constructor for keyGenerator */
keyGenerator::keyGenerator() {
	time_t t;
	/* Seed the random generator engine */
	srand((unsigned) time(&t));
}

/* Method to update the base key */
unsigned int keyGenerator::updateBasekeystring(std::string str) {
	baseKeyString = str;
	return baseKeyString.size();
}

/* Method to update the base value */
unsigned int keyGenerator::updateBaseValuestring(std::string str) {
	baseValueString = str;
	return baseValueString.size();
}

/* Method to generate a random number within a range.
 * 0 <= random < order
 */
unsigned int keyGenerator::getRandomNum(unsigned int order) {
	return rand() % order;
}

/* timerHandler function to report the transaction numbers in 1 second interval*/
int timerHandler(void(*fptr)(int),int sec){
	struct sigaction sa;
	struct itimerval timer;

	/* Install timer_handler as the signal handler for SIGVTALRM. */
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = fptr;
	sigaction (SIGALRM, &sa, NULL);

	/* Configure the timer to expire after 1sec... */
	timer.it_value.tv_sec = sec;
	timer.it_value.tv_usec = 0;
	/* ... and every 1 sec after that. */
	timer.it_interval.tv_sec = sec;
	timer.it_interval.tv_usec = 0;
	/* Start a virtual timer. It counts down whenever this process is
	  executing. */
	setitimer (ITIMER_REAL, &timer, NULL);
}

/* Method to generate a key using the index parameter
 * This function will replace last few characters of the base string
 * depending on the index passed
 */
void keyGenerator::generateKeyWithIndex(int id, std::string &key) {
	char sr[1024] = "";
	sprintf(sr, "%d", id);
	std::string returnKey = baseKeyString;
	key = returnKey.replace(baseKeyString.size() - strlen(sr), strlen(sr),
			std::string(sr));
}

/* Method to generate a value using the index parameter
 * This function will replace last few characters of the base string
 * depending on the index passed
 */
void keyGenerator::generateValueWithIndex(int id, std::string &value) {
	char sr[1024] = "";
	sprintf(sr, "%d", id);
	std::string returnValue = baseValueString;
	value = returnValue.replace(baseValueString.size() - strlen(sr), strlen(sr),
			std::string(sr));
}

/*
 ****************************readWriteFile***********************************
 */

/* Class constructor */
readWriteFile::readWriteFile() {
	fileName = string("test.txt");
	fileStream.open(fileName,
			std::fstream::in | std::fstream::out | std::fstream::app);
}

/* Overload class constructor */
readWriteFile::readWriteFile(std::string filename) {
	fileName = filename;
	fileStream.open(filename,
			std::fstream::in | std::fstream::out | std::fstream::app);
}

/* Method to seek to the start of file */
void readWriteFile::gotoFileStart(void) {
	fileStream.seekg(0, ios_base::beg);
}

/* Method to seek to the end of file */
void readWriteFile::gotoFileEnd(void) {
	fileStream.seekg(0, ios::end);
}

/* Method to write a string to file
 * The str parameter is appended with a newline character and
 * written to the file
 */
void readWriteFile::writeOneLineToFile(std::string str) {
	fileStream << str << endl;
}

/* Method to reset the file stream error status */
void readWriteFile::resetFileAtEOD(void) {
	fileStream.clear();
}

/* Method to read a line from file and return its length */
unsigned long readWriteFile::readOneLineFromFile(std::string & str) {
	getline(fileStream, line);
	str = line;
	if (!line.size())
		return 0;
	str = line;
	return str.size();
}

/* Destructor for the class. */
readWriteFile::~readWriteFile() {
	fileStream.close();
}

/*
 ****************************tokenizeString***********************************
 */

/* Class constructor
 * Parameter tkn is used to init the class
 */
tokenizeString::tokenizeString(std::string tkn) {
	token = tkn;
}

/* breakStringWithToken tokenize's the string input w.r.t the token.
 * Token is initialized when an instance of tokenizeString
 * class is created.
 * This method uses strtok c function to do it and the result is
 * pushed into a vector.
 * Returns the  total number of strings.
 */
unsigned long tokenizeString::breakStringWithToken(std::string str) {
	char * pch;
	char *pattern = strdup(str.c_str());
	char *srcStr = strdup(token.c_str());
	pch = strtok(pattern, srcStr);
	while (pch != NULL) {
		vectorOfStrings.push_back(pch);
		pch = strtok(NULL, srcStr);
	}
	free(pattern);
	free(srcStr);
	return vectorOfStrings.size();
}

/*
 ****************************configReader***********************************
 */

/* Class constructor
 * The entries in the configuration file are key-value pairs.
 * Each key-value pair is separated by a colon (':') .
 * Configuration parameter should be one per line.
 */
configReader::configReader(std::string fileName) :
		readWriteFile(fileName) {

}

/* Method to return the value from a configuration parameter */
std::string configReader::readConfigParameter(std::string configParameter) {
	std::string line;
	gotoFileStart();
	while (readOneLineFromFile(line)) {
		tokenizeString tkn = tokenizeString(":");
		tkn.breakStringWithToken(line);
		if (!tkn.vectorOfStrings[0].compare(configParameter)) {
			return tkn.vectorOfStrings[1];
		}
	}
	return NULL;
}

/*
 ****************************timer***********************************
 */

/* Constructor for the class */
timer::timer() {
	memset(&t0, 0, sizeof(t0));
	memset(&t1, 0, sizeof(t1));
}

/* Method to start the timer */
void timer::startTimer(void) {
	gettimeofday(&t0, 0);
}

/* Method to return the elapsed time, from a previously started timer.
 * If timer is not started then the elapsed time returned is undefined.
 */
unsigned long timer::getElapsedTime(void) {
	gettimeofday(&t1, 0);
	return ((t1.tv_sec * 1000000 + t1.tv_usec)
			- (t0.tv_sec * 1000000 + t0.tv_usec));
}

/* Method to reset the timer */
void timer::resetTimer(void) {
	memset(&t0, 0, sizeof(t0));
	memset(&t1, 0, sizeof(t1));
}

/*
 ****************************processInfo***********************************
 */

/* Constructor for the class */
processInfo::processInfo() {
	getProcessAttribute();
}

/* Method to get the process attributes */
void processInfo::getProcessAttribute(void) {
	short pHandle[16], ret, myNamelen;
	char myName[30];
	ret = PROCESSHANDLE_GETMINE_(pHandle);
	if (ret != 0) {
		return;
	}
	ret = PROCESSHANDLE_DECOMPOSE_(pHandle,
			&processCpu,
			&processPin,
			, // node number
			,// node name
			,// maxlen
			,// node name length
			myName,
			sizeof(myName) - 1,
			&myNamelen);

	if (ret != 0) {
		return;
	}
	myName[myNamelen] = '\0';
	processName = myName;
	return;
}

/* Method to get the process information */
void processInfo::getProcessInfo(std::string &name, short &cpu, short &pin) {
	name = processName;
	cpu = processCpu;
	pin = processPin;

}


