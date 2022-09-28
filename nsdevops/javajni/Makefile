# JAVA_HOME should be set to the directory where java has been installed.
JAVA_HOME = /usr/tandem/nssjava/jdk180_l80

# JREHOME need only be set if java has not been installed in the default
# location of /usr/tandem/nssjava/jdk180_l80.
#JREHOME = $(JAVA_HOME)/jre

CC=/usr/bin/c89
ELD=/usr/bin/xld

CFLAGS= -g -I $(JAVA_HOME)/include -I $(JAVA_HOME)/include/oss -I . \
	 -I /G/system/system -I /G/system/zsysdefs -I /G/system/zspidef \
        -Wallow_cplusplus_comments -Wextensions -D_XOPEN_SOURCE_EXTENDED=1 \
	  -Wnowarn=141,209 -D_PUT_MODEL_ \
      -Wcall_shared -Wsystype=oss -Wtarget=tns/x

LDFLAGS = -set floattype IEEE_float -set SYSTYPE OSS -set HIGHPIN ON \
          -set CPlusPlusDialect neutral -dll \
	   -L Pthreads -lxputdll\ \
          -lcre -lcrtl -export_all

C64FLAGS= -g -I $(JAVA_HOME)/include -I $(JAVA_HOME)/include/oss -I . \
         -I /G/system/system -I /G/system/zsysdefs -I /G/system/zspidef \
         -Wallow_cplusplus_comments -Wlp64 \
        -Wextensions -D_XOPEN_SOURCE_EXTENDED=1 -Wnowarn=141,209  -D_PUT_MODEL_ \
        -Wcall_shared -Wsystype=oss -Wtarget=tns/x

LD64FLAGS = -set data_model lp64 -set floattype IEEE_float -set SYSTYPE OSS -set HIGHPIN ON \
          -set CPlusPlusDialect neutral -dll \
          -L Pthreads -lxputdll\
          -lcre -lcrtl -export_all \
          
          

JAVAC = $(JAVA_HOME)/bin/javac
JAVAH = $(JAVA_HOME)/bin/javah
JAVADOC = $(JAVA_HOME)/bin/javadoc
JAR = $(JAVA_HOME)/bin/jar


RM= /bin/rm -f

ALL = quickstart

.SUFFIXES: .java .class .h .c .o

all: $(ALL)

clean:
	@$(RM) *.o *.so com/hpe/nsk/nsdevops/client/*.class com/hpe/nsk/nsdevops/server/*.class com/hpe/nsk/nsdevops/server/test/*.class


JavaHelloClient.class: com/hpe/nsk/nsdevops/client/JavaHelloClient.java
	@echo "-----------------------------------"
	@echo "Compiling JavaHelloClient Program 
	@echo "-----------------------------------"
	$(JAVAC)  -cp . com/hpe/nsk/nsdevops/client/JavaHelloClient.java

JavaHelloServer.class: com/hpe/nsk/nsdevops/server/JavaHelloServer.java
	@echo "-----------------------------------"
	@echo "Compiling JavaHelloServer Program 
	@echo "-----------------------------------"
	$(JAVAC)  -cp . com/hpe/nsk/nsdevops/server/JavaHelloServer.java
	$(JAVAC)  -cp .:junit.jar:org.hamcrest.core_1.1.0.v20090501071000.jar  com/hpe/nsk/nsdevops/server/test/JavaHelloServerTest.java

JavaHelloServer.h: JavaHelloServer.class
	@echo "-----------------------------------"
	@echo "Generating the JNI header file  
	@echo "-----------------------------------"
	$(JAVAH)  -jni com.hpe.nsk.nsdevops.server.JavaHelloServer

	@echo "-----------------------------------"
	@echo "Generating the Server Jar file  
	@echo "-----------------------------------"
javajnihelloserver.jar: JavaHelloServer.class
	$(JAR)	-cvf javajnihelloserver.jar com/hpe/nsk/nsdevops/server/*.class  com/hpe/nsk/nsdevops/server/test/*.class


JavaHelloServer.o: javahelloserver.c
	$(CC) $(CFLAGS) -WIEEE_float -c  javahelloserver.c 


libjavahelloserver.so: JavaHelloServer.o
	@echo "-------------------------------------"
	@echo "Generating libjavahelloserver.so file  
	@echo "-------------------------------------"
	$(ELD) javahelloserver.o  -o libjavajnihelloserver.so $(LDFLAGS)


C_test: JNIHelloServerTest.cpp
	@echo "-------------------------------------------------------------"
	@echo "Compiling JNIHelloServerTest.cpp Program with Boost Framework" 
	@echo "-------------------------------------------------------------"
	c99 -Wcplusplus -I/usr/boost_1_56 -g -O1 -Wsystype=oss -Wboost -Wcall_shared -c -L/usr/boost_1_56/libv3 -lboostx_unit_test_framework -llibboostx_prg_exec_monitor.a JNIHelloServerTest.cpp 
	c99 -Wcplusplus -I/usr/boost_1_56 -g -O1 -Wsystype=oss -Wboost -Wcall_shared -L/usr/boost_1_56/libv3 -lboostx_unit_test_framework -llibboostx_prg_exec_monitor.a JNIHelloServerTest.o  -o test.exe 


quickstart:JavaHelloClient.class JavaHelloServer.class JavaHelloServer.h libjavahelloserver.so javajnihelloserver.jar C_test run_java_test run_C_test

run_java_test:
	@echo "\n----------------------------------------------------------"
	@echo "Running JUnit library tests with Boost Unit Test Framework"
	@echo "----------------------------------------------------------"
	export _RLD_LIB_PATH=.:/G/system/system; \
	$(JAVA_HOME)/bin/java -cp javajnihelloserver.jar:junit.jar:org.hamcrest.core_1.1.0.v20090501071000.jar com.hpe.nsk.nsdevops.server.test.JavaHelloServerTest

run_C_test:
	@echo "\n------------------------------------------------------"
	@echo "Running C library tests with Boost Unit Test Framework"
	@echo "------------------------------------------------------"
	export _RLD_LIB_PATH=.:/G/system/system; \
	./test.exe --show_progress --report_level=detailed

