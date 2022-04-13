# NSFTILib Sample Application : Data consistency and high availability

This application demonstrates the following key features of the library

*   High Availability
>    -   Definition: The application is available at all times and can survive a single point of failure
*   Data Consistency
>    -   Definition: At any point in time, the library returns the exact same result for a given operation and the result does not vary across failures

It also demonstrates how an HA application can be written without any knowledge of the underlying process pair technology.

## What the application does ?

The application uses the Map interface of the library and continuously performs ___get___ , ___erase___ and ___insert___ operations in a configured ratio. When the primary of the pair is killed (manually) the backup takes over. This demonstrates High Availability of the application. 

When the backup takes over it counts the number of keys in its memory store and also validates the value stored in each key. The primary generates the key-value pairs based on the templates in the configuration file (`config.txt`). The backup fetches the data from its memory store and validates if they match the template. This demonstrates data remains consistent across failures.

## Programmer remains unaware of the process pair technology in use

A cursory glance at _example1.cpp_ reveals the programmer has absolutely no knowledge of the underlying process pair technology used for high availability. The program flow is explained in brief with relevant code snippets

### Read application configuration
The application first reads the configuration file _config.txt_. It has to be noted that this is not required by the library and is just part of the application logic.
    `configReader conf = configReader("data.txt");`
    `conf.readConfigParameter("templateKeyString");`
    `conf.readConfigParameter("templateValueString");`
    `conf.readConfigParameter("GetPercentage");`
    `conf.readConfigParameter("InsertPercentage");`
    `conf.readConfigParameter("erasePercentage");`

The _templateKeyString_ is used to generate __key__ to insert. The primary and the backup both use this template to generate the key. Similarly _templateValueString_ is used to generate the __value__ for the key to be inserted in the Map. 

The _GetPercentage_ , _ErasePercentage_and _InsertPercentage_ values indicate the ratio in which the operations need to be performed.

### Initialize the library
The following code initializes the library
`shared_ptr<nsfti::GlobalConfig> config = FTILib::getGlobalConfig();`
`config->SetCPU(3);`
`FTILib::initialize();`
Setting the CPU tells the library which is the other CPU to consider for creating backup. The CPU on which the primary is started is always considered as one of the CPUs. A successful invocation of the `initialize` method starts the backup process.

#### A note about `initialize` method
When the library detects the running process is the backup of the pair, it blocks at the `initialize` method. The method returns only after the primary fails and the backup is promoted as the primary. 

### Perform `Map` operations
The programmer first gets an instance of the library by invoking `FTILib::getInstance()`. From the instance a map factory is obtained by invoking `ftiLibInstance->getMapFactory()`. From the factory map instances are created by `factory->createNamedMap<std::string,std::string>("employees")`.Programer have to specify the data type since the methods suport templates. Values are inserted into the map by `map->insert(key, value)`. Values are read from the map by `map->get(key)`

### Graceful exit
To indicate a graceful exit of the application to the backup process `imclib->shutdown(EXIT_SUCCESS);` should be invoked.

## Programmer tips 
As can be seen from the above description a programmer can remain agnostic of the underlying process pair technology. The conventional NonStop process pair programming best practice requires the backup process to listen for the primary failure (through system messages) and then begin its work.

The NSFTILib allows the programmer to just check the application state and start off from the current state. Here the application state is stored in the data structures of the library.

