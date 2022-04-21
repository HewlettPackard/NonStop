#
#  Copyright 2021 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

CURRENT=.
TARGET=$(CURRENT)/bin/ISO-App

LIB_FOLDER= $(CURRENT)/lib/

OPT=O0
DATA_MODEL=Wlp64

SRCS := $(wildcard $(CURRENT)/src/*.cpp)
OBJS := $(SRCS:.cpp=.o)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $< 

$(TARGET): $(OBJS)	
	$(CXX)  -o $(TARGET) $(CXXFLAGS) $(OBJS) $(LIBUTIL) $(FTILIB) \
		-Wsaveabend $(CXXLIBS)


all:	$(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
