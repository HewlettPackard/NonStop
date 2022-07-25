/*
 *  Copyright 2021 Hewlett Packard Enterprise Development LP
 *
 *     ALL RIGHTS RESERVED
 */

/*
 * Sample application to demonstrate high availability of hashmap.
 *
 * The application will create a hashmap and insert entries into
 *  the hashmap. This application performs user specified
 * insnsert and get operations on the hashmap.
 * The high availability of the hashmap can be tested by stopping
 * one of the process pair and observing the application status.
 * User inputs to the application are provided by a predefined file
 * called config.txt which is present in the current directory.
 *
 * Configuration file :
 * The entries in the configuration file are key-value pairs.
 * Each key-value pair is separated by a colon (':').
 * Configuration parameter should be one per line.
 * The application expects the following configuration parameters
 * 1.templateKeyString
 * 		is the key pattern that the application should use to identify
 * 		the keys in the hashmap. The application will generate a key name
 * 		with this pattern.
 * 2.templateValueString
 * 		is the value pattern that the application should use to generate
 * 		the values to store in the hashmap.
 * 3.GetPercentage
 * 		ratio of hash get operations.
 * 4.InsertPercentage
 * 		ratio of hash insert operations.
 *
 * After the primary process fail over the application will verify
 * the data integrity of the keys.
 *
 * The application will print, the number of
 * get and insert operations completed.
 */

/* ftilib.h is the only header user need to include for compiling the
 * app to use the NSFTI library infrastructure.
 */
#include <memory>
#include <iostream>
#include <nsfti/ftilib.h>
using namespace std;

/* utility API header file */
#include "utils.h"

/* namespace to be included while we use NSFTIlibrary */
using namespace nsfti;

int dummy;
/* preLoadStatus()
 * Function to check for the preload status of the nsftilib
 * The function checks for a predefined key called "demoKeyName".
 * If key exists then preload is complete, continue with rest of
 * the application logic.
 * If the key does not exist preload has to be performed.
 */
bool preLoadStatus() {
	bool ret = false;
	const std::string mapName = "demoMapName";
	const std::string keyName = "demoKeyName";
	const std::string valueString = "demoValueString";
	std::string valueFromMap;
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();
	shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();
	shared_ptr<Map<std::string,std::string>> mapping;
	try {
		mapping = factory->createNamedMap<std::string,std::string>(mapName);
	} catch (char *err) {
		cout << "After map create failed" << err << endl;
		exit(-1);
	}

	try {
		/* get the predefined key  */
		valueFromMap = mapping->get(keyName);
		ret = true;
	} catch(std::exception& e)  {
		ret = false;
		if(!mapping->insert(keyName, valueString)){
			cout<<"Value is not inserted in preload"<<endl;
		}

	}

	return ret;
}

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
/* Variable to keep the number of set and get operations */
int setOperation = 0, getOperation = 0,delOperation=0;


/* statusUpdate
 * This function will print out the status of the operations
 * completed with the <cpu,pin> information.
 * The update will get printed in the same line
 */
void statusUpdate(short cpu, short pin, int get, int set,int del) {
	int static oldTransaction=0;
	cout << "\r";
	cout << "<" << cpu << "," << pin << "> ";
	cout << "Performed \"GET\"=" << get << " \"INSERT\"=" << set <<" \"ERASE\"=" << del <<" \"TPS\"="<<(get+set+del)-oldTransaction ;
	oldTransaction=get+set+del;
	fflush(stdout);
}
/* Function to report the */
void statusReporter (int signum)
{
	/* To get the current process information */
	static short pCpu=0,pPin=0;
	static std::string pName;
	if(!pPin){
		processInfo info = processInfo();
		info.getProcessInfo(pName, pCpu, pPin);
	}
	statusUpdate(pCpu,pPin,getOperation,setOperation,delOperation);
}

