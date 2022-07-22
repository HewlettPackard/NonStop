# What is NSFTI?
NonStop offers multiple ways of writing fault tolerant applications. One of the earliest technologies of programming fault tolerant application is using process pairs. Writing an application using process-pair technology can be quite daunting as it expects the programmer to have a deep understanding of several NonStop specific programming aspects such as data checkpointing, primary and backup processes, the File System (FS) API and handling process pair signals to name a few. NSFTI (NonStop Fault Tolerant Interface) is a C++ library that hides the process pair programming complexity in a few C++ classes. The classes are designed for use by any C++ programmer without any NonStop programming experience.

The library provides a Map data structure whose contents are always kept in sync with the backup. The programmer just needs to store the application data that needs to be checkpointed, in the Map. The Map data structure is implemented as a C++ class with method signatures very similar to that of the C++ STL (standard template library).

# How do I know if the product is installed on my NonStop system?
NSFTI is part of suite of products packaged together under the name "NonStop middleware tools" and is shipped as T1150. It is installed in the directory '/usr/tandem/middlewaretools'

# Where can I find the documentation for this product?
The product documentation can be found at https://support.hpe.com/hpesc/public/docDisplay?docId=a00056322en_us&page=index.html

# About the examples
Here are a few example applications to help you get started with NSFTI.
* Getting started
  - If you are completely new to NSFTI and want to try out a sample application you should try the _dataconsistency_ha_ example. This example demonstrates not just the programmatic usage of NSFTI library, but it also demonstrates data consistency and HA features of the library
* OLTP
  - If you are looking at a fault tolerant server application servicing requests from an off platform client then the _oltp_ example is the right place to get you started
* Data Isolation
  - If you are looking at using the namespace feature of NSFTI to have separate buckets for your data then _nsfti_namespaces_ example is the place to get started
* Client Server on NonStop
  - If you are looking to write a process pair to be consumed by a NonStop client applications (it could either be an FS client or a pathsend client), then look no further than the example _dollar_recv_ipc_. Here you have a server application that uses NSFTI, receives IPC messages on $RECEIVE and when the primary fails the backup opens its $RECEIVE to provide uninterrupted operations. The exmaple also has a sample FS client

# Note to developers
Ensure that the _RLD_LIB_PATH is set to the lib folder where the `libnsfti.so` is present and the executable are in the folder before running the examples.
This directory contains the example applications that demonstrate the features & usage of nsfti library
# Each example directory is organized as following
*   bin
>    -   This directory contains the example application executable files which use the nsfti library.
*   include
>    -   This directory contains the header files required by the example application.
*   src
>    -   This directory contains the source files (C++) for example application.
*   Makefile
>    -   Makefile to manage the build of the example.
*   README.md
>    -   Description of the example application.


