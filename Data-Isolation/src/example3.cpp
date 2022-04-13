/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */

/*
 *  Sample application to demonstrate data isolation feature of NSFTI
 *  namespaces.
 *
 * This application creates named hash maps with same name but in
 * different name spaces. The application performs operations on
 * these named maps to demonstrate the name space isolation feature
 * of the library.
 *
 * The input data is a CSV formatted file. The application reads this file
 * & performs the following operations
 * creates named maps,
 * stores key-values using `insert` API
 * and retrieves map data using `get` API.
 *
 * The application reads the data from file and stores the lines/records in
 * a vector of strings. Each line/record is tokenized to get the name
 * of the hash map and the key-value pair information.
 *
 * Each comma separated line is parsed as
 * hashmap name, key-name, value1,value2.
 * The first line of the file is always parsed as a comment.
 *
 * Once all the records are inserted, randomly picked records
 * are displayed.
 *
 */

/* ftilib.h is the only header user need to include for compiling the
 * app to use the nsfti library infrastructure.
 */
#include <nsfti/ftilib.h>
#include <memory>
#include <vector>
#include <iostream>

/* utility API header file */
#include "utils.h"

/* namespace to be included while we use nsfti library */
using namespace nsfti;
using namespace std;

/* displayWithProcessinfo
 * This function prints out the display with the <cpu,pin>
 * information with the actual message
 */
void displayWithProcessinfo(std::string displayString) {
	static short cpu = 0, pin = 0;
	static std::string pName;
	if (!cpu) {
		/* To get the current process information */
		processInfo info = processInfo();
		info.getProcessInfo(pName, cpu, pin);
	}

	cout << "<" << cpu << "," << pin << "> ";
	cout << displayString;
}
/* exit_demo()
 * Function to show the gracefully shutdown the application
 */
void exit_demo(void) {
	/* API to get the FTILib instance. */
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();
	displayWithProcessinfo("exiting application\n");
	/* API to be called for the graceful exit of the application */
	ftiLibInstance->shutdown(EXIT_SUCCESS);
}

/* readCSV
 * Function to read from the csv file and return the string vector
 * (records ) read
 */
std::vector<std::string> readCSV(std::string fileName) {
	/* readWriteFile object for reading from the file */
	readWriteFile rd = readWriteFile(fileName);
	/* vector to keep the read lines/records */
	std::vector < std::string > lines;
	/* string to read one line/records from the file */
	std::string line;
	/* Read one line/row from the file* /
	 /* Discard the first line considering it is the description*/
	rd.readOneLineFromFile(line);
	line.erase();
	while (rd.readOneLineFromFile(line)) {
		/* Read one by one line/record and keeping it in the vector */
		lines.push_back(line);
	}
	/* return the read lines/records */
	return lines;
}

/* generateMapWithNameSpace
 * This function will create namedmap with namespace reference
 * different name spaces can have maps with the same name.
 *
 * In this function two namespace reference is used to create
 * same named hashmap with same keys with differnt values in it
 */
int generateMapWithNameSpace(std::vector<std::string> v,
		shared_ptr<Namespace> &FaceValueNS,
		shared_ptr<Namespace> &TradeValueNS) {

	/* Variables for map instance */
	shared_ptr<Map<std::string,double>> Facemap, Trademap;
	/* Get the FTILib instance */
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();
	/* Get the MapFactory for creating the map */
	shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();

	/* Iterate through all the strings in the vector */
	int i = 0;
	for (i = 0; i < v.size(); i++) {
		tokenizeString tkn = tokenizeString(",");
		/* tokenizing the line/records */
		tkn.breakStringWithToken(v[i]);
		/* Creating the namedmap with namespace reference */
		Facemap = factory->createNamedMap<std::string,double>(tkn.vectorOfStrings[0], FaceValueNS);
		/* Creating the same namedmap with different namespace reference*/
		Trademap = factory->createNamedMap<std::string,double>(tkn.vectorOfStrings[0],
				TradeValueNS);
		/* Incrementing two counts at each iteration since it is key,value pair*/
		for (int j = 1; j < tkn.vectorOfStrings.size(); j = j + 3) {
			double value = std::stod(tkn.vectorOfStrings[j + 1]);
			Facemap->insert(tkn.vectorOfStrings[j], value);
			value = std::stod(tkn.vectorOfStrings[j + 2]);
			Trademap->insert(tkn.vectorOfStrings[j],
					value);
		}
	}
	return i;
}

/* updateTradeValue
 *
 * function to update a value associated with the key
 * The update is done by del and then inserting
 *
 * */