int main(int argc, char *argv[]) { /* argc and argv not used */

	/* Map name string to be used for creating named map */
	const std::string mapName = "demoMapName";

	/* String variables to store */
	std::string keyGenerated, ValueGenerated, valueFromMap;
	/* String variables to read the configuration parameters */
	std::string getPerString, setPerString,delPerString;
	/*string variable to get the value from the map*/
	std::string value, pName;
	std::string line, templateValueString, templateKeyString;

	int i = 0;
	/*variables to keep the integer version of the configuration parameters */
	int getPer, setPer,delPer;
	/*variable to have the ramdom number */
	int randomNum;
	short pCpu, pPin;

	char displayBuf[1024] = "";

	/* To get the current process information */
	processInfo info = processInfo();
	info.getProcessInfo(pName, pCpu, pPin);
	/* Initialize the configuration file */
	configReader appConf = configReader("config.txt");
	/* Read the configuration strings from the file */
	templateKeyString = appConf.readConfigParameter("templateKeyString");
	templateValueString = appConf.readConfigParameter("templateValueString");
	getPerString = appConf.readConfigParameter("GetPercentage");
	setPerString = appConf.readConfigParameter("InsertPercentage");
	delPerString = appConf.readConfigParameter("erasePercentage");
	/* Check for the validity of the read strings from the configuration file */
	if (!(templateKeyString.size()) || !(templateValueString.size())
			|| !(getPerString.size()) || !(delPerString.size()) || !(setPerString.size())) {
		displayWithProcessinfo(
				"not all parameters mentioned in input parameter file\n");
		return 0;
	}
	/* Convert the read configuration parameter into integers */
	getPer = atoi(getPerString.c_str());
	setPer = atoi(setPerString.c_str());
	delPer = atoi(delPerString.c_str());

	/* Update the template key/value string with the class methods */
	keyGenerator keyGen = keyGenerator();
	if (!keyGen.updateBaseValuestring(templateValueString))
		displayWithProcessinfo("Updating base value string failed");
	if (!keyGen.updateBasekeystring(templateKeyString))
		displayWithProcessinfo("Updating base key string failed");


	shared_ptr<nsfti::GlobalConfig> nsftiConfig = FTILib::getGlobalConfig();
	/* Set back-up cpu configuration */
	nsftiConfig->setCPU(3); 


	nsftiConfig->setProgramArgs(argc, argv);
	struct timeval begin, end;
	gettimeofday(&begin, NULL);

	try {
		/*initialize the NSFTILib*/
		std::shared_ptr<FTILib> initRet = FTILib::initialize();
		if (!initRet) {
			displayWithProcessinfo("NSFTI library initialization failed.");
			exit(-1);
		}
	} catch (char *err) {
		sprintf(displayBuf, "After init, init failed: %s", err);
		displayWithProcessinfo(displayBuf);
	}

	/* Get the NSFTI instance */
	shared_ptr<FTILib> ftiLibInstance = FTILib::getInstance();

	/* Get factory instance to create the map */
	shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();

	/* Variable for NSFTI Map instance */
	shared_ptr<Map<std::string,std::string>> mapping;

	/* Create a named map. Returns a NSFTI Map instance
	 * If the map is already existing it will be returned else
	 * a new map instance is created and returned
	 */
	try {
		mapping = factory->createNamedMap<std::string,std::string>(mapName);
	} catch (char *err) {
		displayWithProcessinfo("After map create failed");
		sprintf(displayBuf, "%s", err);
		displayWithProcessinfo(displayBuf);
		exit(-1);
	}

	/* Preload data for total iterations */
	if (!preLoadStatus()) {
		displayWithProcessinfo(
				"My business logic says that I am the primary process\n");
		displayWithProcessinfo(
				"Now I will continuously perform \"GET\" and \"INSERT\" "
						"operations in the predefined ratio, and I will "
						"print the number of operations performed\n");

		/* This loop will run for ever until the failover happens
		 * depending on the percentage of the set/get configured
		 * it will do the operations
		 */
		setOperation = 0;
		getOperation = 0;
		delOperation = 0;
		int noset=1;
		timerHandler(statusReporter,1);
		while (1) {
			keyGenerated.erase();
			ValueGenerated.erase();
			randomNum = keyGen.getRandomNum(i);
			if (((i > 100) & (randomNum % 100) < getPer) && noset) {
				/* For get operation it will randomly pick one key to be verified
				 * out of the inserted keys
				 */
				keyGen.generateKeyWithIndex(randomNum, keyGenerated);
				keyGen.generateValueWithIndex(randomNum, ValueGenerated);
				/* Get the value of the key inserted */

				try {
					/* get the predefined key  */
					value = mapping->get(keyGenerated);

				} catch(std::exception& e)  {
					cout<<"No value for "<<keyGenerated<<endl;
					continue;
				}

				/* Compare the value string that just got */
				if (value.compare(ValueGenerated)) {
					/* If the key is not matching throw an error message and breaking
					 *  the loop
					 */
					displayWithProcessinfo(
							"The value got from the nsfti map does not match");
					break;
				} else {
					/* update the get operation count for every successful operation */
					getOperation++;
				}
			} else if ((i > 100) & (randomNum % 100) < (getPer+delPer)) {

				/* Generate the keystring and value string w.r.t the index */
				keyGen.generateKeyWithIndex(i-1, keyGenerated);
				if (!mapping->erase(keyGenerated))
					cout<<"erase failed"<<endl;
				else {
					/* Update the set operation count */
					delOperation++;
					i--;
				}

			} else {
				/* Generate the keystring and value string w.r.t the index */
				keyGen.generateKeyWithIndex(i, keyGenerated);
				keyGen.generateValueWithIndex(i, ValueGenerated);
				try {
					if (!mapping->insert(keyGenerated, ValueGenerated)){
						cout<<"Value is not inserted"<<endl;
					}
				}catch(std::runtime_error& e)  {
					displayWithProcessinfo("\n\ncaught at insert run time exception for memory\n");
					//keyGen.generateKeyWithIndex(i-1, keyGenerated);
					//mapping->erase(keyGenerated);
					noset=0;
					break;
				}
				try {
					/* get the predefined key  */
					value = mapping->get(keyGenerated);

				} catch(std::exception& e)  {
					cout<<"No value just after the insert:"<<keyGenerated<<endl;
				}
				/* Update the set operation count */
				setOperation++;
				i++;
			}
			//statusUpdate(pCpu, pPin, getOperation, setOperation,delOperation);
		}
	}

	else {
		cout << endl;
		displayWithProcessinfo(
				"My business logic says that there was a "
						"primary and I was the backup. But the primary seems to have died"
						" and am taking over as the new primary\n");
		displayWithProcessinfo(
				"I will check the number of keys in my database and validate"
						" the 'values' as per the template\n");
		i = 0;
		setOperation = 0;
		getOperation = 0;
		while (1) {
			keyGen.generateKeyWithIndex(i, keyGenerated);
			keyGen.generateValueWithIndex(i, ValueGenerated);

			try {
				/* get the predefined key  */
				value = mapping->get(keyGenerated);

			} catch(std::exception& e)  {
				break;
			}

			if (value.compare(ValueGenerated)) {
				break;
			} else {
				/* Update the successful get/verification of the keys from the map */
				getOperation++;
			}
			i++;
		}
	}

	sprintf(displayBuf, "I found %d of keys in my database and I also "
			"verified the values to match the given template%s", i - 1, "\n");
	displayWithProcessinfo(displayBuf);
	displayWithProcessinfo("I will gracefully shutdown in 60 seconds\n");
	sleep(60);
	/* API to be called for the graceful exit of the application */
	ftiLibInstance->shutdown(EXIT_SUCCESS);
}

