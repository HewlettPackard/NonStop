# NSFTILib Sample Application : Data isolation feature of the library
This example demonstrates the data isolation feature of the library. __Data isolation__ allows clean demarcation between data structures and also provides a sandbox for keeping all the related data together.
Data isolation is achieved using the namespace infrastruture of the data structure library.
This example demostrates the use of `Namespace` API.

## What the application does ?

The application uses the Namespace & Map interface of the library. The application creates two hashmaps with _same name_ in different namespaces & performs `insert` and `get` operations on these hash maps. The `get` operation returning different values from the map demonstrate the isolation between the maps in different namespaces.

The example reads test data from a csv (comma separated values) file.Column structure of the CSV file

Column 1 | Column 2 | Column 3 | Column 4
-----|-------|-------|------
hashmap name | key name | value for map in namespace 1 | value for map in namespace 2

The below table is the list of features demonstrated:

Feature | Description
--------|------------
Initialize | `FTILib::initialize()` initializes the library and starts the backup process
Create a map factory | `FTILib::getInstance();` obtains an instance of the library. `instance->getMapFactory()` returns a map factory from which map data structures can be obtained
Create a name space | `FTILib::createNamespace();` creates a name space. If the namespace already exists it returns the existing instance of that namespace. 
Create a named map data structure within a namespace | `factory->createNamedMap<data type of key,data type of value>("name", "namespacehandle")` creates a named map in the namespace "namespacehandle". If the map already exists it returns the existing instance of that map.
Insert into map | `map->insert(key,value)` inserts `key` and associates a `value` with it 
Read from map | `map->get(key)` will return the value associated with the key