void updateTradeValue(std::string line,
		shared_ptr<Namespace>& TradeValueNS) {
	/* string variable to store key values */
	std::string faceValue;
	/* variable to have key values */
	double tradevalue;
	double updatedvalue;
	/* Variables for map instance */
	shared_ptr<Map<std::string,double>> Trademap;
	/* Get the FTILib instance */
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();
	/* Get the MapFactory for creating the map */
	shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();

	keyGenerator rd = keyGenerator();
	tokenizeString tkn = tokenizeString(",");
	/* tokenizing the line/row */
	tkn.breakStringWithToken(line);
	displayWithProcessinfo("Hashmap name: ");
	cout << tkn.vectorOfStrings[0] << endl;
	Trademap = factory->createNamedMap<std::string,double>(tkn.vectorOfStrings[0], TradeValueNS);
	for (int j = 1; j < tkn.vectorOfStrings.size(); j = j + 3) {
		/* Get the value from the map for updating it with different value later*/
		tradevalue = Trademap->get(tkn.vectorOfStrings[j]);
		displayWithProcessinfo("Current value for key: ");
		cout << tradevalue << endl;
		/* Insert the updated value into the map */
		double val = rd.getRandomNum(10)*.1;
		updatedvalue+=tradevalue+val;
		Trademap->insert(tkn.vectorOfStrings[j], updatedvalue);
		displayWithProcessinfo("The value after update:");
		cout << Trademap->get(tkn.vectorOfStrings[j]) << endl;
	}
}
/* getValueFromMap
 * This function is to get the values from same named hashmap from
 * different namespace and display the values
 */
void getValueFromMap(std::string line, shared_ptr<Namespace> &FaceValueNS,
		shared_ptr<Namespace> &TradeValueNS) {
	/* double variables for the values */
	double faceValue, tradeValue;
	/* Variable for Map instance */
	shared_ptr<Map<std::string,double>> Facemap, Trademap;
	/* Get the FTILib instance */
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();
	/* Get the MapFactory instance for creating/retrieving named map */
	shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();
	/* Initializing the tokenizeString class with ',' */
	tokenizeString tkn = tokenizeString(",");
	/* tokenizing the line/record  */
	tkn.breakStringWithToken(line);

	/*Getting the instances of the named hashmap with different
	 * namespace reference
	 */
	Facemap = factory->createNamedMap<std::string,double>(tkn.vectorOfStrings[0], FaceValueNS);
	Trademap = factory->createNamedMap<std::string,double>(tkn.vectorOfStrings[0], TradeValueNS);

	displayWithProcessinfo("Hashmap name: ");
	cout << tkn.vectorOfStrings[0] << endl;
	for (int j = 1; j < tkn.vectorOfStrings.size(); j = j + 3) {
		/* Display the values retrieved from the same named hashmap
		 * in different namespace */
		faceValue = Facemap->get(tkn.vectorOfStrings[j]);
		tradeValue = Trademap->get(tkn.vectorOfStrings[j]);
		displayWithProcessinfo("Value for key: ");
		cout << tkn.vectorOfStrings[j] << endl;
		displayWithProcessinfo("In first namespace  :");
		cout << faceValue << endl;
		displayWithProcessinfo("In second namespace :");
		cout << tradeValue << endl << endl;
	}
}

int main() {
	int i = 0;
	/* Variable for random number */
	int random = 0;
	/* Vector to keep the lines/rows read from the file */
	std::vector < std::string > v;
	char displayBuf[1024] = "";
    /*  getting the globalconfig instance */
	shared_ptr<nsfti::GlobalConfig> config = FTILib::getGlobalConfig();
	/* setting the backup CPU number with globalconfig instance */
	config->setCPU(3);
	try {
        /* Initializing the FTIlib */
		std::shared_ptr<FTILib> initRet = FTILib::initialize();
		if (!initRet) {
			cout << "NSFTI library initialization failed." << endl;
			exit(-1);
		}
	} catch (char *err) {
		cout << "After init, init failed" << err << endl;
	}

	/* Get the FTILib */
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();

	/* Creating namespaces with FTILib instance */
	shared_ptr<Namespace> FaceValue = ftiLibInstance->createNamespace("FaceValue");
	shared_ptr<Namespace> TradeValue = ftiLibInstance->createNamespace(
			"TradeValue");
	displayWithProcessinfo("I am going to read from face_value.csv\n");
	/* Reading the csv file */
	v = readCSV("face_value.csv");
	if (!v.size()) {
		displayWithProcessinfo("There are no records to read\n");
		exit_demo();
	}
	sprintf(displayBuf, "%s%d%s", "I have read total records of : ", v.size(),
			"\n");
	displayWithProcessinfo(displayBuf);
	/* Generating named hashmaps in two namespaces*/
	i = generateMapWithNameSpace(v, FaceValue, TradeValue);
	sprintf(displayBuf, "%s%d%s",
			"I have inserted named hashmaps with namespace reference of :", i,
			"\n");
	displayWithProcessinfo(displayBuf);

	keyGenerator rd = keyGenerator();
	/* Run the loop for some iterations and retrieve the values from the
	 * named hashmap from different namespace */
	displayWithProcessinfo(
			"I am going to read hashmaps for values in different namespace\n");
	i = 0;
	while ((i++) < 2) {
		random = rd.getRandomNum(v.size());
		getValueFromMap(v[random], FaceValue, TradeValue);
	}

	displayWithProcessinfo(
			"I am going to update a value in  hashmaps \n");
	/* Get a random number */
	random = rd.getRandomNum(v.size());

	updateTradeValue(v[random], TradeValue);

	displayWithProcessinfo(
			"I am going to update a value in  hashmaps \n");
	/* Get a random number */
	random = rd.getRandomNum(v.size());
	updateTradeValue(v[random], TradeValue);

	exit_demo();
}

